/*
 * Copyright (c) 2014-2017, Irideon SL. All rights reserved.
 *
 * End-User License Agreement ("Agreement")
 *
 * Please read this End-User License Agreement ("Agreement") carefully before you start
 * using the Application:
 *
 * (a) LICENSE: IRIDEON grants you a revocable, non-exclusive, non-transferable, limited
 * license to download, install and use the Application solely for your personal, non-
 * commercial purposes strictly in accordance with the terms of this Agreement.
 *
 * (b) RESTRICTIONS: You agree not to, and you will not permit others to license, sell,
 * rent, lease, assign, distribute, transmit, host, outsource, disclose or otherwise
 * commercially exploit the Application or make the Application available to any third
 * party.
 *
 * (c) MODIFICATIONS TO APPLICATION: IRIDEON reserves the right to modify, suspend or
 * discontinue, temporarily or permanently, the Application or any service to which it
 * connects, with or without notice and without liability to you.
 *
 * (d) TERM AND TERMINATION: This Agreement shall remain in effect until terminated by
 * you or IRIDEON. IRIDEON may, in its sole discretion, at any time and for any or no
 * reason, suspend or terminate this Agreement with or without prior notice. This
 * Agreement will terminate immediately, without prior notice from IRIDEON, in the event
 * that you fail to comply with any provision of this Agreement. You may also terminate
 * this Agreement by deleting the Application and all copies thereof from your mobile
 * device or from your desktop. Upon termination of this Agreement, you shall cease all
 * use of the Application and delete all copies of the Application from your mobile
 * device or from your desktop.
 *
 * (e) SEVERABILITY: If any provision of this Agreement is held to be unenforceable or
 * invalid, such provision will be changed and interpreted to accomplish the objectives
 * of such provision to the greatest extent possible under applicable law and the
 * remaining provisions will continue in full force and effect.
 *
 * (d) AMENDMENTS TO THIS AGREEMENT: IRIDEON reserves the right, at its sole discretion,
 * to modify or replace this Agreement at any time. If a revision is material we will
 * provide at least 30 (changes this) days' notice prior to any new terms taking effect.
 * What constitutes a material change will be determined at our sole discretion.
 *
 * If you have any questions about this Agreement, please contact us at info@irideon.eu.
 *
 * @author Pancras Villalonga <p.villalonga@irideon.eu>
 * @date   August 16, 2017
 */

#include "Senscape.h"
#include "SensTimer.h"
#include "SensBMP280.h"

typedef enum {
	S_IDLE,
	S_START,
	S_STOP,
	S_READ,
	S_READ_NOW
} bmp280_req_t;

struct bmp280_calib_t {
	uint16_t dig_T1;
	int16_t  dig_T2;
	int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
};

struct bmp280_state_t {
	bmp280_req_t req;
	boolean_t is_started;
	boolean_t is_ready;
	uint8_t resol;
    uint8_t standby_time;
    uint8_t filter_coef;
	float_t sea_level_press;
	bmp280_calib_t calib;
};

uint8_t bmp280_buffer[24] =  { 0 };

bmp280_state_t SensBMP280::_state = { S_IDLE, false, false, 0, 0, 0, 0.0, { 0 } };
bmp280_data_t SensBMP280::_data;
SensTimer *SensBMP280::_spiTimer = NULL;
Resource *SensBMP280::_spiResource = NULL;
SPI *SensBMP280::_spiObj = NULL;

void ((*SensBMP280::_onStartDone)(error_t));
void ((*SensBMP280::_onStopDone)(error_t));
void ((*SensBMP280::_onReadDone)(sensor_data_t *, error_t));

/* Constructors ***************************************************************/

SensBMP280::SensBMP280(SPI *spi, Resource *resource, SensTimer *timer, const uint8_t resolution,
		const uint8_t standby_time, const uint8_t filter_coeficient, const float_t sea_level_pressure) {
	/* spi settings */
    _spiTimer = timer;
	_spiResource = resource;
	_spiObj = spi;

	/* sensor default settings */
	_state.resol = resolution | BMP280_SLEEP_MODE;
    _state.standby_time = standby_time;
    _state.filter_coef = filter_coeficient;
	_state.sea_level_press = sea_level_pressure;

    /* sensor default data */
    _data.u_press = 0;
    _data.u_temp = 0;
}

