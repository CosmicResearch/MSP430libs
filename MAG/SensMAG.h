#ifndef SENSCAPE_MAG_H_
#define SENSCAPE_MAG_H_

#define CHIP_CS_XM	(19) //19

#define LSM9DS0_MAG_MGAUSS_2GAUSS      (0.08F)
#define LSM9DS0_MAG_MGAUSS_4GAUSS      (0.16F)
#define LSM9DS0_MAG_MGAUSS_8GAUSS      (0.32F)
#define LSM9DS0_MAG_MGAUSS_12GAUSS     (0.48F)

#include "Senscape.h"
#include "sensor_types.h"

//* Status Register Bits */

const uint8_t LSM9DS0_S_WIP_BIT = 0x01;        // Write-In-Process
const uint8_t LSM9DS0_S_WEL_BIT = 0x02;        // Write Enable Latch
const uint8_t LSM9DS0           = 0x04;        // Block Protection (low-bit)
const uint8_t LSM9DS0_S_BP1_BIT = 0x08;        // Block Protection (high-bit)

/* Magnetometer/Acceletometer Registers */

const uint8_t LSM9DS0_REGISTER_TEMP_OUT_L_XM       = 0x05;
const uint8_t LSM9DS0_REGISTER_TEMP_OUT_H_XM       = 0x06;
const uint8_t LSM9DS0_REGISTER_STATUS_REG_M        = 0x07;
const uint8_t LSM9DS0_REGISTER_OUT_X_L_M           = 0x08;
const uint8_t LSM9DS0_REGISTER_OUT_X_H_M           = 0x09;
const uint8_t LSM9DS0_REGISTER_OUT_Y_L_M           = 0x0A;
const uint8_t LSM9DS0_REGISTER_OUT_Y_H_M           = 0x0B;
const uint8_t LSM9DS0_REGISTER_OUT_Z_L_M           = 0x0C;
const uint8_t LSM9DS0_REGISTER_OUT_Z_H_M           = 0x0D;
const uint8_t LSM9DS0_REGISTER_WHO_AM_I_XM         = 0x0F;
const uint8_t LSM9DS0_REGISTER_INT_CTRL_REG_M      = 0x12;
const uint8_t LSM9DS0_REGISTER_INT_SRC_REG_M       = 0x13;
const uint8_t LSM9DS0_REGISTER_OFFSET_X_L_M		  = 0x16;
const uint8_t LSM9DS0_REGISTER_OFFSET_X_H_M		  = 0x17;
const uint8_t LSM9DS0_REGISTER_OFFSET_Y_L_M		  = 0x18;
const uint8_t LSM9DS0_REGISTER_OFFSET_Y_H_M		  = 0x19;
const uint8_t LSM9DS0_REGISTER_OFFSET_Z_L_M		  = 0x1A;
const uint8_t LSM9DS0_REGISTER_OFFSET_Z_H_M		  = 0x1B;
const uint8_t LSM9DS0_REGISTER_CTRL_REG0_XM        = 0x19;
const uint8_t LSM9DS0_REGISTER_CTRL_REG1_XM        = 0x20;
const uint8_t LSM9DS0_REGISTER_CTRL_REG2_XM        = 0x21;
const uint8_t LSM9DS0_REGISTER_CTRL_REG3_XM        = 0x22;
const uint8_t LSM9DS0_REGISTER_CTRL_REG4_XM        = 0x23;
const uint8_t LSM9DS0_REGISTER_CTRL_REG5_XM        = 0x24;
const uint8_t LSM9DS0_REGISTER_CTRL_REG6_XM        = 0x25;
const uint8_t LSM9DS0_REGISTER_CTRL_REG7_XM        = 0x26;
const uint8_t LSM9DS0_REGISTER_OUT_X_L_A           = 0x28;
const uint8_t LSM9DS0_REGISTER_OUT_X_H_A           = 0x29;
const uint8_t LSM9DS0_REGISTER_OUT_Y_L_A           = 0x2A;
const uint8_t LSM9DS0_REGISTER_OUT_Y_H_A           = 0x2B;
const uint8_t LSM9DS0_REGISTER_OUT_Z_L_A           = 0x2C;
const uint8_t LSM9DS0_REGISTER_OUT_Z_H_A           = 0x2D;

//* Magnetometer Gain */

