
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

#include "SensADXL377.h"


void ((*SensADXL377::_onStartDone)(error_t)) = NULL;
void ((*SensADXL377::_onStopDone)(error_t)) = NULL;
void ((*SensADXL377::_onReadDone)(sensor_data_t *, error_t)) = NULL;
read_state_t SensADXL377::readState = {false, false, false};
adxl377_data_t SensADXL377::_data = adxl377_data_t();
adxl377_calib_t SensADXL377::_calib = {0,0,0};
SensADC* SensADXL377::_adcx = NULL; /* SensADC(
		ADC_CHANNEL_1,
		REFERENCE_AVcc_AVss,
		REFVOLT_LEVEL_NONE,
		SHT_SOURCE_ACLK,
		SHT_CLOCK_DIV_1,
		SAMPLE_HOLD_4_CYCLES,
		SAMPCON_SOURCE_SMCLK,
		SAMPCON_CLOCK_DIV_1);*/
SensADC* SensADXL377::_adcy = NULL; /*SensADC(
		ADC_CHANNEL_2,
		REFERENCE_AVcc_AVss,
		REFVOLT_LEVEL_NONE,
		SHT_SOURCE_ACLK,
		SHT_CLOCK_DIV_1,
		SAMPLE_HOLD_4_CYCLES,
		SAMPCON_SOURCE_SMCLK,
		SAMPCON_CLOCK_DIV_1);*/
SensADC* SensADXL377::_adcz = NULL;/* SensADC(
		ADC_CHANNEL_3,
		REFERENCE_AVcc_AVss,
		REFVOLT_LEVEL_NONE,
		SHT_SOURCE_ACLK,
		SHT_CLOCK_DIV_1,
		SAMPLE_HOLD_4_CYCLES,
		SAMPCON_SOURCE_SMCLK,
		SAMPCON_CLOCK_DIV_1);*/

/* Constructors ***************************************************************/
SensADXL377::SensADXL377(/*uint16_t inchx,
		uint16_t inchy,
		uint16_t inchz,
		uint16_t sref,
		uint16_t ref2_5v,
		uint16_t ssel,
		uint16_t div,
		uint16_t sht,
		uint16_t sampcon_ssel,
		uint16_t sampcon_id*/
		SensADC* x, SensADC* y, SensADC* z) {

	SensADXL377::_adcx = x;
	SensADXL377::_adcy = y;
	SensADXL377::_adcz = z;

	/* Crete an ADC Sens for every axis */
	/*SensADXL377::_adcx = new SensADC(
			inchx,
			sref,
			ref2_5v,
			ssel,
			div,
			sht,
			sampcon_ssel,
			sampcon_id);

	SensADXL377::_adcy = new SensADC(
				inchy,
				sref,
				ref2_5v,
				ssel,
				div,
				sht,
				sampcon_ssel,
				sampcon_id);

	SensADXL377::_adcz = new SensADC(
				inchz,
				sref,
				ref2_5v,
				ssel,
				div,
				sht,
				sampcon_ssel,
				sampcon_id);*/

    /* sensor default data */
	SensADXL377::_data._chanx = 0;
	SensADXL377::_data._chany = 0;
	SensADXL377::_data._chanz = 0;

    /* sensor default calibration of 0g */
	SensADXL377::_calib._chanx = 2040;
	SensADXL377::_calib._chany = 2040;
	SensADXL377::_calib._chanz = 2040;
	
	started = false;
}

/* Private Methods ************************************************************/

boolean SensADXL377::onSingleDataReadyChannelx(uint16_t data, error_t result) {
	SensADXL377::_data._chanx =  (data* 4000 / 4096) - (SensADXL377::_calib._chanx * 4000 / 4096);  //2^12 = 4096. Result in 0.1g
	readState.x = true;
	notifyIfNecessary();

	return false;
}

boolean SensADXL377::onSingleDataReadyChannely(uint16_t data, error_t result) {
	SensADXL377::_data._chany =  (data * 4000 / 4096) - (SensADXL377::_calib._chany * 4000 / 4096);  //2^12 = 4096. Result in 0.1g
	readState.y = true;
	notifyIfNecessary();

	return false;
}

boolean  SensADXL377::onSingleDataReadyChannelz(uint16_t data, error_t result) {
	SensADXL377::_data._chanz = (data * 4000 / 4096) - (SensADXL377::_calib._chanz * 4000 / 4096);  //2^12 = 4096. Result in 0.1g
	readState.z = true;
	notifyIfNecessary();

	return false;
}

void SensADXL377::notifyIfNecessary() {
	if (readState.x && readState.y && readState.z && _onReadDone) {
		adxl377_data_t* ret = new adxl377_data_t;
		*ret = SensADXL377::_data;
		_onReadDone(ret, SUCCESS);
	}
}

/* Public Methods *************************************************************/
error_t SensADXL377::start() {

	if (!started) {
		SensADXL377::_adcx->attachCallback(onSingleDataReadyChannelx);
		SensADXL377::_adcy->attachCallback(onSingleDataReadyChannely);
		SensADXL377::_adcz->attachCallback(onSingleDataReadyChannelz);
		started = true;
		if (_onStartDone) {
			_onStartDone(SUCCESS);
		}
		return SUCCESS;
	}
	else {
		if (_onStartDone) {
			_onStartDone(ERROR);
		}
		return ERROR;
	}
}

error_t SensADXL377::stop() {

	if (started) {
		started = false;
		if (_onStopDone) {
			_onStopDone(SUCCESS);
		}
		return SUCCESS;
	}
	else {
		if (_onStopDone) {
			_onStopDone(ERROR);
		}
		return ERROR;
	}
}

error_t SensADXL377::read(){
	if (!started) {
		return ERROR;
	}
	error_t ret;
		// Start new conversion channel[0..2].
	if ((ret = SensADXL377::_adcx->read()) != SUCCESS) {
		return ret;
	}
	if (( ret = SensADXL377::_adcy->read()) != SUCCESS) {
		return ret;
	}
	if (( ret = SensADXL377::_adcz->read()) != SUCCESS) {
		return ret;
	}
	SensADXL377::readState = {false, false, false};

	return SUCCESS;
}

error_t SensADXL377::readNow() {
	return read();
}

/* Callbacks ******************************************************************/
void SensADXL377::attachStartDone(void (*function)(error_t)) {
	_onStartDone = function;
}
void SensADXL377::attachStopDone(void (*function)(error_t)) {
	_onStopDone = function;
}
void SensADXL377::attachReadDone(void (*function)(sensor_data_t *, error_t)) {
	_onReadDone = function;
}
boolean_t SensADXL377::isStarted() {
	return this->started;
}


