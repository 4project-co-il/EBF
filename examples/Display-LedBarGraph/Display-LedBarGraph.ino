#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_AnalogInput analogInput;

// the number of LEDs in the bar graph
const int ledCount = 10;

// an array of pin numbers to which LEDs are attached
int ledPins[] = {
	2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

// Array of digital outputs
EBF_DigitalOutput leds[ledCount];

// Analog input change callback
void onAnalogChange()
{
	// read the potentiometer:
	int sensorReading = analogInput.GetRawValue();
	// map the result to a range from 0 to the number of LEDs:
	int ledLevel = map(sensorReading, 0, 1023, 0, ledCount);

	// loop over the LED array:
	for (int thisLed = 0; thisLed < ledCount; thisLed++) {
		// if the array element's index is less than ledLevel,
		// turn the pin for this element on
		if (thisLed < ledLevel) {
			leds[thisLed].SetValue(HIGH);
		}
		// turn off all pins higher than the ledLevel
		else {
			leds[thisLed].SetValue(LOW);
		}
	}
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize knock sensor on A0, for 1% change
	analogInput.Init(A0, onAnalogChange, 1);
	// Will poll the input every 10 mSec
	analogInput.SetPollingInterval(10);

	// loop over the leds array and set them all to output:
	for (int thisLed = 0; thisLed < ledCount; thisLed++) {
		leds[thisLed].Init(ledPins[thisLed]);
	}
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