/* Private Methods ************************************************************/

uint8_t SensBMP280::readRegister(uint8_t reg) {
	uint8_t value;

	/* assert chip select */
	digitalWrite(BMP280_CSN, LOW);

	/* read register */
	_spiObj->transfer(reg | 0x80);
	value = _spiObj->transfer(0xFF);

	/* de-assert chip select */
	digitalWrite(BMP280_CSN, HIGH);
	return value;
}

void SensBMP280::writeRegister(uint8_t reg, uint8_t value) {
	/* assert chip select */
	digitalWrite(BMP280_CSN, LOW);

	/* write register */
	_spiObj->transfer(reg & 0x7F);
	_spiObj->transfer(value);

	/* de-assert chip select */
	digitalWrite(BMP280_CSN, HIGH);
}

void SensBMP280::setPowerMode(uint8_t mode) {
	uint8_t reg = readRegister(BMP280_CTRL_MEAS_REG);
	reg = (reg & ~(BMP280_POWER_MODE_MASK)) | mode;
	writeRegister(BMP280_CTRL_MEAS_REG, reg);
}

void SensBMP280::setSettings(uint8_t resol, uint8_t standby_time, uint8_t filter_coef) {
    /* set resolution and power mode */
    uint8_t reg = readRegister(BMP280_CTRL_MEAS_REG);
	reg = (reg & ~(BMP280_WORKING_MODE_MASK)) | resol;
	writeRegister(BMP280_CTRL_MEAS_REG, reg);

	/* set standby time and filter coeficient */
	reg = readRegister(BMP280_CONFIG_REG);
	reg = (reg & ~(BMP280_STANDBY_AND_FILTER_MASK)) | standby_time | filter_coef;
	writeRegister(BMP280_CONFIG_REG, reg);
}

int32_t SensBMP280::getFineResolutionTemperature() {
    int32_t var1, var2;

#ifdef BMP280_TEST_PRESS_AND_TEMP_FORMULAS
    /* temperature calculation example, see BMP280 datasheet @ page 23, result => 25.08 degC */
    _state.calib.dig_T1 = 27504;
    _state.calib.dig_T2 = 26435;
    _state.calib.dig_T3 = -1000;
    _data.u_temp = 519888;
#endif

    var1 = ((((_data.u_temp >> 3) - ((int32_t)_state.calib.dig_T1 << 1))) *
            ((int32_t)_state.calib.dig_T2)) >> 11;
    var2 = (((((_data.u_temp >> 4) - ((int32_t)_state.calib.dig_T1)) *
            ((_data.u_temp >> 4) - ((int32_t)_state.calib.dig_T1))) >> 12) *
            ((int32_t)_state.calib.dig_T3)) >> 14;
    return (var1 + var2);
}

void SensBMP280::onSpiResourceGranted() {
    /* enable SPI bus */
    _spiObj->begin();
	/* attach SPI tranfer callback */
    _spiObj->attachTransferDone(onSpiTranferDone);

    switch(_state.req) {
    	case S_START:
    		/* check device id */
    		if (readRegister(BMP280_CHIP_ID_REG) == BMP280_CHIP_ID) {
    			/* do soft reset */
    			writeRegister(BMP280_RST_REG, BMP280_RESET);

    			/* wait for NVM data is copied to image registers */
    			wait(BMP280_DELAY);

    			/* start reading calibration registers */
    			digitalWrite(BMP280_CSN, LOW);
    			_spiObj->transfer(BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG | 0x80);
    		    _spiObj->transfer(NULL, bmp280_buffer, 24);
    		}
    		else {
    			/* invalid device id */
    			postTask(onSignalDoneTask, (void*)(uint16_t)ERROR);
    		}
    		break;
    	case S_STOP:
    		/* put sensor in sleep mode */
    		setPowerMode(BMP280_SLEEP_MODE);
    		postTask(onSignalDoneTask, (void*)(uint16_t)SUCCESS);
			break;
    	case S_READ:
    	case S_READ_NOW:
    		if (!_state.is_ready) {
    		    if (_state.req == S_READ) {
                    /* put sensor in normal mode */
                    setPowerMode(BMP280_NORMAL_MODE);
    		    }
    		    else {
    		        /* put sensor in forced mode */
    		        setPowerMode(BMP280_FORCED_MODE);
    		    }
    			/* wait for measure completed (100 msec max) */
    			_spiTimer->startOneShot(BMP280_DELAY);

    			_state.is_ready = true;
    		}
    		else {
    	        /* start reading pressure and temperature registers */
    	        digitalWrite(BMP280_CSN, LOW);
    	        _spiObj->transfer(BMP280_PRESSURE_MSB_REG | 0x80);
    	        _spiObj->transfer(NULL, bmp280_buffer, 6);
    		}
    		break;
        default:
        	/* fatal error */
        	postTask(onSignalDoneTask, (void*)(uint16_t)ERROR);
            break;
    }
}

