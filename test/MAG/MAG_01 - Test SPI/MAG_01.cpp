/*
 * @author David Williams <davidwcorral@hotmail.com>
 * @date   July 25, 2017
 */


#include "Senscape.h"
#include "SensMAG.h"

Resource mag_resource = Resource(ARBITER_USCIB_0);

SensMAG MAG(&Spi0, &mag_resource, M_SCALE_2GS, M_ODR_125);

void onRequestAccelMagId(uint8_t *id, error_t);
void onStartDone(error_t result);

uint8_t id_mag;

void setup(void) {
	Debug.begin();

	// TOSO F("...")
	Debug.println("MAG_01 example - Test SPI");
	Debug.println();

	MAG.attachRequestAccelMagIdDone(onRequestAccelMagId);
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
		Debug.print("Requesting MAD ID ... ");
		MAG.requestAccelMagId();
	}
	else {
		Debug.println("error");
	}
}

void onRequestAccelMagId(uint8_t *id_mag, error_t result){
	if (result == SUCCESS) {
			Debug.print("ACCL/MAGNET CHIP ID = 0x");
			Debug.println(id_mag, HEX);
		}
	else {
			Debug.print("ERROR");
	}
}
