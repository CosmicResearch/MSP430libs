#include "Senscape.h"
#include "SensMAG.h"

typedef enum {
	S_IDLE,
	S_STOP,
	S_START,
	S_READ,
	S_CALIB,
	S_XM_CHIP_ID
} lsm9ds0_req_t;

struct lsm9ds0_state_t {
	boolean_t is_started;
	boolean_t is_ready;
    lsm9ds0_req_t req;
};

struct lsm9ds0_data_t : sensor_data_t {
    int16_t x;
    int16_t y;
    int16_t z;
    //int32_t u_temp;
};

uint8_t lsm9ds0_buffer[24] =  { 0 };
uint8_t lsm9ds0_xm_id;

lsm9ds0_state_t SensMAG::_state = {false, false, S_IDLE };
lsm9ds0_data_t SensMAG::_data;
Resource *SensMAG::_spiResource = NULL;
SPI *SensMAG::_spiObj = NULL;

int16_t *SensMAG::_calibX = 0;
int16_t *SensMAG::_calibY = 0;
int16_t *SensMAG::_calibZ = 0;

mag_scale SensMAG::mScale = M_SCALE_2GS;
mag_odr SensMAG::mRate = M_ODR_125;
float_t SensMAG::mRes = 00;

void ((*SensMAG::_onRequestAccelMagIdDone)(uint8_t *id, error_t));
void ((*SensMAG::_onStartDone)(error_t));
void ((*SensMAG::_onStopDone)(error_t));
void ((*SensMAG::_onCalibrationDone)(error_t));
void ((*SensMAG::_onReadDone)(sensor_data_t *, error_t));


/* Constructors ***************************************************************/

SensMAG::SensMAG(SPI *spi, Resource *resource, mag_scale mScl, mag_odr mOdr){
    _spiResource = resource;
    _spiObj = spi;
    mRate = mOdr;
    mScale = mScl;
    _data.x = 0;
    _data.y = 0;
    _data.z = 0;
}

/* Private Methods ************************************************************/

void SensMAG::calcmRes(){
    mRes = mScale == M_SCALE_2GS ? 2.0 / 32768.0 :
    		(float_t) (mScale << 2) / 32768.0;
}

void SensMAG::readRegister(uint8_t addr, uint8_t *data){
    digitalWrite(CHIP_CS_XM, LOW); // Initiate communication
	_spiObj->transfer((addr & 0x3F) | 0x80);
	*data = _spiObj->transfer(0xFF);
    digitalWrite(CHIP_CS_XM, HIGH); // Close communication
}

uint8_t SensMAG::readRegister(uint8_t reg) {
	uint8_t value;
	/* assert chip select */
    digitalWrite(CHIP_CS_XM, LOW);

	/* read register */
    _spiObj->transfer((reg & 0x3F) | 0x80);
    value = _spiObj->transfer(0xFF);

	/* de-assert chip select */
    digitalWrite(CHIP_CS_XM, HIGH);
	return value;
}

void SensMAG::readBuffer(uint8_t addr, uint8_t *buffer, uint8_t len) {
    digitalWrite(CHIP_CS_XM, LOW); // Initiate communication
	//_spiObj->transfer(addr | 0x80 | 0x40);
    //for (uint8_t i = 0; i < len; ++i){
    	//	buffer[i] = _spiObj->transfer(0xFF);
    //}
    _spiObj->transfer(addr | 0x80);
    _spiObj->transfer(NULL, buffer, len);
    digitalWrite(CHIP_CS_XM, HIGH); // Close communication
}

void SensMAG::writeRegister(uint8_t addr, uint8_t data) {
    digitalWrite(CHIP_CS_XM, LOW); // Initiate communication
    _spiObj->transfer(addr & 0x3F);
    _spiObj->transfer(data);
    digitalWrite(CHIP_CS_XM, HIGH); // Close communication
}

void SensMAG::writeBuffer(uint8_t addr, uint8_t *buffer, uint8_t len) {
    digitalWrite(CHIP_CS_XM, LOW); // Initiate communication
    _spiObj->transfer(addr & 0x7F);
    for (int i = 0; i < len; ++i){
    		_spiObj->transfer(buffer[i]);
    }
    digitalWrite(CHIP_CS_XM, HIGH); // Close communication
}

