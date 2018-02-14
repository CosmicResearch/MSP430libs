#include "Senscape.h"
#include "GPS.h"

const uint32_t baudrate = 9600;

GPS gps(&Serial0, baudrate);

void startDone(error_t error) {
    if (error == SUCCESS) {
        Debug.println("start done");
    }
    else {
        Debug.println("error on start");
    }
}

void stopDone(error_t error) {

}

void readDone(sensor_data_t* data, error_t error) {
    if (error != SUCCESS) {
        return;
    }
    gps_data_t gps_data = *((gps_data_t*)data);
    Debug.println("---------------------------------------");
    Debug.println("new gps data");
    Debug.print("Latitude: ").print(gps_data.latitude/1000000.0f).println(gps_data.latitudeChar);
    Debug.print("Longitude: ").print(gps_data.longitude/1000000.0f).println(gps_data.longitudeChar);
    Debug.print("Altitude: ").print(gps_data.altitude).println(" m");
    Debug.print("Fix: ").println(gps_data.fix);
    Debug.print("Time: ").print(gps_data.hour).print(":").print(gps_data.minute).print(":").println(gps_data.seconds);
    Debug.print("Type: ").println(gps_data.type);
}

void setup() {
    Debug.begin();
    gps.attachStartDone(startDone);
    gps.attachReadDone(readDone);
    gps.attachStopDone(stopDone);

    gps.start();
}

void loop() {

}
