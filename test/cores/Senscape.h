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

#ifndef SENSCAPE_H_
#define SENSCAPE_H_

#include <ctype.h>
#include <stdint.h>
#include <string.h>

#define ERROR 1
#define SUCCESS 0

typedef bool boolean_t;
typedef unsigned char byte_t;
typedef int error_t;
typedef float float_t;

error_t postTask(void (*function)(void*), void *param);

#include "SensorClient.h"
#include "Serial.h"

#endif
