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

#define MAXLENGTH 120

char lineBuffer1[MAXLENGTH];
char lineBuffer2[MAXLENGTH];

gps_state_t GPS::state = {S_IDLE, false, false, lineBuffer1, lineBuffer2, 0};
gps_data_t GPS::lastData;
void ((*GPS::onReadDone)(sensor_data_t*, error_t)) = NULL;
void ((*GPS::onStartDone)(error_t)) = NULL;
void ((*GPS::onStopDone)(error_t)) = NULL;


/*
PUBLIC METHODS
*/

GPS::GPS(Serial* serial, const uint32_t baudRate) {
    this->serial = serial;
    this->baudRate = baudRate;
}

error_t GPS::start() {
    if (!GPS::state.isStarted) {
        serial->attachReceive(onSerialReceive);
        serial->attachSendDone(onSerialSendDone);
        serial->begin(baudRate);
        GPS::state.isStarted = true;
        if (onStartDone) {
            onStartDone(SUCCESS);
        }
        return SUCCESS;
    }
    if (onStartDone) {
        onStartDone(SUCCESS);
    }
    return ERROR;
}

error_t GPS::stop() {
    if (GPS::state.isStarted) {
        serial->detachSendDone();
        serial->detachReceive();
        serial->end();
        GPS::state.isStarted = false;
        if (onStopDone) {
            onStopDone(SUCCESS);
        }
        return SUCCESS;
    }
    if (onStopDone) {
        onStopDone(ERROR);
    }
    return ERROR;
}

error_t GPS::read() {
    return SUCCESS;
}

error_t GPS::readNow() {
    return SUCCESS;
}

boolean_t GPS::isStarted() {
    return GPS::state.isStarted;
}

/*
CALLBACKS
*/

void GPS::onSerialSendDone() {}

void GPS::onSerialReceive(uint8_t data) {
    if (data == '$') {
        GPS::state.lineIndex = 0;
    }
    if ((data == '\n' || data == '\r') && GPS::state.lineIndex > 0 ) {
        GPS::state.currentLine[GPS::state.lineIndex] = 0;
        if (GPS::state.currentLine == lineBuffer1) {
            GPS::state.currentLine = lineBuffer2;
            GPS::state.lastLine = lineBuffer1;
        }
        else {
            GPS::state.currentLine = lineBuffer1;
            GPS::state.lastLine = lineBuffer2;
        }
        GPS::state.lineIndex = 0;
        postTask(onSignalDoneTask, SUCCESS);
    }
    GPS::state.currentLine[GPS::state.lineIndex++] = data;
    if (GPS::state.lineIndex >= MAXLENGTH) {
        GPS::state.lineIndex = MAXLENGTH - 1;
    }
}

/*
SETTERS FOR THE CALLBACKS
*/

void GPS::attachStartDone(void(*function)(error_t)) {
    GPS::onStartDone = function;
}

void GPS::attachReadDone(void(*function)(sensor_data_t *, error_t)) {
    GPS::onReadDone = function;
}

void GPS::attachStopDone(void(*function)(error_t)) {
    GPS::onStopDone = function;
}

gps_data_t GPS::getLastData() {
    return GPS::lastData;
}

/*
PRIVATE METHODS
*/

void GPS::sendNMEACommand(char* command) {
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
    uint16_t len = strlen(c);
    uint16_t i = 0;
    while (i < len && c[i] != ',') {
        ++i;
    }
    char buff[20] = {0};
    strncpy(buff, c, i);
    c += i;
    return atof(buff);
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
            ret = ret + multiplier * (*c - '0');
            multiplier = multiplier / 10;
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
    unsigned long size = strlen(line);
    if(line[0] != '$') { //NMEA 0183 messages start with '$'
        return false;
    }
    if(line[size-3] != '*') {
        return false;
    }
    /*
     Let's process the checksum
     Checksum = XOR of all characters between '$' and '*'
     */
    uint8_t checksum = charHexToInt(line[size-2])*16 + charHexToInt(line[size-1]);
    uint8_t parity = 0;
    char messageType[4] = {0};
    strncpy(messageType, &line[3], 3);
    for (int i = 1; i < (size-3); ++i) {
        parity ^= line[i];
    }
    if (checksum != parity) {
        return false;
    }
    messageType[3] = 0;
    if(strcmp(messageType,"GGA") == 0) {
        return processGGALine(line, data);
    }
    else if (strcmp(messageType, "RMC") == 0) {
        return processRMCLine(line, data);
    }
    else if (strcmp(messageType, "GLL") == 0) {
        return processGLLLine(line, data);
    }
    return true;
}