void SensBMP280::onSpiTranferDone(uint8_t* tx_buf, uint8_t* rx_buf, uint16_t len, error_t result) {
	/* de-assert chip select */
	digitalWrite(BMP280_CSN, HIGH);

	switch(_state.req) {
		case S_START:
			if (result == SUCCESS) {
				/* save temperature calibration coeficients */
				_state.calib.dig_T1 = (rx_buf[1] << 8) | rx_buf[0];
				_state.calib.dig_T2 = (int16_t)((rx_buf[3] << 8) | rx_buf[2]);
				_state.calib.dig_T3 = (int16_t)((rx_buf[5] << 8) | rx_buf[4]);

				/* save presure calibration coeficients */
				_state.calib.dig_P1 = (rx_buf[7] << 8) | rx_buf[6];
				_state.calib.dig_P2 = (int16_t)((rx_buf[9] << 8) | rx_buf[8]);
				_state.calib.dig_P3 = (int16_t)((rx_buf[11] << 8) | rx_buf[10]);
				_state.calib.dig_P4 = (int16_t)((rx_buf[13] << 8) | rx_buf[12]);
				_state.calib.dig_P5 = (int16_t)((rx_buf[15] << 8) | rx_buf[14]);
				_state.calib.dig_P6 = (int16_t)((rx_buf[17] << 8) | rx_buf[16]);
				_state.calib.dig_P7 = (int16_t)((rx_buf[19] << 8) | rx_buf[18]);
				_state.calib.dig_P8 = (int16_t)((rx_buf[21] << 8) | rx_buf[20]);
				_state.calib.dig_P9 = (int16_t)((rx_buf[23] << 8) | rx_buf[22]);

				/* set sensor settings */
				setSettings(_state.resol, _state.standby_time, _state.filter_coef);
			}
			postTask(onSignalDoneTask, (void*)(uint16_t)result);
			break;
		case S_READ:
		case S_READ_NOW:
			if (result == SUCCESS) {
				/* save uncompensated pressure (20-bit) */
				_data.u_press = rx_buf[0];
				_data.u_press <<= 8;
				_data.u_press |= rx_buf[1];
				_data.u_press <<= 8;
				_data.u_press |= rx_buf[2];
				_data.u_press >>= 4;

				/* save uncompensated temperature (20-bit) */
				_data.u_temp = rx_buf[3];
				_data.u_temp <<= 8;
				_data.u_temp |= rx_buf[4];
				_data.u_temp <<= 8;
				_data.u_temp |= rx_buf[5];
				_data.u_temp >>= 4;
			}
			postTask(onSignalDoneTask, (void*)(uint16_t)result);
			break;
		default:
			/* fatal error */
			postTask(onSignalDoneTask, (void*)(uint16_t)ERROR);
			break;
	}
}

void SensBMP280::onDataReady() {
    if (_spiResource->isOwner()) {
        /* start reading pressure and temperature registers */
        digitalWrite(BMP280_CSN, LOW);
        _spiObj->transfer(BMP280_PRESSURE_MSB_REG | 0x80);
        _spiObj->transfer(NULL, bmp280_buffer, 6);
    }
    else {
        /* fatal error */
        postTask(onSignalDoneTask, (void*)(uint16_t)ERROR);
    }
}

