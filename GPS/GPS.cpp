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

#include "GPS.h"
#include <iostream>

#define MAXLENGTH 120

volatile char lineBuffer1[MAXLENGTH];
volatile char lineBuffer2[MAXLENGTH];

volatile uint8_t lineIndex = 0;
volatile char* currentLine;
volatile char* lastLine;

/*
PUBLIC METHODS
*/

GPS::GPS(Serial* serial, const uint32_t baudRate) {
    GPS::serial = serial;
    GPS::baudRate = baudRate;
    GPS::state = {S_IDLE, false, false};
    GPS::lastData = new gps_data_t;
    currentLine = lineBuffer1;
    GPS::onReadDone = NULL;
    GPS::onStartDone = NULL;
    GPS::onStopDone = NULL;
}

error_t GPS::start() {
    serial->attachReceive(onSerialReceive);
    state.isStarted = true;
    return SUCCESS;
}

error_t GPS::stop() {
    serial->attachReceive(NULL);
    return SUCCESS;
}

error_t GPS::read() {
    /*if (state.isStarted) {
        if (state.request == S_IDLE) {
            state.request = S_READ;
            return resource->request();
        }
        return EBUSY;
    }*/
    return ERROR;
}

error_t GPS::readNow() {
    /*if (state.isStarted) {
        if (state.request == S_IDLE) {
            state.request = S_READ_NOW;
            state.isReady = false;
            return resource->immediateRequest();
        }
        return EBUSY;
    }*/
    return ERROR;
}

boolean_t GPS::isStarted() {
    return state.isStarted;
}

/*
CALLBACKS
*/

void GPS::onSerialSendDone() {}

void GPS::onSerialReceive(uint8_t data) {
    if (data == '\n') {
        currentLine[lineIndex] = 0;
        if (currentLine == lineBuffer1) {
            currentLine = lineBuffer2;
            lastLine = lineBuffer1;
        }
        else {
            currentLine = lineBuffer1;
            lastLine = lineBuffer2;
        }
        lineIndex = 0;
        gps_data_t* gps_data = new gps_data_t;
        bool correct = processLine((char*)lastLine, gps_data);
        if (!correct) {
            if (onReadDone)
                onReadDone(NULL, ERROR);
            return;
        }
        else  {
            lastData = gps_data;
            if (onReadDone != NULL)
                onReadDone(gps_data, SUCCESS);
        }
        return;
    }
    currentLine[lineIndex++] = data;
    if (lineIndex >= MAXLENGTH) {
        lineIndex = MAXLENGTH - 1;
    }
}

/*
SETTERS FOR THE CALLBACKS
*/

void GPS::attachStartDone(void(*function)(error_t)) {
    onStartDone = function;
}

void GPS::attachReadDone(void(*function)(sensor_data_t *, error_t)) {
    onReadDone = function;
}

void GPS::attachStopDone(void(*function)(error_t)) {
    onStopDone = function;
}

gps_data_t GPS::getLastData() {
    return *lastData;
}

/*
PRIVATE METHODS
*/

void GPS::sendCommand(char* command) {
    serial->println(command);
}

uint8_t GPS::charHexToInt(char c) {
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    else {
        return c - '0';
    }
}

uint16_t GPS::stringToInt(char* &c) {
    uint16_t ret = 0;
    while(*c >= '0' && *c <= '9') {
        ret = ret*10 + (*c - '0');
        ++c;
    }
    return ret;
}

float_t GPS::stringToFloat(char* &c) {
    float_t integerPart = 0.0f;
    float_t decimalPart = 0.0f;
    float_t divisor = 1.0f;
    bool isInDecimalPart = false;
    int sign = 1;
    if (*c == '-') {
        sign = -1;
        ++c;
    }
    else if (*c == '+') {
        ++c;
    }
    while ((*c >= '0' && *c <= '9') || *c == '.') {
        if (isInDecimalPart) {
            decimalPart = (decimalPart*10) + (*c - '0');
            divisor *= 10;
        }
        else if (*c == '.') {
            isInDecimalPart = true;
        }
        else {
            integerPart = (integerPart*10) + (*c - '0');
        }
        c++;
    }
    return float_t(integerPart + (decimalPart/divisor)*sign);
}

uint32_t GPS::stringToDegreesIn1000000ths(char* &c) {
    uint32_t integerPart = stringToInt(c);
    uint32_t minutesIn100000ths = (integerPart % 100) * 100000;
    if (*c++ == '.') {
        uint32_t multiplier = 10000;
        while (*c >= '0' && *c <= '9') {
            minutesIn100000ths += multiplier * (*c - '0');
            multiplier /= 10;
            ++c;
        }
    }
    return (integerPart / 100) * 1000000 + (minutesIn100000ths + 3) / 6;
}

uint32_t GPS::stringToFloatIn100ths(char* &c) {
    uint32_t ret = stringToInt(c) * 100;
    if (*c++ == '.') {
        uint8_t multiplier = 10;
        while (*c >= '0' && *c <= '9' && multiplier >= 1) {
            ret += multiplier * (*c - '0');
            multiplier /= 10;
            ++c;
        }
    }
    return ret;
}

uint8_t GPS::charIntToInt(char c) {
    return c - '0';
}

/**
 * Parses the line passed as argument
 * Returns if the line is correct and saves the parsed data to 'data'
 */
bool GPS::processLine(char* line, gps_data_t* data) {
    int size = strlen(line);
    if(line[0] != '$') { //NMEA 0183 messages start with '$'
        return false;
    }
    if(line[size-3] != '*') {
        return false;
    } 
    uint8_t checksum = charHexToInt(line[size-2])*16 + charHexToInt(line[size-1]);
    uint8_t parity = 0;
    char messageType[4];
    for (int i = 1; i < (size-3); ++i) {
        if (i >= 3 && i <= 5) {
            messageType[i-3] = line[i];
        }
        parity ^= line[i];
    }
    if (checksum != parity) {
        return false;
    }
    messageType[3] = 0;
    if(strcmp(messageType,"GGA") == 0) {
        return processGGALine(line, data);
    }
    if (strcmp(messageType, "RMC") == 0) {
        
    }
    return false;
}

bool GPS::processGGALine(char* GGALine, gps_data_t* data) {
    GGALine += 7;
    data->hour = charIntToInt(*GGALine++)*10 + charIntToInt(*GGALine++);
    data->minute = charIntToInt(*GGALine++)*10 + charIntToInt(*GGALine++);
    data->seconds = uint8_t(stringToFloat(GGALine)); GGALine++;
    data->latitude = stringToDegreesIn1000000ths(GGALine); GGALine++;
    data->latitudeChar = *GGALine++; GGALine++;
    data->longitude = stringToDegreesIn1000000ths(GGALine); GGALine++;
    data->longitudeChar = *GGALine++; GGALine++;
    data->fixQuality = charIntToInt(*GGALine++); GGALine++;
    data->fix = (data->fixQuality > 0 && data->fixQuality < 6);
    data->satellites = stringToInt(GGALine); GGALine++;
    data->HDOP = stringToFloat(GGALine); GGALine++;
    data->altitude = stringToFloatIn100ths(GGALine); GGALine++;
    return true;
}


