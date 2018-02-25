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

#ifndef SENSCAPE_BMP280_H_
#define SENSCAPE_BMP280_H_

#include "SensTimer.h"
#include "math.h"

//#define BMP280_TEST_PRESS_AND_TEMP_FORMULAS
//#define BMP280_DEBUG

#define BMP280_DELAY    (50)    // in msec

/* Chip ID Definition */
const uint8_t BMP280_CHIP_ID = 0x58;

/* Calibration Parameter Definition */
const uint8_t BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG = 0x88;
const uint8_t BMP280_TEMPERATURE_CALIB_DIG_T1_MSB_REG = 0x89;
const uint8_t BMP280_TEMPERATURE_CALIB_DIG_T2_LSB_REG = 0x8A;
const uint8_t BMP280_TEMPERATURE_CALIB_DIG_T2_MSB_REG = 0x8B;
const uint8_t BMP280_TEMPERATURE_CALIB_DIG_T3_LSB_REG = 0x8C;
const uint8_t BMP280_TEMPERATURE_CALIB_DIG_T3_MSB_REG = 0x8D;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P1_LSB_REG = 0x8E;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P1_MSB_REG = 0x8F;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P2_LSB_REG = 0x90;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P2_MSB_REG = 0x91;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P3_LSB_REG = 0x92;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P3_MSB_REG = 0x93;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P4_LSB_REG = 0x94;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P4_MSB_REG = 0x95;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P5_LSB_REG = 0x96;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P5_MSB_REG = 0x97;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P6_LSB_REG = 0x98;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P6_MSB_REG = 0x99;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P7_LSB_REG = 0x9A;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P7_MSB_REG = 0x9B;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P8_LSB_REG = 0x9C;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P8_MSB_REG = 0x9D;
const uint8_t BMP280_PRESSURE_CALIB_DIG_P9_LSB_REG = 0x9E;

/* Register Address Definition */
const uint8_t BMP280_CHIP_ID_REG = 0xD0;			/* chip ID register */
const uint8_t BMP280_RST_REG = 0xE0; 				/* soft-reset register */
const uint8_t BMP280_STAT_REG = 0xF3;       		/* status Register */
const uint8_t BMP280_CTRL_MEAS_REG = 0xF4;			/* ctrl measure register */
const uint8_t BMP280_CONFIG_REG = 0xF5;				/* configuration register */
const uint8_t BMP280_PRESSURE_MSB_REG = 0xF7;		/* pressure MSB register */
const uint8_t BMP280_PRESSURE_LSB_REG = 0xF8; 		/* pressure LSB register */
const uint8_t BMP280_PRESSURE_XLSB_REG = 0xF9;		/* pressure XLSB register */
const uint8_t BMP280_TEMPERATURE_MSB_REG = 0xFA;	/* temperature MSB register */
const uint8_t BMP280_TEMPERATURE_LSB_REG = 0xFB;	/* temperature LSB register */
const uint8_t BMP280_TEMPERATURE_XLSB_REG = 0xFC;	/* temperature XLSB register */

/* Status Flags Definition */
const uint8_t BMP280_STATUS_IMAGE_UPDATE_FLAG = 0x01;
const uint8_t BMP280_STATUS_MEASURING_FLAG = 0x08;

/* Standby Time Definition */
const uint8_t BMP280_STANDBY_TIME_1_MS = (0x00 << 5);
const uint8_t BMP280_STANDBY_TIME_63_MS = (0x01 << 5);
const uint8_t BMP280_STANDBY_TIME_125_MS = (0x02 << 5);
const uint8_t BMP280_STANDBY_TIME_250_MS = (0x03 << 5);
const uint8_t BMP280_STANDBY_TIME_500_MS = (0x04 << 5);
const uint8_t BMP280_STANDBY_TIME_1000_MS = (0x05 << 5);
const uint8_t BMP280_STANDBY_TIME_2000_MS = (0x06 << 5);
const uint8_t BMP280_STANDBY_TIME_4000_MS = (0x07 << 5);

/* Filter Definition */
const uint8_t BMP280_FILTER_COEF_OFF = (0x00 << 2);
const uint8_t BMP280_FILTER_COEF_2 = (0x01 << 2);
const uint8_t BMP280_FILTER_COEF_4 = (0x02 << 2);
const uint8_t BMP280_FILTER_COEF_8 = (0x03 << 2);
const uint8_t BMP280_FILTER_COEF_16 = (0x04 << 2);