bool GPS::processGGALine(char* GGALine, gps_data_t* data) {
    char* line = GGALine;
    GGALine += 7;
    data->type = GGA;
    if (*GGALine != ',') {
        data->hour = charIntToInt(*GGALine++)*10 + charIntToInt(*GGALine++);
        data->minute = charIntToInt(*GGALine++)*10 + charIntToInt(*GGALine++);
        data->seconds = uint8_t(stringToFloat(GGALine)); GGALine++;
    }
    else {
        GGALine++;
        data->hour = 0;
        data->minute = 0;
        data->seconds = 0;
    }
    if (*GGALine != ',') {
        data->latitude = stringToDegreesIn1000000ths(GGALine); GGALine++;
    }
    else {
        GGALine++;
        data->latitude = 0;
    }
    if (*GGALine != ',') {
        data->latitudeChar = *GGALine++; GGALine++;
    }
    else {
        GGALine++;
        data->latitudeChar = 'N';
    }
    if (*GGALine != ',') {
        data->longitude = stringToDegreesIn1000000ths(GGALine); GGALine++;
    }
    else {
        GGALine++;
        data->longitude = 0;
    }
    if (*GGALine != ',') {
        data->longitudeChar = *GGALine++; GGALine++;
    }
    else {
        GGALine++;
        data->longitudeChar = 'E';
    }
    if (*GGALine != ',') {
        data->fixQuality = charIntToInt(*GGALine++); GGALine++;
        data->fix = (data->fixQuality > 0 && data->fixQuality < 6);
    }
    else {
        GGALine++;
        data->fix = false;
    }
    if (*GGALine != ',') {
        data->satellites = stringToInt(GGALine); GGALine++;
    }
    else {
        GGALine++;
        data->satellites = 0;
    }
    if (*GGALine != ',') {
        data->HDOP = stringToFloat(GGALine); GGALine++;
    }
    else {
        GGALine++;
        data->HDOP = 0;
    }
    if (*GGALine != ',') {
        data->altitude = stringToFloatIn100ths(GGALine); GGALine++;
    }
    else {
        GGALine++;
        data->altitude = -1;
    }
    return true;

}

bool GPS::processGLLLine(char* GLLLine, gps_data_t* data) {
    char* line = GLLLine;
    GLLLine+=7;
    data->type = GLL;
    if (*GLLLine != ',') {
        data->latitude = stringToDegreesIn1000000ths(GLLLine); GLLLine++;
    }
    else {
        GLLLine++;
        data->latitude = 0;
    }
    if (*GLLLine != ',') {
        data->latitudeChar = *GLLLine++; GLLLine++;
    }
    else {
        GLLLine++;
        data->latitudeChar = 'N';
    }
    if (*GLLLine != ',') {
        data->longitude = stringToDegreesIn1000000ths(GLLLine); GLLLine++;
    }
    else {
        GLLLine++;
        data->longitude = 0;
    }
    if (*GLLLine != ',') {
        data->longitudeChar = *GLLLine++; GLLLine++;
    }
    else {
        GLLLine++;
        data->longitudeChar = 'E';
    }
    if (*GLLLine != ',') {
            data->hour = charIntToInt(*GLLLine++)*10 + charIntToInt(*GLLLine++);
            data->minute = charIntToInt(*GLLLine++)*10 + charIntToInt(*GLLLine++);
            data->seconds = uint8_t(stringToFloat(GLLLine)); GLLLine++;
    }
    else {
        GLLLine++;
        data->hour = 0;
        data->minute = 0;
        data->seconds = 0;
    }
    data->fix = (*GLLLine=='A'); GLLLine++; GLLLine++;
    return true;
}

bool GPS::processRMCLine(char* RMCLine, gps_data_t* data) {
    char* line = RMCLine;
    data->type = RMC;
    RMCLine += 7;
    if (*RMCLine != ',') {
        data->hour = charIntToInt(*RMCLine++)*10 + charIntToInt(*RMCLine++);
        data->minute = charIntToInt(*RMCLine++)*10 + charIntToInt(*RMCLine++);
        data->seconds = uint8_t(stringToFloat(RMCLine)); RMCLine++;
    }
    else {
        RMCLine++;
        data->hour = 0;
        data->minute = 0;
        data->seconds = 0;
    }
    data->fix = (*RMCLine=='A'); RMCLine++; RMCLine++;
    if (*RMCLine != ',') {
        data->latitude = stringToDegreesIn1000000ths(RMCLine); RMCLine++;
    }
    else {
        RMCLine++;
        data->latitude = 0;
    }
    if (*RMCLine != ',') {
        data->latitudeChar = *RMCLine++; RMCLine++;
    }
    else {
        RMCLine++;
        data->latitudeChar = 'N';
    }
    if (*RMCLine != ',') {
        data->longitude = stringToDegreesIn1000000ths(RMCLine); RMCLine++;
    }
    else {
        RMCLine++;
        data->longitude = 0;
    }
    if (*RMCLine != ',') {
        data->longitudeChar = *RMCLine++; RMCLine++;
    }
    else {
        RMCLine++;
        data->longitudeChar = 'E';
    }
    if (*RMCLine != ',') {
        data->speed = stringToFloat(RMCLine); RMCLine++;
    }
    else {
        RMCLine++;
        data->speed = 0.0f;
    }
    if (*RMCLine != ',') {
        data->angle = stringToFloat(RMCLine); RMCLine++;
    }
    else {
        RMCLine++;
        data->angle = 0.0f;
    }
    if (*RMCLine != ',') {
        data->day = charIntToInt(*RMCLine++)*10 + charIntToInt(*RMCLine++);
        data->month = charIntToInt(*RMCLine++)*10 + charIntToInt(*RMCLine++);
        data->year = charIntToInt(*RMCLine++)*10 + charIntToInt(*RMCLine++); RMCLine++;
    }
    else {
        RMCLine++;
        data->day = 0;
        data->month = 0;
        data->year = 0;
    }
    if (*RMCLine != ',') {
        data->magvariation = stringToFloat(RMCLine); RMCLine++;
    }
    else {
        data->magvariation = 0.0f;
    }
    data->altitude = -1;
    return true;
}

void GPS::onSignalDoneTask(void* param) {
    gps_data_t gps_data;
    bool correct = processLine((char*)GPS::state.lastLine, &gps_data);
    if (!correct) {
        if (onReadDone != NULL)
            onReadDone(NULL, ERROR);
        return;
    }
    else  {
        GPS::lastData = gps_data;
        if (onReadDone != NULL) {
            onReadDone(&gps_data, SUCCESS);
        }
    }
    return;
}

