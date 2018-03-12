

#include "Senscape.h"
#include "SensTimer.h"
#include "SensADC.h"
#include "SensADXL377.h"

SensTimer timer = SensTimer(TMILLIS);

SensADC _adcx =  SensADC(
				ADC_CHANNEL_1,
				REFERENCE_AVcc_AVss,
				REFVOLT_LEVEL_NONE,
				SHT_SOURCE_ACLK,
				SHT_CLOCK_DIV_1,
				SAMPLE_HOLD_4_CYCLES,
				SAMPCON_SOURCE_SMCLK,
				SAMPCON_CLOCK_DIV_1);

SensADC _adcy =  SensADC(
				ADC_CHANNEL_2,
				REFERENCE_AVcc_AVss,
				REFVOLT_LEVEL_NONE,
				SHT_SOURCE_ACLK,
				SHT_CLOCK_DIV_1,
				SAMPLE_HOLD_4_CYCLES,
				SAMPCON_SOURCE_SMCLK,
				SAMPCON_CLOCK_DIV_1);

SensADC _adcz =  SensADC(
				ADC_CHANNEL_3,
				REFERENCE_AVcc_AVss,
				REFVOLT_LEVEL_NONE,
				SHT_SOURCE_ACLK,
				SHT_CLOCK_DIV_1,
				SAMPLE_HOLD_4_CYCLES,
				SAMPCON_SOURCE_SMCLK,
				SAMPCON_CLOCK_DIV_1);

SensADXL377 adxl = SensADXL377(_adcx, _adcy, _adcz);

void onPeriodicTimerDone(void);
void onStartDone(error_t result);
void onStopDone(error_t result);
void onReadDone(sensor_data_t* data, error_t result);

void setup(void) {
	Debug.begin();

	Debug.println("ADXL377_12 example - Multiple Channel Single Conversion");

	pinMode(LED_BUILDIN, OUTPUT);
	digitalWrite(LED_BUILDIN, LOW);

	// 1. Attach timer callback.
	timer.attachCallback(onPeriodicTimerDone);
	// 2. Start periodic timer to 1 sec.
	timer.startPeriodic(1000);

	// 1. Attach sensor callbacks.
	adxl.attachStartDone(onStartDone);
	adxl.attachStopDone(onStopDone);
	adxl.attachReadDone(onReadDone);

	// 2. Start sensor.
	Debug.print("Starting sensor ... ");
	adxl.start();
}

void loop(void) {

}

void onPeriodicTimerDone() {
	adxl.read();
}



void onStartDone(error_t result) {
	if (result == SUCCESS) {
		Debug.println("done");

		// 3. Read sensor.
		Debug.println();
		Debug.print("Reading sensor ... ");
		adxl.readNow();
	}
	else {
		Debug.println("error1");
	}
}

void onReadDone(sensor_data_t* sdata, error_t result) {
    adxl377_data_t* data = (adxl377_data_t*) sdata;
    if (result == SUCCESS) {
        Debug.println("done");
        Debug.print("ax: ").println(data->_chanx);
        Debug.print("ay: ").println(data->_chany);
        Debug.print("az: ").println(data->_chanz);
    	digitalToggle(LED_BUILDIN);
        delete(data);
        delete(sdata);
		delay(1000);

		Debug.println();
		Debug.print("Reading sensor ... ");
		adxl.readNow();
	}
	else {
		Debug.println("error2");
	}
}

void onStopDone(error_t result) {
	if (result == SUCCESS) {
		Debug.println("done");
	}
	else {
		Debug.println("error3");
	}
}
