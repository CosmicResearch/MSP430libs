
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
adxl377_calib_t SensADXL377::_dataaux = {0,0,0};
int16_t SensADXL377::_numLectures = 10;
int16_t SensADXL377::_counter = 0;
boolean_t SensADXL377::started =  0;

SensADC* SensADXL377::_adcx = NULL;
SensADC* SensADXL377::_adcy = NULL;
SensADC* SensADXL377::_adcz = NULL;


/* Null, because instance will be initialized on demand. */
SensADXL377* SensADXL377::instance = NULL;

SensADXL377* SensADXL377::getInstance(SensADC* x, SensADC* y, SensADC* z)
{
    if (instance == NULL)
    {
        instance = new SensADXL377(x,y,z);
    }
    return instance;
}



/* Constructors ***************************************************************/
SensADXL377::SensADXL377(SensADC* x, SensADC* y, SensADC* z) {

    /* Create channels */
	SensADXL377::_adcx = x;
	SensADXL377::_adcy = y;
	SensADXL377::_adcz = z;

    /* sensor default data */
	SensADXL377::_data._chanx = 0;
	SensADXL377::_data._chany = 0;
	SensADXL377::_data._chanz = 0;

    /* sensor default data */
    SensADXL377::_dataaux._chanx = 0;
    SensADXL377::_dataaux._chany = 0;
    SensADXL377::_dataaux._chanz = 0;

    /* Num samples to integrate (less noisy) */
    SensADXL377::_numLectures = 20;

    /* sensor default calibration of 0g */
	SensADXL377::_calib._chanx = 2005;
	SensADXL377::_calib._chany = 2007;
	SensADXL377::_calib._chanz = 2009;
	
	started = false;
}

/* Private Methods ************************************************************/

boolean SensADXL377::onSingleDataReadyChannelx(uint16_t data, error_t result) {
	SensADXL377::_data._chanx = (data* 1) - (SensADXL377::_calib._chanx * 1);  //2^12 = 4096. 4000/4096=0.98 Result in 0.1g
	readState.x = true;
	notifyIfNecessary();

	return false;
}

boolean SensADXL377::onSingleDataReadyChannely(uint16_t data, error_t result) {
	SensADXL377::_data._chany = (data * 1) - (SensADXL377::_calib._chany * 1);  //2^12 = 4096. 4000/4096=0.98. Result in 0.1g
	readState.y = true;
	notifyIfNecessary();

	return false;
}

boolean  SensADXL377::onSingleDataReadyChannelz(uint16_t data, error_t result) {
	SensADXL377::_data._chanz = (data * 1) - (SensADXL377::_calib._chanz * 1);  //2^12 = 4096. 4000/4096=0.98. Result in 0.1g
	readState.z = true;
	notifyIfNecessary();

	return false;
}


error_t SensADXL377::privateReadNow(){
    if (!started) {
        return ERROR;
    }
    error_t ret;

            // Start new conversion channels
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

void SensADXL377::notifyIfNecessary() {
	if (readState.x && readState.y && readState.z && _onReadDone) {


        SensADXL377::_dataaux._chanx = SensADXL377::_dataaux._chanx + SensADXL377::_data._chanx;
        SensADXL377::_dataaux._chany = SensADXL377::_dataaux._chany + SensADXL377::_data._chany;
        SensADXL377::_dataaux._chanz = SensADXL377::_dataaux._chanz + SensADXL377::_data._chanz;
        SensADXL377::_counter = SensADXL377::_counter + 1;

        if (SensADXL377::_counter >= SensADXL377::_numLectures){
            adxl377_data_t* ret = new adxl377_data_t;
            SensADXL377::_counter = 0;
            SensADXL377::_data._chanx = (int16_t) SensADXL377::_dataaux._chanx/SensADXL377::_numLectures;
            SensADXL377::_data._chany = (int16_t) SensADXL377::_dataaux._chany/SensADXL377::_numLectures;
            SensADXL377::_data._chanz = (int16_t) SensADXL377::_dataaux._chanz/SensADXL377::_numLectures;
            SensADXL377::_dataaux._chanx = 0;
            SensADXL377::_dataaux._chany = 0;
            SensADXL377::_dataaux._chanz = 0;

            *ret = SensADXL377::_data;
            _onReadDone(ret, SUCCESS);
        }
        else {
            wait(2); //Adafruit ADXL377 board cannot be reeded higher han 500MHz due to board capacitors
            privateReadNow();
        }
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

            // Start new conversion channels
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


