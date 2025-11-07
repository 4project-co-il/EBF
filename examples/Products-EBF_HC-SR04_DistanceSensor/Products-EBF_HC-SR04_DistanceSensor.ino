#include <Arduino.h>
#include "EBF_Products.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_HCSR04_DistanceSensor sensor;

// Distance change callback function
void onDistanceChange()
{
	Serial.println(sensor.GetValue());
}

void setup()
{
	Serial.begin(115200);
	Serial.println("Starting...");

	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize HC-SR04 distance sensor on lines 9 (trigger output) and 10 (echo input)
	sensor.Init(9, 10);
	// the onDistanceChange function will be called when the distance will be change more than 1%
	sensor.SetOnChange(onDistanceChange, 1);
	// The sensor will be polled every 100 mSec
	sensor.SetPollingInterval(100);
}


void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
