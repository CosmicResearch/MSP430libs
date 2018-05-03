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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.aaaaa
*/


#ifndef SENSCAPE_ADXL377_H_
#define SENSCAPE_ADXL377_H_
#include "sensor_types.h"
#include "Senscape.h"
#include "SensADC.h"

/* Chip ID Definition */
//const uint8_t ADXL377_CHIP_ID = 0x59;

struct adxl377_calib_t {
	int16_t x;
	int16_t y;
	int16_t z;
};

struct read_state_t {
	boolean_t x, y, z;
};


class SensADXL377 : SensorClient{
	private:

        SensADXL377(SensADC* x, SensADC* y, SensADC* z);
        SensADXL377(SensADXL377& adxl ) {}
        static SensADXL377* instance;
		static SensADC* _adcx;
		static SensADC* _adcy;
		static SensADC* _adcz;
		static accel_data_t _data;
		static adxl377_calib_t _calib;
		static adxl377_calib_t _dataaux;
		static int16_t _numLectures;
		static int16_t _counter;
		static read_state_t readState;
		static boolean_t started;

		static void ((*_onStartDone)(error_t));
		static void ((*_onStopDone)(error_t));
		static void ((*_onReadDone)(sensor_data_t *, error_t));

		static error_t privateReadNow(void);
		static void notifyIfNecessary(void);

		static boolean onSingleDataReadyChannelx(uint16_t data, error_t result);
		static boolean onSingleDataReadyChannely(uint16_t data, error_t result);
		static boolean onSingleDataReadyChannelz(uint16_t data, error_t result);

	public:
		static SensADXL377* getInstance(SensADC* x, SensADC* y, SensADC* z);

		virtual error_t start(void);
		error_t read(void);
		error_t stop(void);
		error_t readNow(void);
		boolean_t isStarted(void);

		/* callbacks */
		virtual void attachStartDone(void (*)(error_t));
		virtual void attachStopDone(void (*)(error_t));
		virtual void attachReadDone(void (*)(sensor_data_t *, error_t));

};

#endif //
