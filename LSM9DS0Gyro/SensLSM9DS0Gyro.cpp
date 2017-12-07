#include "Senscape.h"
#include "SensLSM9DS0Gyro.h"

typedef enum {
	S_IDLE,
	S_START,
	S_STOP,
	S_READ,
	S_READ_NOW
} lsm9ds0gyro_req_t;

struct lsm9ds0gyro_state_t {
	lsm9ds0gyro_req_t req;
	lsm9ds0gyro_scale_t scale;
	lsm9ds0gyro_odr_t odrate;
	boolean_t is_started;
	boolean_t is_ready;
};

uint8_t lsm9ds0_buffer[24] = { 0 };

lsm9ds0gyro_state_t SensLSM9DS0Gyro::_state = { S_IDLE, LSM9DS0_GYROSCALE_245DPS, G_ODR_95_BW_125, false, false};
lsm9ds0gyro_data_t SensLSM9DS0Gyro::_data;
Resource *SensLSM9DS0Gyro::_spiResource = NULL;
SPI *SensLSM9DS0Gyro::_spiObj = NULL;

void ((*SensLSM9DS0Gyro::_onStartDone)(error_t));
void ((*SensLSM9DS0Gyro::_onStopDone)(error_t));
void ((*SensLSM9DS0Gyro::_onReadDone)(sensor_data_t *, error_t));

SensLSM9DS0Gyro::SensLSM9DS0Gyro(SPI *spi, Resource *resource, const lsm9ds0gyro_scale_t scale) {
	_spiResource = resource;
	_spiObj = spi;
	_state.scale = scale;
	/* sensor default data */
	_data.x = 0.;
	_data.y = 0.;
	_data.z = 0.;
}

uint8_t SensLSM9DS0Gyro::readRegister(uint8_t reg) {
	uint8_t value;

	/* assert chip select */
	digitalWrite(LSM9DS0_G_CSN, LOW);

	/* read register */
	_spiObj->transfer(reg | 0x40 | 0x80);
	value = _spiObj->transfer(0xFF);

	/* de-assert chip select */
	digitalWrite(LSM9DS0_G_CSN, HIGH);
	return value;
}

void SensLSM9DS0Gyro::writeRegister(uint8_t reg, uint8_t value) {
	/* assert chip select */
	digitalWrite(LSM9DS0_G_CSN, LOW);

	/* write register */
	_spiObj->transfer(reg | 0x40);
	_spiObj->transfer(value);

	/* de-assert chip select */
	digitalWrite(LSM9DS0_G_CSN, HIGH);
}

void SensLSM9DS0Gyro::onSpiTransferDone(uint8_t* tx_buf, uint8_t* rx_buf, uint16_t len, error_t result) {
	/* de-assert chip select */
	digitalWrite(LSM9DS0_G_CSN, HIGH);

	switch(_state.req) {
		case S_START:
			if (result == SUCCESS) {
				//TODO
			}
			postTask(onSignalDoneTask, (void*)(uint16_t)result);
			break;
		case S_READ:
		case S_READ_NOW:
			if (result == SUCCESS) {
				_data.x = ((rx_buf[1] << 8) | rx_buf[0]) * _gyro_dps_digit;
				_data.y = ((rx_buf[3] << 8) | rx_buf[2]) * _gyro_dps_digit;
				_data.z = ((rx_buf[5] << 8) | rx_buf[4])  * _gyro_dps_digit;
			}
			postTask(onSignalDoneTask, (void*)(uint16_t)result);
			break;
		default:
			/* fatal error */
			postTask(onSignalDoneTask, (void*)(uint16_t)ERROR);
			break;
	}
}

void SensLSM9DS0Gyro::onSpiResourceGranted() {
    /* enable SPI bus */
    _spiObj->begin();
	/* attach SPI tranfer callback */
    _spiObj->attachTransferDone(onSpiTransferDone);

    switch(_state.req) {
    	case S_START:
    		/* check device id */
    		if (readRegister(LSM9DS0_REGISTER_WHO_AM_I_G) == LSM9DS0_G_ID) {
    			//TODO + Reset?
    		}
    		else {
    			/* invalid device id */
    			postTask(onSignalDoneTask, (void*)(uint16_t)ERROR);
    		}
    		break;
    	case S_STOP:
    		/** Nothing to be done in here since the sensor is continuous */
    		postTask(onSignalDoneTask, (void*)(uint16_t)SUCCESS);
    		break;
    	case S_READ:
    	case S_READ_NOW:
    		if (!_state.is_ready) {
    		    if (_state.req == S_READ) {
                    /* put sensor in normal mode */
                    //setPowerMode(BMP280_NORMAL_MODE);
    		    }
    		    else {
    		        /* put sensor in forced mode */
    		        //setPowerMode(BMP280_FORCED_MODE);
    		    }
    			/* wait for measure completed (100 msec max) */
    			//_spiTimer->startOneShot(LSM9DS0_DELAY);

    			_state.is_ready = true;
    		}
    		else {
    	        digitalWrite(LSM9DS0_G_CSN, LOW);
    	        
    	        _spiObj->transfer(LSM9DS0_REGISTER_OUT_X_L_G | 0x80 | 0x40);
    	        _spiObj->transfer(NULL, lsm9ds0_buffer, 6);
    		}
    		break;
        default:
        	/* fatal error */
        	postTask(onSignalDoneTask, (void*)(uint16_t)ERROR);
            break;
    }
}

