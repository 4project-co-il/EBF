#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_AnalogInput sensor;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication
EBF_Serial serial(Serial);

// these constants won't change. They are the lowest and highest readings you
// get from your sensor:
const int sensorMin = 0;   // sensor minimum, discovered through experiment
const int sensorMax = 600; // sensor maximum, discovered through experiment

// Sensor change callback
void onSensorChange()
{
	// read the sensor
	// We use GetRawValue to have it compatible with the map code in original example
	// GetValue function returns float in percentage between 0% and 100%
	uint16_t sensorReading = sensor.GetRawValue();

	// map the sensor range to a range of four options:
	uint8_t range = map(sensorReading, sensorMin, sensorMax, 0, 3);

	// do something different depending on the range value:
	switch (range) {
	case 0: // your hand is on the sensor
		serial.println("dark");
		break;
	case 1: // your hand is close to the sensor
		serial.println("dim");
		break;
	case 2: // your hand is a few inches from the sensor
		serial.println("medium");
		break;
	case 3: // your hand is nowhere near the sensor
		serial.println("bright");
		break;
	}
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize serial, default speed is 115200
	serial.Init();

	// Initialize analog input on line A0, function will be called on every 1% change
	sensor.Init(A0, onSensorChange, 1);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
