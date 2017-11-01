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

#include <Senscape.h>
#include <Serial.h>

struct gps_data_t : sensor_data_t {
    float speed, angle, magvariation, HDOP;    
    uint8_t hour, minute, seconds;
    int32_t latitude, longitude; //Stored in units of 1/100000 degrees
    bool fix;
    uint8_t fixQuality, satellites;
};

class GPS : SensorClient {

private:

    bool paused;
    gps_data_t lastData;
    uint8_t parseResponse(char* response);
    Serial *serial;

public:

    GPS(Serial *serial, Resource *resource);

    virtual error_t start(void);
    virtual error_t stop(void);
    virtual error_t read(void);
    virtual error_t readNow(void);
    virtual boolean_t isStarted(void);

    virtual void attachStartDone(void (*)(error_t));
    virtual void attachStopDone(void (*)(error_t));
    virtual void attachReadDone(void (*)(sensor_data_t *data, error_t));

    gps_data_t getLastData();

}

#endif