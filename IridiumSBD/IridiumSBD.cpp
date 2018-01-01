/*
    Bondar libraries
    Copyright (C) 2017  Associació Cosmic Research

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "IridiumSBD.h"
#include "WString.h"

IridiumSBD::IridiumSBD(Serial* serial, uint8_t sleepPin, uint8_t ringPin, uint32_t baudRate) {
    this->serial = serial;
    this->sleepPin = sleepPin;
    this->ringPin = ringPin;
    this->powered = false; 
    countdown = new Countdown();
    this->baudRate = baudRate;
}

IridiumSBD::IridiumSBD(Serial* serial, uint8_t sleepPin, uint8_t ringPin) {
    this->serial = serial;
    this->sleepPin = sleepPin;
    this->ringPin = ringPin;
    this->powered = false;
    countdown = new Countdown();
    this->baudRate = 19200;
}

error_t IridiumSBD::start() {

    pinMode(sleepPin, OUTPUT);
    pinMode(ringPin, INPUT);

    serial->begin(this->baudRate);

    this->powerOn();

    countdown->request();

    uint16_t startUpTime = 500;
    countdown->set_ms(startUpTime);
    while(!countdown->has_expired());
    countdown->release();

    send("AT\r");
    if (waitForATResponse("OK\r") != SUCCESS) {
        return ERROR;
    }

    send("AT&D0\r");
    if (waitForATResponse("OK\r") != SUCCESS) {
        return ERROR;
    }

    send("AT&K0\r");
    if (waitForATResponse("OK\r") != SUCCESS) {
        return ERROR;
    }

    if (this->baudRate != 19200) {
        const String command = String("AT+IPR=") + String(this->baudRate) + String('\r');
        send(command);
        if (waitForATResponse("OK\r") != SUCCESS) {
            return ERROR;
        }
    }
    onStartDone(SUCCESS);
}

error_t IridiumSBD::stop() {
    return this->powerOff();
}

error_t IridiumSBD::sendText(const char* text) {
    return sendText(String(text));
}

error_t IridiumSBD::sendText(const String& text) {
    if (text.length() > ISBD_MAX_MESSAGE_LENGTH) {
        return ERROR;
    }
    send("AT+SBDWT\r");
    if (waitForATResponse("READY\r\n") != SUCCESS) {
        return ERROR;
    }
    send(text + "\r");
    if (waitForATResponse("0\r\n\r\nOK\r\n") != SUCCESS) {
        return ERROR;
    }
    uint16_t moStatus, moMSN, mtStatus, mtMSN, mtLength, mtQueued;
    if (doSBDIX(moStatus, moMSN, mtStatus, mtMSN, mtLength, mtQueued) != SUCCESS) {
        return ERROR;
    }
    if (moStatus >= 0 && moStatus <= 4) {
        if (onSendDone) {
            onSendDone(SUCCESS);
        }
        return SUCCESS;
    }
    else {
        if (onSendDone) {
            onSendDone(ERROR);
        }
        return ERROR;
    }
}

error_t IridiumSBD::sendBinary(const uint8_t* txData, size_t size) {
    if (size > ISBD_MAX_MESSAGE_LENGTH) {
        return ERROR;
    }
    send(String("AT+SBDWB=") + size);
    if (waitForATResponse("READY\r\n") != SUCCESS) {
        return ERROR;
    }
    uint16_t checksum = 0;
    for (int i = 0; i < size; ++i) {
        serial->write(txData[i]);
        checksum += (uint16_t)txData[i];
    }
    serial->write(checksum >> 8);
    serial->write(checksum & 0x00FF);
    if (waitForATResponse("0\r\n\r\nOK\r\n") != SUCCESS) {
        return ERROR;
    }
    uint16_t moStatus, moMSN, mtStatus, mtMSN, mtLength, mtQueued;
    if (doSBDIX(moStatus, moMSN, mtStatus, mtMSN, mtLength, mtQueued) != SUCCESS) {
        return ERROR;
    }
    if (moStatus >= 0 && moStatus <= 4) {
        if (onSendDone) {
            onSendDone(SUCCESS);
        }
        return SUCCESS;
    }
    else {
        if (onSendDone) {
            onSendDone(ERROR);
        }
        return ERROR;
    }
}

error_t IridiumSBD::waitForATResponse(const String& terminator) {
    countdown->request();
    countdown->set_ms(1000);
    String repl;
    int i = 0;
    do {

        while (serial->available()) {
            char c = serial->read();
            if (terminator[i] == c) {
                repl.concat(c);
            }
            if (++i >= terminator.length() && terminator == repl) {
                countdown->release();
                return SUCCESS;
            }
        }

    } while(!countdown->has_expired());
    countdown->release();
    return ERROR;
}

error_t IridiumSBD::waitForATResponse(String& response, const String& prompt) {
    countdown->request();
    countdown->set_ms(1000);
    int promptPos = 0;
    enum {
        LOOKING_FOR_PROMPT,
        LOOKING_FOR_RESPONSE
    };
    int state = LOOKING_FOR_PROMPT;
    do {
        while(serial->available()) {
            char c = serial->read();
            switch(state) {

                case LOOKING_FOR_PROMPT:
                    if (c == prompt[promptPos]) {
                        ++promptPos;
                        if (promptPos >= prompt.length()) {
                            state = LOOKING_FOR_RESPONSE;
                        } 
                    }
                    else {
                        promptPos = c == prompt[0]? 1 : 0;
                    } 
                    break;
                case LOOKING_FOR_RESPONSE:
                    if (c == '\r') {
                        countdown->release();
                        return SUCCESS;
                    }
                    response.concat(c);
                    break;
            }
        }
    } while(!countdown->has_expired());
    countdown->release();
    return ERROR;
}

error_t IridiumSBD::doSBDIX(uint16_t& moStatus, uint16_t& moMSN, uint16_t& mtStatus, uint16_t& mtMSN, uint16_t& mtLength, uint16_t& mtQueued) {
    String response;
    send("AT+SBDIX\r");
    if (waitForATResponse(response, "+SBDIX: ") != SUCCESS) {
        return ERROR;
    }
    char buff[100];
    response.toCharArray(buff, sizeof(buff));
    uint16_t *values[6] = {&moStatus, &moMSN, &mtStatus, &mtMSN, &mtLength, &mtQueued};
    for (int i = 0; i < 6; ++ i) {
        char *value = strtok(buff, ", ");
        if (value == NULL) {
            return ERROR;
        }
        *values[i] = atol(value);
    }
    return SUCCESS;
}

error_t IridiumSBD::powerOn() {
    if (!powered) {
        digitalWrite(sleepPin, HIGH);
        powered = true;
        return SUCCESS;
    }
    return ERROR;
}

error_t IridiumSBD::powerOff() {
    if (powered) {
        digitalWrite(sleepPin, LOW);
        powered = false;
        return SUCCESS;
    }
    return ERROR;
}

void IridiumSBD::send(const char* text) {
    serial->print(text);
}

void IridiumSBD::send(const String& text) {
    serial->print(text);
}

void IridiumSBD::send(uint16_t number) {
    serial->print(number);
} 

bool IridiumSBD::isAsleep() {
    return powered;
}

void IridiumSBD::attachStartDone(void (*function)(error_t)) {
    this->onStartDone = function;
}

void IridiumSBD::attachStopDone(void (*function)(error_t)) {
    this->onStopDone = function;
}

void IridiumSBD::attachSendDone(void (*function)(error_t)) {
    this->onSendDone = function;
}
    
void IridiumSBD::attachReceiveDone(void (*function)(const uint8_t*, size_t, error_t)) {
    this->onReceiveDone = function;
}