void SensBMP280::onSignalDoneTask(void *param) {
	bmp280_req_t req = _state.req;
	error_t result = (error_t)(uint16_t)param;

	/* disable SPI bus */
    _spiObj->end();
    /* detach SPI callbacks */
    _spiObj->detachTransferDone();
    /* release SPI resource */
    _spiResource->release();

    _state.req = S_IDLE;

    switch(req) {
    	case S_START:
    		if (_onStartDone) {
                _state.is_started = true;
#ifdef BMP280_DEBUG
                Debug.println();
                Debug.print("dig_T1: ").println(_state.calib.dig_T1);
                Debug.print("dig_T2: ").println(_state.calib.dig_T2);
                Debug.print("dig_T3: ").println(_state.calib.dig_T3);
                Debug.print("dig_P1: ").println(_state.calib.dig_P1);
                Debug.print("dig_P2: ").println(_state.calib.dig_P2);
                Debug.print("dig_P3: ").println(_state.calib.dig_P3);
                Debug.print("dig_P4: ").println(_state.calib.dig_P4);
                Debug.print("dig_P5: ").println(_state.calib.dig_P5);
                Debug.print("dig_P6: ").println(_state.calib.dig_P6);
                Debug.print("dig_P7: ").println(_state.calib.dig_P7);
                Debug.print("dig_P8: ").println(_state.calib.dig_P8);
                Debug.print("dig_P9: ").println(_state.calib.dig_P9);
                Debug.print("dig_P1: ").println(_state.calib.dig_P1);
#endif
                /* notify start event */
                _onStartDone(result);
    		}
    		break;
    	case S_STOP:
    		if (_onStopDone) {
				_state.is_started = false;
				/* detach timer callback */
				_spiTimer->attachCallback(NULL);
                /* detach SPI resource callback */
                _spiResource->detachResourceGranted();
                /* configure chip select as input */
                pinMode(BMP280_CSN, INPUT);
                /* notify stop event */
                _onStopDone(result);
    		}
    		break;
    	case S_READ:
    	case S_READ_NOW:
    		if (_onReadDone) {
                if (result == SUCCESS) {
#ifdef BMP180_DEBUG
                    Debug.println();
                    Debug.print("Uncompensated Temperature: ").println(_data.u_temp);
                    Debug.print("Uncompensated Pressure: ").println(_data.u_press);
#endif
                }
                /* compute fine resolution temperature */
                _data.t_fine = getFineResolutionTemperature();
                /* notify data event */
				_onReadDone((sensor_data_t*)&_data, (error_t)(uint16_t)result);
    		}
    		break;
        default:
            break;
    }
}

/* Public Methods *************************************************************/

error_t SensBMP280::start() {
    if (!_state.is_started) {
        if (_state.req == S_IDLE) {
            _state.req = S_START;

            /* configure chip select as high output */
            pinMode(BMP280_CSN, OUTPUT);
            digitalWrite(BMP280_CSN, HIGH);

            /* attach SPI resource and timer callback */
            _spiResource->attachResourceGranted(onSpiResourceGranted);
            _spiTimer->attachCallback(onDataReady);

            /* request SPI resource */
            return _spiResource->request();
        }
        return EBUSY;
    }
    return ERROR;
}

error_t SensBMP280::stop() {
	if (_state.req == S_IDLE) {
		_state.req = S_STOP;

		/* request SPI resource */
		return _spiResource->request();
	}
	return EBUSY;
}

error_t SensBMP280::read() {
    if (_state.is_started) {
        if (_state.req == S_IDLE) {
            _state.req = S_READ;
            /* request SPI resource */
            return _spiResource->request();
        }
        return EBUSY;
    }
    return ERROR;
}

error_t SensBMP280::readNow() {
    if (_state.is_started) {
        if (_state.req == S_IDLE) {
            _state.req = S_READ_NOW;
            _state.is_ready = false;
            /* inmediate request SPI resource */
            return _spiResource->immediateRequest();
        }
        return EBUSY;
    }
    return ERROR;
}

boolean_t SensBMP280::isStarted(void) {
    return _state.is_started;
}

/* Setters & Getters **********************************************************/

