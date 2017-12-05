

#ifndef SENSCAPE_ADXL377_H_
#define SENSCAPE_ADXL377_H_
#include "SensADC.h"

/* Chip ID Definition */
const uint8_t ADXL377_CHIP_ID = 0x59;


struct adxl377_data_t : sensor_data_t{
	int32_t _chanx;
	int32_t _chany;
	int32_t _chanz;
};

struct adxl377_calib_t {
	int16_t _chanx;
	int16_t _chany;
	int16_t _chanz;
};


class SensADXL377:SensorClient{
	private:
		static SensADC _adcx;
		static SensADC _adcy;
		static SensADC _adcz;
		static adxl377_data_t _data;
		static adxl377_calib_t _calib;

		static void ((*_onStartDone)(error_t));
		static void ((*_onStopDone)(error_t));
		static void ((*_onReadDone)(sensor_data_t *, error_t));

		static boolean onSingleDataReadyChannelx(uint16_t data, error_t result);
		static boolean onSingleDataReadyChannely(uint16_t data, error_t result);
		static boolean onSingleDataReadyChannelz(uint16_t data, error_t result);

	public:
		SensADXL377(uint16_t inchx,
				uint16_t inchy,
				uint16_t inchz,
				uint16_t sref,
				uint16_t ref2_5v,
				uint16_t ssel,
				uint16_t div,
				uint16_t sht,
				uint16_t sampcon_ssel,
				uint16_t sampcon_id);

		error_t readAccel(adxl377_data_t *data);

		/* callbacks */
		virtual void attachStartDone(void (*)(error_t));
		virtual void attachStopDone(void (*)(error_t));
		virtual void attachReadDone(void (*)(sensor_data_t *, error_t));

};

#endif //
