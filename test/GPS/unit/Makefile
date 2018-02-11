all: GGAtest RMCtest

GGAtest:
	g++ -o GGATest.exe GGATest.cpp ../../cores/Senscape.cpp ../../cores/Serial.h ../../cores/SensorClient.h ../../../GPS/GPS.cpp -fpermissive -I ../../cores/ -I ../../../GPS/ -D__TEST__

RMCtest:
	g++ -o RMCTest.exe RMCTest.cpp ../../cores/Senscape.cpp ../../cores/SensorClient.h ../../../GPS/GPS.cpp -fpermissive -I ../../cores/ -I ../../../GPS/ -D__TEST__
