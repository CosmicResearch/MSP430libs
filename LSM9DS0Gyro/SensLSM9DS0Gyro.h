#ifndef __LSM9DS0_H__
#define __LSM9DS0_H__

#define GYROTYPE                           (true)
#define LSM9DS0_DELAY    (50)    // in msec

//#define LSM9DS0_ADDRESS_GYRO               (0x6B)         // D6 >> 1 = 7bit default
//#define LSM9DS0_G_ID                       (0b11010100)
const uint8_t LSM9DS0_ADDRESS_GYRO = 0x6B;
const uint8_t LSM9DS0_G_ID = 0xD4;
/** ^^alternatives^^ */

const uint8_t LSM9DS0_REGISTER_WHO_AM_I_G     	   = 0x0F;
const uint8_t LSM9DS0_REGISTER_CTRL_REG1_G         = 0x20;
const uint8_t LSM9DS0_REGISTER_CTRL_REG2_G         = 0x21;
const uint8_t LSM9DS0_REGISTER_CTRL_REG3_G         = 0x22;
const uint8_t LSM9DS0_REGISTER_CTRL_REG4_G         = 0x23;
const uint8_t LSM9DS0_REGISTER_CTRL_REG5_G         = 0x24;
const uint8_t LSM9DS0_REGISTER_OUT_X_L_G           = 0x28;
const uint8_t LSM9DS0_REGISTER_OUT_X_H_G           = 0x29;
const uint8_t LSM9DS0_REGISTER_OUT_Y_L_G           = 0x2A;
const uint8_t LSM9DS0_REGISTER_OUT_Y_H_G           = 0x2B;
const uint8_t LSM9DS0_REGISTER_OUT_Z_L_G           = 0x2C;
const uint8_t LSM9DS0_REGISTER_OUT_Z_H_G           = 0x2D;
const uint8_t LSM9DS0_GYRO_DPS_DIGIT_245DPS        = (0b00 << 4);  // +/- 245 degrees per second rotation
const uint8_t LSM9DS0_GYRO_DPS_DIGIT_500DPS        = (0b01 << 4);  // +/- 500 degrees per second rotation
const uint8_t LSM9DS0_GYRO_DPS_DIGIT_2000DPS       = (0b10 << 4);  // +/- 2000 degrees per second rotation

struct lsm9ds0gyro_state_t;

struct lsm9ds0gyro_data_t : sensor_data_t {
	float_t x;
	float_t y;
	float_t z;
};

typedef enum
{
	LSM9DS0_GYROSCALE_245DPS = (0b00 << 4),
	LSM9DS0_GYROSCALE_500DPS = (0b01 << 4),
	LSM9DS0_GYROSCALE_2000DPS = (0b10 << 4)
} lsm9ds0gyro_scale_t;

typedef enum {							// ODR (Hz) --- Cutoff
		G_ODR_95_BW_125  = 0x0, //   95         12.5
		G_ODR_95_BW_25   = 0x1, //   95          25
		// 0x2 and 0x3 define the same data rate and bandwidth
		G_ODR_190_BW_125 = 0x4, //   190        12.5
		G_ODR_190_BW_25  = 0x5, //   190         25
		G_ODR_190_BW_50  = 0x6, //   190         50
		G_ODR_190_BW_70  = 0x7, //   190         70
		G_ODR_380_BW_20  = 0x8, //   380         20
		G_ODR_380_BW_25  = 0x9, //   380         25
		G_ODR_380_BW_50  = 0xA, //   380         50
		G_ODR_380_BW_100 = 0xB, //   380         100
		G_ODR_760_BW_30  = 0xC, //   760         30
		G_ODR_760_BW_35  = 0xD, //   760         35
		G_ODR_760_BW_50  = 0xE, //   760         50
		G_ODR_760_BW_100 = 0xF, //   760         100
} lsm9ds0gyro_odr_t;

class SensLSM9DS0Gyro : public SensorClient {
	private:
		static lsm9ds0gyro_state_t _state;
        static lsm9ds0gyro_data_t _data;

		static Resource *_spiResource;
		static SPI *_spiObj;
		static float _gyro_dps_digit;

		static uint8_t readRegister(uint8_t reg);
		static void writeRegister(uint8_t reg, uint8_t value);

		static void ((*_onStartDone)(error_t));
		static void ((*_onStopDone)(error_t));
		static void ((*_onReadDone)(sensor_data_t *, error_t));

		static void onSpiResourceGranted(void);
		static void onSpiTransferDone(uint8_t*, uint8_t*, uint16_t, error_t);
		static void onDataReady(void);
		static void onSignalDoneTask(void *);
		
	public:
		SensLSM9DS0Gyro(SPI *spi, Resource *resource, const lsm9ds0gyro_scale_t scale);

		virtual error_t start(void);
        virtual error_t stop(void);
        virtual error_t read(void);
        virtual error_t readNow(void);
        virtual boolean_t isStarted(void);
        
        /** setter & getters */
        void setScale(lsm9ds0gyro_scale_t scale);
        void setODRate(lsm9ds0gyro_odr_t gRate);
        
        /** callbacks */
        virtual void attachStartDone(void (*)(error_t));
		virtual void attachStopDone(void (*)(error_t));
		virtual void attachReadDone(void (*)(sensor_data_t *, error_t));
};

#endif
