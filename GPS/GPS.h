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

#ifndef BONDAR_GPS
#define BONDAR_GPS

#include "Senscape.h"
#include "Serial.h"
#include <string.h>

struct gps_data_t : sensor_data_t {
    float speed, angle, magvariation, HDOP;
    uint8_t hour, minute, seconds;
    uint8_t day, month;
    uint16_t year;
    int32_t latitude, longitude; //Stored in units of 1/1000000 degrees
    char latitudeChar, longitudeChar;
    int32_t altitude; //Stored in units of 1/100 meters
    bool fix;
    uint8_t fixQuality, satellites;
};

typedef enum {
    S_IDLE,
    S_START,
    S_STOP,
    S_READ,
    S_READ_NOW,
    S_WAKE_UP,
} gps_request_t;

struct gps_state_t;

class GPS : SensorClient {

private:

    static gps_data_t *lastData;
    Serial *serial;
    static gps_state_t state;
    uint32_t baudRate;

    static void ((*onStartDone)(error_t));
    static void ((*onStopDone)(error_t));
    static void ((*onReadDone)(sensor_data_t *, error_t));

    static bool processLine(char* line, gps_data_t *data);
    static bool processGGALine(char* GGALine, gps_data_t *data);
    static bool processRMCLine(char* RMCLine, gps_data_t *data);

    static uint8_t charHexToInt(char c);
    static uint8_t charIntToInt(char c);
    static uint16_t stringToInt(char* &c);
    static float_t stringToFloat(char* &c);
    static uint32_t stringToDegreesIn1000000ths(char* &c);
    static uint32_t stringToFloatIn100ths(char* &c);
    
    static void onSignalDoneTask(void* param);

    void sendNMEACommand(char* command);

public:

    GPS(Serial *serial, uint32_t baudRate);

    virtual error_t start(void);
    virtual error_t stop(void);
    virtual error_t read(void);
    virtual error_t readNow(void);
    virtual boolean_t isStarted(void);

    virtual void attachStartDone(void (*)(error_t));
    virtual void attachStopDone(void (*)(error_t));
    virtual void attachReadDone(void (*)(sensor_data_t *data, error_t));

    static void onSerialSendDone(void);
    static void onSerialReceive(uint8_t data);

    static gps_data_t getLastData(void);

};

#endif