void SensLSM9DS0Gyro::onDataReady() {
    if (_spiResource->isOwner()) {
        /* start reading X,Y and Z registers */
        digitalWrite(LSM9DS0_G_CSN, LOW);
        _spiObj->transfer(LSM9DS0_REGISTER_OUT_X_L_G | 0x40 | 0x80);
        _spiObj->transfer(NULL, lsm9ds0_buffer, 6);
    }
    else {
        /* fatal error */
        postTask(onSignalDoneTask, (void*)(uint16_t)ERROR);
    }
}

void SensLSM9DS0Gyro::onSignalDoneTask(void *param) {
	lsm9ds0gyro_req_t req = _state.req;
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
                /* notify start event */
                _onStartDone(result);
    		}
    		break;
    	case S_STOP:
    		if (_onStopDone) {
				_state.is_started = false;
                /* detach SPI resource callback */
                _spiResource->detachResourceGranted();
                /* configure chip select as input */
                pinMode(LSM9DS0_G_CSN, INPUT);
                /* notify stop event */
                _onStopDone(result);
    		}
    		break;
    	case S_READ:
    	case S_READ_NOW:
    		if (_onReadDone) {
                if (result == SUCCESS) {
#ifdef LSM9DS0Gyro_DEBUG
                    Debug.println();
                    Debug.print("X axis: ").println(_data.x);
                    Debug.print("Y axis:").println(_data.y);
                    Debug.print("Z axis:").println(_data.z);
#endif
                }
                /* notify data event */
				_onReadDone((sensor_data_t*)&_data, (error_t)(uint16_t)result);
    		}
    		break;
        default:
            break;
    }
}

/** Reset de l'escala */
void SensLSM9DS0Gyro::setScale(lsm9ds0gyro_scale_t scale) {
	uint8_t reg = readRegister(LSM9DS0_REGISTER_CTRL_REG4_G);
	reg &= ~(0b00110000);
	reg |= scale;
	writeRegister(LSM9DS0_REGISTER_CTRL_REG4_G, reg );

	switch(scale)
	{
	  case LSM9DS0_GYROSCALE_245DPS:
	      _gyro_dps_digit = LSM9DS0_GYRO_DPS_DIGIT_245DPS;
	      break;
	  case LSM9DS0_GYROSCALE_500DPS:
	      _gyro_dps_digit = LSM9DS0_GYRO_DPS_DIGIT_500DPS;
	      break;
	  case LSM9DS0_GYROSCALE_2000DPS:
	      _gyro_dps_digit = LSM9DS0_GYRO_DPS_DIGIT_2000DPS;
	      break;
	}
}

void SensLSM9DS0Gyro::setODRate(lsm9ds0gyro_odr_t gRate) {
	
	uint8_t reg = readRegister(LSM9DS0_REGISTER_CTRL_REG1_G);
	/* Mask out the gyro ODR bits */
	reg &= 0xFF^(0xF << 4);
	/* Shift in our new ODR bits */
	reg |= (gRate << 4);
	/* Write the new register value back into CTRL_REG1_G */
	writeRegister(LSM9DS0_REGISTER_CTRL_REG1_G, reg);
}

error_t SensLSM9DS0Gyro::start() {
    if (!_state.is_started) {
        if (_state.req == S_IDLE) {
            _state.req = S_START;
            
            /* configure chip select as high output */
            pinMode(LSM9DS0_G_CSN, OUTPUT);
            digitalWrite(LSM9DS0_G_CSN, HIGH);
            
            /* Makes the Gyro continuous */
            uint8_t continuous = 0b00001111;
            writeRegister(LSM9DS0_REGISTER_CTRL_REG1_G, continuous);
            
            /* Set default values */
            writeRegister(LSM9DS0_REGISTER_CTRL_REG2_G, 0x00);
            writeRegister(LSM9DS0_REGISTER_CTRL_REG4_G, 0x00);
            writeRegister(LSM9DS0_REGISTER_CTRL_REG5_G, 0x00);

            /* Set output data rate - Reg.1 */
            setODRate(_state.odrate);

            /* Set scale settings - Reg.4 */
            setScale(_state.scale);
            
            /* attach SPI resource */
            _spiResource->attachResourceGranted(onSpiResourceGranted);

            /* request SPI resource */
            return _spiResource->request();
        }
        return EBUSY;
    }
    return ERROR;
}

error_t SensLSM9DS0Gyro::stop() {
	if (_state.req == S_IDLE) {
		_state.req = S_STOP;

		/* request SPI resource */
		return _spiResource->request();
	}
	return EBUSY;
}

error_t SensLSM9DS0Gyro::read() {
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

error_t SensLSM9DS0Gyro::readNow() {
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

boolean_t SensLSM9DS0Gyro::isStarted(void) {
    return _state.is_started;
}

void SensLSM9DS0Gyro::attachStartDone(void (*function)(error_t)) {
	_onStartDone = function;
}

void SensLSM9DS0Gyro::attachStopDone(void (*function)(error_t)) {
	_onStopDone = function;
}

void SensLSM9DS0Gyro::attachReadDone(void (*function)(sensor_data_t *, error_t)) {
	_onReadDone = function;
}
