/*
 * @author David Williams <davidwcorral@hotmail.com>
 * @date   July 25, 2017
 */


#include "Senscape.h"
#include "SensMAG.h"
#include "SensorFusion.h"
#include "math.h"

Resource mag_resource = Resource(ARBITER_USCIB_0);

uint8_t a = 3;
SensFusion SF;
SensMAG MAG(&Spi0, &mag_resource, M_SCALE_2GS, M_ODR_100);

void onReadDone(sensor_data_t* data, error_t result);
void onCalibrationDone(error_t result);
void onStartDone(error_t result);
void magcalMPU9250();

boolean_t read;
sensfusion_data_t data;
lsm9ds0_data_t *dataMag;
void setup(void) {
	Debug.begin();

	// TOSO F("...")
	Debug.println("MAG_03 example - Calibration");
	Debug.println();

	data.heading = 0; data.pitch = 0; data.roll = 0;

	dataMag = new lsm9ds0_data_t;
	MAG.attachReadDone(onReadDone);
	MAG.attachStartDone(onStartDone);
	MAG.attachCalibrationDone(onCalibrationDone);

	MAG.start();
}

void loop(void) {
}

void onStartDone(error_t result){
	if (result == SUCCESS) {
		Debug.println("Start Done");
		MAG.read();
	}
	else {
		Debug.println("error");
	}
}

void onCalibrationDone(error_t result){
    if (result == SUCCESS) {
        Debug.println("Calibration Done");
    }
    else {
        Debug.println("error");
    }
}


void onReadDone(sensor_data_t* data_t, error_t result) {
    dataMag = (lsm9ds0_data_t*) data_t;
	//MAG.getMagnetism(&xhi, &yhi, &zhi);
    if (result == SUCCESS) {
        SF.magGetOrientation(SENSOR_AXIS_Z, dataMag, &data);
        //float_t a = atan(2);
        Debug.print("x: ").print(dataMag->x).println(" mGauss");
        Debug.print("y: ").print(dataMag->y).println(" mGauss");
        Debug.print("heading: ").println(data.heading);
        delay(1000);
        MAG.read();
	}
	else {
		Debug.println("error");
	}
}

void magcalMPU9250()
{
    uint16_t ii = 0, sample_count = 0;
    int16_t mag_bias[3] = {0, 0, 0}, mag_scale[3] = {0, 0, 0};
    int16_t mag_max[3] = {-32767, -32767, -32767}, mag_min[3] = {32767, 32767, 32767}, mag_temp[3] = {0, 0, 0};

    Debug.println("Mag Calibration: Wave device in a figure eight until done!");
    delay(4000);
    read = 0;// shoot for ~fifteen seconds of mag data
    //if(MPU9250Mmode == 0x02) sample_count = 128;  // at 8 Hz ODR, new mag data is available every 125 ms
    //if(MPU9250Mmode == 0x06) sample_count = 1500;  // at 100 Hz ODR, new mag data is available every 10 ms
    for(ii = 0; ii < sample_count; ii++) {
       MAG.read();
       while(!read){};
       if (dataMag->x > mag_max[0]) mag_max[0] = dataMag->x;
       if(dataMag->x < mag_min[0]) mag_min[0] = dataMag->x;

       if (dataMag->y > mag_max[1]) mag_max[1] = dataMag->y;
       if(dataMag->y < mag_min[1]) mag_min[1] = dataMag->y;

       if (dataMag->z > mag_max[2]) mag_max[2] = dataMag->z;
       if(dataMag->z < mag_min[2]) mag_min[2] = dataMag->z;
       read = 0;
        //if(MPU9250Mmode == 0x02) delay(135);  // at 8 Hz ODR, new mag data is available every 125 ms
        //if(MPU9250Mmode == 0x06) delay(12);  // at 100 Hz ODR, new mag data is available every 10 ms
        wait(12);
    }


    // Get hard iron correction
    mag_bias[0]  = (mag_max[0] + mag_min[0])/2;  // get average x mag bias in counts
    mag_bias[1]  = (mag_max[1] + mag_min[1])/2;  // get average y mag bias in counts
    mag_bias[2]  = (mag_max[2] + mag_min[2])/2;  // get average z mag bias in counts

    float_t MPU9250mRes = MAG.calcmRes();
    float_t dest1[3];
    dest1[0] = (float_t) mag_bias[0]*MPU9250mRes;  // save mag biases in G for main program
    dest1[1] = (float_t) mag_bias[1]*MPU9250mRes;
    dest1[2] = (float_t) mag_bias[2]*MPU9250mRes;

    // Get soft iron correction estimate
    mag_scale[0]  = (mag_max[0] - mag_min[0])/2;  // get average x axis max chord length in counts
    mag_scale[1]  = (mag_max[1] - mag_min[1])/2;  // get average y axis max chord length in counts
    mag_scale[2]  = (mag_max[2] - mag_min[2])/2;  // get average z axis max chord length in counts

    float avg_rad = mag_scale[0] + mag_scale[1] + mag_scale[2];
    avg_rad /= 3.0;
    int16_t dest2[3];
    dest2[0] = avg_rad/((float_t)mag_scale[0]);
    dest2[1] = avg_rad/((float_t)mag_scale[1]);
    dest2[2] = avg_rad/((float_t)mag_scale[2]);
    //MAG.calibrate(&dest2[0], &dest2[1], &dest2[2]);


    Debug.println("Mag Calibration done!");
}