/* Oversampling Definition */
const uint8_t BMP280_OVERSAMP_SKIPPED = 0x00;
const uint8_t BMP280_OVERSAMP_1X = 0x01;
const uint8_t BMP280_OVERSAMP_2X = 0x02;
const uint8_t BMP280_OVERSAMP_4X = 0x03;
const uint8_t BMP280_OVERSAMP_8X = 0x04;
const uint8_t BMP280_OVERSAMP_16X = 0x05;

/* Working Mode Definition */
const uint8_t BMP280_ULTRA_LOW_POWER = (BMP280_OVERSAMP_1X << 5)
		| (BMP280_OVERSAMP_1X << 2);	/* 5.5 msec */
const uint8_t BMP280_LOW_POWER = (BMP280_OVERSAMP_1X << 5)
		| (BMP280_OVERSAMP_2X << 2);	/* 7.5 msec */
const uint8_t BMP280_STANDARD_RESOLUTION = (BMP280_OVERSAMP_1X << 5)
		| (BMP280_OVERSAMP_4X << 2);	/* 11.5 msec */
const uint8_t BMP280_HIGH_RESOLUTION = (BMP280_OVERSAMP_1X << 5)
		| (BMP280_OVERSAMP_8X << 2);	/* 19.5 msec */
const uint8_t BMP280_ULTRA_HIGH_RESOLUTION = (BMP280_OVERSAMP_2X << 5)
		| (BMP280_OVERSAMP_16X << 2);	/* 37.5 msec */

/* Power Mode Definition  */
const uint8_t BMP280_SLEEP_MODE = 0x00;
const uint8_t BMP280_FORCED_MODE = 0x01;
const uint8_t BMP280_NORMAL_MODE = 0x03;
const uint8_t BMP280_RESET = 0xB6;

/* Mask Definition */
const uint8_t BMP280_POWER_MODE_MASK = B00000011;
const uint8_t BMP280_WORKING_MODE_MASK = B11111100;
const uint8_t BMP280_STANDBY_AND_FILTER_MASK = B11111100;

struct bmp280_state_t;

struct bmp280_data_t : sensor_data_t {
    int32_t t_fine;
    int32_t u_press;
    int32_t u_temp;
};

class SensBMP280 : public SensorClient {
	private:
        static bmp280_state_t _state;
        static bmp280_data_t _data;
        static SensTimer *_spiTimer;
		static Resource *_spiResource;
		static SPI *_spiObj;

		static uint8_t readRegister(uint8_t reg);
		static void writeRegister(uint8_t reg, uint8_t value);
		static void setPowerMode(uint8_t mode);
		static void setSettings(uint8_t resol, uint8_t standby_time, uint8_t filter_coef);
		static int32_t getFineResolutionTemperature(void);

		static void ((*_onStartDone)(error_t));
		static void ((*_onStopDone)(error_t));
		static void ((*_onReadDone)(sensor_data_t *, error_t));

		static void onSpiResourceGranted(void);
		static void onSpiTranferDone(uint8_t*, uint8_t*, uint16_t, error_t);
		static void onDataReady(void);
		static void onSignalDoneTask(void *);

    public:
		SensBMP280(SPI *spi,
				Resource *resource,
				SensTimer *timer,
				const uint8_t resolution,
				const uint8_t standby_time,
				const uint8_t filter_coeficient,
				const float_t sea_level_pressure);

        virtual error_t start(void);
        virtual error_t stop(void);
        virtual error_t read(void);
        virtual error_t readNow(void);
        virtual boolean_t isStarted(void);

		/* setters & getters */

        error_t setResolution(uint16_t resolution);
        error_t setStandbyTime(uint8_t time);
        error_t setFilterCoeficient(uint8_t coeficient);
        error_t setSeaLevelPressure(float_t pressure);

        float_t getPressure(sensor_data_t *data);
        float_t getAltitude(float_t pressure);
        float_t getTemperature(sensor_data_t *data);

		/* callbacks */

		virtual void attachStartDone(void (*)(error_t));
		virtual void attachStopDone(void (*)(error_t));
		virtual void attachReadDone(void (*)(sensor_data_t *, error_t));
};

#endif // SENSCAPE_BMP280_H_