const uint8_t LSM9DS0_MAGGAIN_2GAUSS               = (0b00 << 5);  // +/- 2 gauss
const uint8_t LSM9DS0_MAGGAIN_4GAUSS               = (0b01 << 5);  // +/- 4 gauss
const uint8_t LSM9DS0_MAGGAIN_8GAUSS               = (0b10 << 5);  // +/- 8 gauss
const uint8_t LSM9DS0_MAGGAIN_12GAUSS              = (0b11 << 5);  // +/- 12 gauss

/* Magnetometer Data rate */

const uint8_t LSM9DS0_MAGDATARATE_3_125HZ          = (0b000 << 2);
const uint8_t LSM9DS0_MAGDATARATE_6_25HZ           = (0b001 << 2);
const uint8_t LSM9DS0_MAGDATARATE_12_5HZ           = (0b010 << 2);
const uint8_t LSM9DS0_MAGDATARATE_25HZ             = (0b011 << 2);
const uint8_t LSM9DS0_MAGDATARATE_50HZ             = (0b100 << 2);
const uint8_t LSM9DS0_MAGDATARATE_100HZ            = (0b101 << 2);


enum mag_scale
{
	M_SCALE_2GS     = (0b00 << 5),    // 00:  2Gs
	M_SCALE_4GS     = (0b01 << 5),    // 01:  4Gs
	M_SCALE_8GS     = (0b10 << 5),    // 10:  8Gs
	M_SCALE_12GS    = (0b11 << 5)     // 11:  12Gs
};

enum mag_odr
{
	M_ODR_3125      = (0b000 << 2),   // 3.125 Hz (0x00)
	M_ODR_625       = (0b001 << 2),   // 6.25 Hz (0x01)
	M_ODR_125       = (0b010 << 2),   // 12.5 Hz (0x02)
	M_ODR_25        = (0b011 << 2),   // 25 Hz (0x03)
	M_ODR_50        = (0b100 << 2),   // 50 (0x04)
	M_ODR_100       = (0b101 << 2),   // 100 Hz (0x05)
};


struct lsm9ds0_state_t;

struct mag_offsets {
    int16_t x, y, z;
};

class SensMAG : public SensorClient {

    private:
        static mag_offsets mag_off;

        static float_t _mag_mgauss_lsb;

		static mag_data_t _data;
		static lsm9ds0_state_t _state;
		static Resource *_spiResource;
		static SPI *_spiObj;

        static mag_scale mScale;
	    static mag_odr mRate;
		static float_t mRes;

		static int16_t *_calibX;
		static int16_t *_calibY;
		static int16_t *_calibZ;

        static void ((*_onRequestAccelMagIdDone)(uint8_t *id, error_t));
        static void ((*_onStartDone)(error_t));
        static void ((*_onStopDone)(error_t));
        static void ((*_onCalibrationDone)(error_t));
        static void ((*_onReadDone)(sensor_data_t *, error_t));

        static void calcaRes();
        static void initMag();

        static void setMagScale(mag_scale mScl);
        static void setMagODR(mag_odr mRate);

        static uint8_t readRegister(uint8_t reg);
        static void readBuffer(uint8_t addr, uint8_t *buffer, uint8_t len);


        static void writeRegister(uint8_t addr, uint8_t data);
        static void writeBuffer(uint8_t addr, uint8_t *buffer, uint8_t len);

        static void onSpiResourceGranted(void);
        static void onSpiTranferDone(uint8_t*, uint8_t*, uint16_t, error_t);
        static void onSignalDoneTask(void*);

    public:
   		SensMAG(SPI *spi, Resource *resource, mag_scale mScl, mag_odr mRate);

   		virtual error_t start(void);
		virtual error_t stop(void);
		virtual error_t read(void);
		virtual error_t readNow(void);
		virtual boolean_t isStarted(void);

        static float_t calcmRes();

		//Convert from RAW signed 16-bit value to Gauss (Gs)
		float calcMag(int32_t mag);

	 	error_t requestAccelMagId();
	 	error_t getMagnetism(int16_t *xhi, int16_t *yhi, int16_t *zhi);
	 	error_t calibrate(int16_t *x, int16_t *y, int16_t *z);

           /* call-backs */
	   void attachRequestAccelMagIdDone(void (*)(uint8_t *id, error_t));
	   virtual void attachStartDone(void (*)(error_t));
	   virtual void attachStopDone(void (*)(error_t));
	   void attachCalibrationDone(void (*)(error_t));
	   virtual void attachReadDone(void (*)(sensor_data_t *, error_t));


};


#endif // SENSCAPE_MAG_H_