void SensMAG::initMag(){
	writeRegister(LSM9DS0_REGISTER_CTRL_REG5_XM, 0x94); // Mag data rate - 100 Hz, enable temperature sensor, low magnetic resolution
	writeRegister(LSM9DS0_REGISTER_CTRL_REG6_XM, 0x00); // Mag scale to +/- 2GS
	writeRegister(LSM9DS0_REGISTER_CTRL_REG7_XM, 0x00); // Continuous conversion mode
	writeRegister(LSM9DS0_REGISTER_CTRL_REG4_XM, 0x04); // Magnetometer data ready on INT2_XM (0x08)
	writeRegister(LSM9DS0_REGISTER_INT_CTRL_REG_M, 0x08); // Disable interrupts for mag, active-low, push-pull [My choice to disable]
}


void SensMAG::setMagScale(mag_scale mScl)
{
    // We need to preserve the other bytes in CTRL_REG6_XM. So, first read it:
	temp = readRegister(LSM9DS0_REGISTER_CTRL_REG6_XM);
    // Then mask out the mag scale bits:
    temp &= 0xFF^(0x3 << 5);
    // Then shift in our new scale bits:
    temp |= mScl << 5;
    // And write the new register value back into CTRL_REG6_XM:
    writeRegister(LSM9DS0_REGISTER_CTRL_REG6_XM, temp);

    mScale = mScl;
    calcmRes();
}


void SensMAG::setMagODR(mag_odr mRate){
    // We need to preserve the other bytes in CTRL_REG5_XM. So, first read it:
    uint8_t temp = readRegister(LSM9DS0_REGISTER_CTRL_REG5_XM);
    // Then mask out the mag ODR bits:
    temp &= 0xFF^(0x7 << 2);
    // Then shift in our new ODR bits:
    temp |= (mRate << 2);
    // And write the new register value back into CTRL_REG5_XM:
    writeRegister(LSM9DS0_REGISTER_CTRL_REG5_XM, temp);
}

void SensMAG::onSpiResourceGranted() {
    /* enable SPI bus */
    _spiObj->begin();
    /* attach SPI callbacks */
    _spiObj->attachTransferDone(onSpiTranferDone);

    switch(_state.req) {
		case S_START:
			//initMag();
			writeRegister(LSM9DS0_REGISTER_CTRL_REG7_XM, 0x00); // Continuous conversion mode
			//setMagODR(mRate);
			//setMagScale(mScale);							   // Possible fix is using the transer
			postTask(onSignalDoneTask, (void*)(uint16_t)SUCCESS);
			break;
		case S_STOP:
			postTask(onSignalDoneTask, (void*)(uint16_t)SUCCESS);
			break;
		case S_XM_CHIP_ID:
			lsm9ds0_xm_id = readRegister(LSM9DS0_REGISTER_WHO_AM_I_XM);
			postTask(onSignalDoneTask, (void*)(uint16_t)SUCCESS);
			break;
		case S_READ:
			readBuffer(LSM9DS0_REGISTER_OUT_X_L_M, lsm9ds0_buffer, 6);
			break;
		case S_CALIB:
			writeRegister(LSM9DS0_REGISTER_OFFSET_X_L_M, (uint8_t)(*_calibX & 0xFF));
			writeRegister(LSM9DS0_REGISTER_OFFSET_X_H_M, (uint8_t)((*_calibX & 0xFF00) >> 8));
			writeRegister(LSM9DS0_REGISTER_OFFSET_Y_L_M, (uint8_t)(*_calibY & 0xFF));
			writeRegister(LSM9DS0_REGISTER_OFFSET_Y_H_M, (uint8_t)((*_calibY & 0xFF00) >> 8));
			writeRegister(LSM9DS0_REGISTER_OFFSET_Z_L_M, (uint8_t)(*_calibZ & 0xFF));
			writeRegister(LSM9DS0_REGISTER_OFFSET_Z_H_M, (uint8_t)((*_calibZ & 0xFF00) >> 8));
			break;
		case S_IDLE:
	default:
		// TODO: release SPI bus!
		postTask(onSignalDoneTask, (void*)(uint16_t)ERROR);
		break;
    }
}

