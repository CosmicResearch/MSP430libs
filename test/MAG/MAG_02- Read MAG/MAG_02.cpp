/*
 * @author David Williams <davidwcorral@hotmail.com>
 * @date   July 25, 2017
 */


#include "Senscape.h"
#include "SensMAG.h"

Resource mag_resource = Resource(ARBITER_USCIB_0);

SensMAG MAG(&Spi0, &mag_resource, M_SCALE_2GS, M_ODR_125);

void onReadDone(sensor_data_t* data, error_t result);
void onStartDone(error_t result);


void setup(void) {
	Debug.begin();

	// TOSO F("...")
	Debug.println("MAG_02 example - Read MAG");
	Debug.println();

	MAG.attachReadDone(onReadDone);
	MAG.attachStartDone(onStartDone);

	MAG.start();
}

void loop(void) {
}

void onStartDone(error_t result){
	if (result == SUCCESS) {
		Debug.println("done");

		// 3. Read sensor.
		Debug.println();
		Debug.print("Reading Magnetometer ... ");
		MAG.read();
	}
	else {
		Debug.println("error");
	}
}

void onReadDone(sensor_data_t* data, error_t result) {
	int16_t xhi, yhi, zhi;
	MAG.getMagnetism(&xhi, &yhi, &zhi);
    if (result == SUCCESS) {
        Debug.println("done");
        Debug.print("x: ").println(xhi);
        Debug.print("y: ").println(yhi);
        Debug.print("z: ").println(zhi);

		delay(1000);
		MAG.read();
	}
	else {
		Debug.println("error");
	}
}
