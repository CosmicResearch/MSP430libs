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

IridiumSBD::IridiumSBD(Serial* serial, uint8_t sleepPin, uint_t ringPin, uint32_t baudRate = 19200) {
    this->serial = serial;
    this->sleepPin = sleepPin;
    this->ringPin = ringPin;
    this->powered = false; 
    countdown = new Countdown();
    this->baudRate = baudRate;
}

void IridiumSBD::start() {

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
    error_t result = waitForATResponse("OK\r");

    if (modemAlive != SUCCESS) {
        countdown->release();
        onStartDone(ERROR);
        return;
    }

    onStartDone(SUCCESS);
}

error_t IridiumSBD::waitForATResponse(const char* terminator) {
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
            if (++i >= strlen(terminator) && terminator == repl) {
                countdown->release();
                return SUCCESS;
            }
        }

    } while(!countdown->has_expired());
    countdown->release();
    RETURN ERROR;
}

void IridiumSBD::powerOn() {
    if (!powered) {
        digitalWrite(sleepPin, HIGH);
        powered = true;
    }
}

void IridiumSBD::powerOff() {
    if (powered) {
        digitalWrite(sleepPin, LOW);
        powered = false;
    }
}

void IridiumSBD::send(const char* text) {
    serial->print(text);
}

void IridiumSBD::send(uint16_t number) {
    serial->print(number);
} 