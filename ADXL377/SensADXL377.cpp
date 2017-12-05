

#include "Senscape.h"
#include "SensADXL377.h"



void ((*SensADXL377::_onStartDone)(error_t));
void ((*SensADXL377::_onStopDone)(error_t));
void ((*SensADXL377::_onReadDone)(sensor_data_t *, error_t));

/* Constructors ***************************************************************/

SensADXL377::SensADXL377(uint16_t inchx,
		uint16_t inchy,
		uint16_t inchz,
		uint16_t sref,
		uint16_t ref2_5v,
		uint16_t ssel,
		uint16_t div,
		uint16_t sht,
		uint16_t sampcon_ssel,
		uint16_t sampcon_id
		) {

	/* Crete an ADC Sens for every axis */
	SensADXL377::_adcx = SensADC(
			inchx,
			sref,
			ref2_5v,
			ssel,
			div,
			sht,
			sampcon_ssel,
			sampcon_id);

	SensADXL377::_adcx = SensADC(
				inchy,
				sref,
				ref2_5v,
				ssel,
				div,
				sht,
				sampcon_ssel,
				sampcon_id);

	SensADXL377::_adcz = SensADC(
				inchz,
				sref,
				ref2_5v,
				ssel,
				div,
				sht,
				sampcon_ssel,
				sampcon_id);

    /* sensor default data */
	SensADXL377::_data._chanx = 0;
	SensADXL377::_data._chany = 0;
	SensADXL377::_data._chanz = 0;

    /* sensor default calibration of 0g */
	SensADXL377::_calib._chanx = 2048;
	SensADXL377::_calib._chany = 2048;
	SensADXL377::_calib._chanz = 2048;

}

/* Private Methods ************************************************************/


boolean SensADXL377::onSingleDataReadyChannelx(uint16_t data, error_t result) {
	// Conversion
	SensADXL377::_data._chanx = (data * 400000 / 4096) - (SensADXL377::_calib._chanx * 400000 / 4096);  //2^12 = 4096

	return false;
}

boolean SensADXL377::onSingleDataReadyChannely(uint16_t data, error_t result) {

	SensADXL377::_data._chany = (data * 400000 / 4096) - (SensADXL377::_calib._chany * 400000 / 4096);  //2^12 = 4096

	return false;
}

boolean  SensADXL377::onSingleDataReadyChannelz(uint16_t data, error_t result) {

	SensADXL377::_data._chanz = (data * 400000 / 4096) - (SensADXL377::_calib._chanz * 400000 / 4096);  //2^12 = 4096

	return false;
}


/* Public Methods *************************************************************/

error_t SensADXL377::readAccel(){

		// Attach ADC callbacks.
	SensADXL377::_adcx.attachCallback(onSingleDataReadyChannelx);
	SensADXL377::_adcy.attachCallback(onSingleDataReadyChannely);
	SensADXL377::_adcz.attachCallback(onSingleDataReadyChannelz);

		// Start new conversion channel[0..2].
	SensADXL377::_adcx.read();
	SensADXL377::_adcy.read();
	SensADXL377::_adcz.read();

	return SUCCESS;
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