void SensMAG::onSpiTranferDone(uint8_t* tx_buf, uint8_t* rx_buf, uint16_t len, error_t result) {
    digitalWrite(CHIP_CS_XM, HIGH);

    switch(_state.req) {
    		case S_START:
    			postTask(onSignalDoneTask, (void*)(uint16_t)result);
    			break;
    		case S_READ:
    			if (result == SUCCESS){
					_data.x = rx_buf[1]; // xhi
					_data.x <<= 8;
					_data.x |= rx_buf[0]; //xlo

					_data.y |= rx_buf[3]; //yhi
					_data.y <<= 8;
					_data.y = rx_buf[2]; //ylo

					_data.z |= rx_buf[5]; //zlo
					_data.z <<= 8;
					_data.z |= rx_buf[4]; //zhi
    			}
    			postTask(onSignalDoneTask, (void*)(uint16_t)result);
    			break;
    		case S_CALIB:
    			postTask(onSignalDoneTask, (void*)(uint16_t)result);
    			break;
        case S_IDLE:
        default:
            break;
    }

    /* disable SPI bus */
    _spiObj->end();

    /* detach SPI callbacks */
    _spiObj->detachTransferDone();

    /* release SPI resource */
    _spiResource->release();
}


void SensMAG::onSignalDoneTask(void *param) {
	lsm9ds0_req_t req = _state.req;

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
                _onStartDone(result);
        		}
        		break;
    		case S_STOP:
    			if (_onStopDone) {
					_state.is_started = false;
					/* detach SPI resource callback */
					_spiResource->detachResourceGranted();
					/* configure chip select as input */
					pinMode(CHIP_CS_XM, INPUT);
					/* notify stop event */
					_onStopDone(result);
    			}
			break;
    		case S_XM_CHIP_ID:
			if (_onRequestAccelMagIdDone){
				_onRequestAccelMagIdDone(&lsm9ds0_xm_id, result);
			}
			break;
    		case S_READ:
    			if (_onReadDone) {
    				_onReadDone((sensor_data_t*)&_data, result);
    			}
    			break;
    		case S_CALIB:
    			if (_onCalibrationDone){
    				_onCalibrationDone(result);
    			}
		case S_IDLE:
		default:
			break;
        }
}

/* Public Methods *************************************************************/

error_t SensMAG::start() {
    if (!_state.is_started) {
        if (_state.req == S_IDLE) {
            _state.req = S_START;

            /* configure chip select as high output */
            pinMode(CHIP_CS_XM, OUTPUT);
            digitalWrite(CHIP_CS_XM, HIGH);

            /* attach SPI resource */
            _spiResource->attachResourceGranted(onSpiResourceGranted);

            /* request SPI resource */
            return _spiResource->request();
        }
        return EBUSY;
    }
    return ERROR;
}

error_t SensMAG::stop() {
	if (_state.req == S_IDLE) {
		_state.req = S_STOP;

		/* request SPI resource */
		return _spiResource->request();
	}
	return EBUSY;
}

boolean_t SensMAG::isStarted(void) {
    return _state.is_started;
}

error_t SensMAG::read() {
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

error_t SensMAG::readNow(void) {
	return ERROR;
}

/* Setters & Getters **********************************************************/
error_t SensMAG::requestAccelMagId() {
    if (_state.req == S_IDLE) {
        _state.req = S_XM_CHIP_ID;
        return _spiResource->request();
    }
    return EBUSY;
}

error_t SensMAG::getMagnetism(int16_t *mag_x, int16_t *mag_y, int16_t *mag_z){
	if (_state.is_started){
		*mag_x = _data.x;
		*mag_y = _data.y;
		*mag_z = _data.z;
		return SUCCESS;
	}
	return ERROR;
}

error_t SensMAG::calibrate(int16_t *x, int16_t *y, int16_t *z){
	if (_state.is_started){
		if (_state.req == S_IDLE) {
			_calibX = x;
			_calibY = y;
			_calibZ = z;
			_state.req = S_CALIB;
			return _spiResource->request();
		}
		return EBUSY;
	}
	return ERROR;
}

/* call-backs */

void SensMAG::attachRequestAccelMagIdDone(void (*function)(uint8_t *id, error_t)) {
    _onRequestAccelMagIdDone = function;
}

void SensMAG::attachStartDone(void (*function)(error_t)){
	_onStartDone = function;
}
void SensMAG::attachStopDone(void (*function)(error_t)){
	_onStopDone = function;
}

void SensMAG::attachCalibrationDonde(void (*function)(error_t)){
	_onCalibrationDone = function;
}

void SensMAG::attachReadDone(void (*function)(sensor_data_t *, error_t)){
	_onReadDone = function;
}

