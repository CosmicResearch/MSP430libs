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
#include "Senscape.h"
#include "GPS.h"

int main() {
    char const* string = "$GPGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,*5B\n";
    Serial* serial = new Serial();
    GPS gps (serial, 9600);
    for (int i = 0; i < strlen(string); ++i) {
        gps.onSerialReceive(string[i]);
    }
    gps_data_t data = gps.getLastData();
    std::cout << "TIME: " << int(data.hour) << ":" << int(data.minute) << ":" << int(data.seconds) << std::endl;
    assert(data.hour == 9 && data.minute == 27 && data.seconds == 25);
    std::cout << "LOCATION: " << data.latitude/1000000.0f << data.latitudeChar << ", " << data.longitude/1000000.0f << data.longitudeChar << std::endl;
    assert(data.latitude == 47285233 && data.latitudeChar == 'N' && data.longitude == 8565265 && data.longitudeChar == 'E');
    std::cout << "ALTITUDE: " << data.altitude/100.0f << std::endl;
    assert(data.altitude == 49960);
    std::cout << "TYPE: " << data.type << std::endl;
    return 1;
}
