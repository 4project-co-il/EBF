#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Serial serial;
EBF_AnalogInput analog0;

void onAnalog0Change()
{
	// Get analog value as percentage from 0 to 100%
	float sensorValue = analog0.GetValue();

	// Convert to voltage value for 5V system (0 to 5V range)
	float voltage = 5.0 * sensorValue / 100.0;

	// Print out the value
	serial.println(voltage);
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Serial initializtion
	serial.Init();

	// Analog input initialization on A0 line, on 1% change onAnalog0Change function will be called
	analog0.Init(A0, onAnalog0Change, 1);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
