#include "Senscape.h"
#include "SensLSM9DS0Gyro.h"

#define CHIP_CS		(3)
#define CHIP_ID 	(0x0F)

#define MAX_READINGS	10

Resource gyro_resource = Resource(ARBITER_USCIB_0);

SensLSM9DS0Gyro GYRO(&Spi0, &gyro_resource,
		LSM9DS0_GYROSCALE_245DPS, G_ODR_95_BW_125);

void onStartDone(error_t result);
void onStopDone(error_t result);
void onReadDone(sensor_data_t* data, error_t result);

uint16_t readings = MAX_READINGS;

void setup(void) {
		Debug.begin();

		pinMode(LED_BUILDIN, OUTPUT);
		//Debug.off();

		Debug.println("Gyro_01 example - Read data (continuous mode)");
		Debug.println();

		// 1. Attach sensor callbacks.
		GYRO.attachStartDone(onStartDone);
		GYRO.attachStopDone(onStopDone);
		GYRO.attachReadDone(onReadDone);

		// 2. Start sensor.
		Debug.print("Starting sensor ... ");
		GYRO.start();
}

void loop(void) {
		//digitalWrite(LED_BUILDIN, HIGH);
		//delay(1000);
		//digitalWrite(LED_BUILDIN, LOW);
		//delay(1000);
}

void onStartDone(error_t error) {
    if (error == SUCCESS) {
        Debug.println("Start done");

        Debug.println();
        Debug.println("Reading sensor...");
        GYRO.read();
    }
    else {
        Debug.println("Error on start");
    }
}

void onStopDone(error_t error) {
	if (error == SUCCESS) {
		Debug.println("Stop done");
	} else {
		Debug.println("Error on stop");
	}
}

void onReadDone(sensor_data_t* data_t, error_t error) {
	int16_t x, y, z;
	lsm9ds0gyro_data_t* data = (lsm9ds0gyro_data_t*) data_t;
	x = data->x;
	y = data->y;
	z = data->z;

	if (error == SUCCESS) {
		Debug.println("---------------------------------------");
		Debug.println("new gyro data");
		Debug.print("X axis: ").println(x);
		Debug.print("Y axis: ").println(y);
		Debug.print("Z axis: ").println(z);

		delay(1000);
		GYRO.read();
	} else {
		Debug.println("error");
	}
}


