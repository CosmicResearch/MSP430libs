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

void GPS::attachStartDone(void(*function)(error_t)) {
    _onStartDone = function;    
}

void GPS::attachReadDone(void(*function)(sensor_data_t *, error_t)) {
    _onReadDone = function;
}

void GPS::attachStopDone(void(*function)(error_t)) {
    _onStopDone = function;    
}

gps_data_t GPS::getLastData() {
    return this->lastData;
}