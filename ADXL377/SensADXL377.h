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

struct adxl377_data_t : sensor_data_base_t{
	int16_t _chanx;
	int16_t _chany;
	int16_t _chanz;
};

struct adxl377_calib_t {
	int16_t _chanx;
	int16_t _chany;
	int16_t _chanz;
};

struct read_state_t {
	boolean_t x, y, z;
};


class SensADXL377 : SensorClient{
	private:
		static SensADC* _adcx;
		static SensADC* _adcy;
		static SensADC* _adcz;
		static adxl377_data_t _data;
		static adxl377_calib_t _calib;
		static read_state_t readState;
		boolean_t started;

		static void ((*_onStartDone)(error_t));
		static void ((*_onStopDone)(error_t));
		static void ((*_onReadDone)(sensor_data_t *, error_t));

		static void notifyIfNecessary(void);

		static boolean onSingleDataReadyChannelx(uint16_t data, error_t result);
		static boolean onSingleDataReadyChannely(uint16_t data, error_t result);
		static boolean onSingleDataReadyChannelz(uint16_t data, error_t result);

	public:
		SensADXL377(SensADC* x, SensADC* y, SensADC* z);

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
