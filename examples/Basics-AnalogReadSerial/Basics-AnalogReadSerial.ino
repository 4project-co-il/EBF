#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Serial serial;
EBF_AnalogInput analog0;

void onAnalog0Change()
{
	// Analog values are in percents from 0 to 100
	// For 5V boards it will be from 0V to 5V
	serial.print("Analog 0 value: ");
	serial.println(analog0.GetValue());
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Serial initializtion
	serial.Init();

	// Analog input initialization on A0 line, on 5% change onAnalog0Change function will be called
	analog0.Init(A0, onAnalog0Change, 5);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