error_t SensBMP280::setResolution(uint16_t resolution) {
    if (!_state.is_started) {
        if (resolution == BMP280_ULTRA_LOW_POWER || resolution == BMP280_LOW_POWER ||
                resolution == BMP280_STANDARD_RESOLUTION || resolution == BMP280_HIGH_RESOLUTION ||
                resolution == BMP280_ULTRA_HIGH_RESOLUTION) {
            _state.resol = resolution;
            return SUCCESS;
        }
        return EINVAL;
    }
    return ERROR;
}

error_t SensBMP280::setStandbyTime(uint8_t time) {
    if (!_state.is_started) {
        if (time == BMP280_STANDBY_TIME_1_MS || time == BMP280_STANDBY_TIME_63_MS ||
                time == BMP280_STANDBY_TIME_125_MS || time == BMP280_STANDBY_TIME_250_MS ||
                time == BMP280_STANDBY_TIME_500_MS || time == BMP280_STANDBY_TIME_1000_MS ||
                time == BMP280_STANDBY_TIME_2000_MS || time == BMP280_STANDBY_TIME_4000_MS) {
            _state.standby_time = time;
            return SUCCESS;
        }
        return EINVAL;
    }
    return ERROR;
}

error_t SensBMP280::setFilterCoeficient(uint8_t coeficient) {
    if (!_state.is_started) {
        if (coeficient == BMP280_FILTER_COEF_OFF || coeficient == BMP280_FILTER_COEF_2 ||
                coeficient == BMP280_FILTER_COEF_4 || coeficient == BMP280_FILTER_COEF_8 ||
                coeficient == BMP280_FILTER_COEF_16) {
            _state.filter_coef = coeficient;
            return SUCCESS;
        }
        return EINVAL;
    }
    return ERROR;
}

error_t SensBMP280::setSeaLevelPressure(float_t pressure) {
    if (pressure >= 0.0) {
        _state.sea_level_press = pressure;
        return SUCCESS;
    }
    return ERROR;
}

float_t SensBMP280::getPressure(sensor_data_t *data) {
    int64_t var1, var2, p;

#ifdef BMP280_TEST_PRESS_AND_TEMP_FORMULAS
    /* pressure calculation example, see BMP280 datasheet @ page 23, result => 100653,27 Pa */
    _state.calib.dig_P1 = 36477;
    _state.calib.dig_P2 = -10685;
    _state.calib.dig_P3 = 3024;
    _state.calib.dig_P4 = 2855;
    _state.calib.dig_P5 = 140;
    _state.calib.dig_P6 = -7;
    _state.calib.dig_P7 = 15500;
    _state.calib.dig_P8 = -14600;
    _state.calib.dig_P9 = 6000;
    ((bmp280_data_t*)data)->u_press = 415148;
#endif

    /* compensation pressure formula (see BME208 datasheet @ appendix A) */
    var1 = ((int64_t)_data.t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_state.calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)_state.calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)_state.calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)_state.calib.dig_P3) >> 8) +
            ((var1 * (int64_t)_state.calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_state.calib.dig_P1) >> 33;

    if (var1 == 0) {
        /* avoid exception caused by division by zero */
        return 0;
    }
    p = 1048576 - ((bmp280_data_t*)data)->u_press;
    p = (((p << 31) - var2) * 3125)/var1;
    var1 = (((int64_t)_state.calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)_state.calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)_state.calib.dig_P7) << 4);

    /* pressure in Pa */
    return (float_t)p/256;
}

float_t SensBMP280::getAltitude(float_t pressure) {
    /* altitude in meters */
    return (44330 * (1.0 - powf(pressure/_state.sea_level_press, 0.1903)));
}

float_t SensBMP280::getTemperature(sensor_data_t *data) {
    /* compensation temperature formula (see BME208 datasheet @ appendix A) */
    int32_t t = (_data.t_fine * 5 + 128) >> 8;

    /* temperature in degC */
    return (float_t)t/100;
}

/* Callbacks ******************************************************************/

void SensBMP280::attachStartDone(void (*function)(error_t)) {
	_onStartDone = function;
}

void SensBMP280::attachStopDone(void (*function)(error_t)) {
	_onStopDone = function;
}

void SensBMP280::attachReadDone(void (*function)(sensor_data_t *, error_t)) {
	_onReadDone = function;
}
