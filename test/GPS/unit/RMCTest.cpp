/*
 Bondar libraries - tests
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
#include <iostream>
#include <cstring>
#include <cassert>
#include <cmath>
#include <limits>
#include "GPS.h"
#include "Serial.h"

int main() {
    char const* string = "$GPRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A,V*2D\n";
    Serial* serial = new Serial();
    GPS gps (serial, 9600);
    for (int i = 0; i < strlen(string); ++i) {
        gps.onSerialReceive(string[i]);
    }
    gps_data_t data = gps.getLastData();
    std::cout << "TIME: " << int(data.hour) << ":" << int(data.minute) << ":" << int(data.seconds) << std::endl;
    assert(data.hour == 8 && data.minute == 35 && data.seconds == 59);
    std::cout << "LOCATION: " << data.latitude/1000000.0f << data.latitudeChar << ", " << data.longitude/1000000.0f << data.longitudeChar << std::endl;
    assert(data.latitude == 47285240 && data.latitudeChar == 'N' && data.longitude == 8565254 && data.longitudeChar == 'E');
    std::cout << "SPEED: " << data.speed << std::endl;
    assert(fabs(data.speed-0.004) < std::numeric_limits<float>::epsilon());
    std::cout << "TYPE: " << data.type << std::endl;
    return 1;
}

