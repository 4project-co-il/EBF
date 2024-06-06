#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_AnalogInput a0;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication
EBF_Serial serial(Serial);

// Analog input change
void onAnalog0Change()
{
	// We're using getRawValue, so we could use it's output for the map function
	uint16_t sensorReading = a0.GetRawValue();

	// print the sensor reading so you know its range
	Serial.println(sensorReading);

	// map the analog input range (in this case, 400 - 1000 from the photoresistor)
	// to the output pitch range (120 - 1500Hz)
	// change the minimum and maximum input numbers below depending on the range
	// your sensor's giving:
	int thisPitch = map(sensorReading, 400, 1000, 120, 1500);

	// play the pitch:
	tone(9, thisPitch, 10);
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Default Init is enough for printouts via Serial on 115200 boud speed
	serial.Init();

	// Initialize the analog input on line 0, for any change (0% threshold)
	a0.Init(0, onAnalog0Change, A0);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
