#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_AnalogInput sensor;
EBF_DigitalOutput led;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication
EBF_Serial serial(Serial);

// threshold value to decide when the detected sound is a knock or not
const int threshold = 100;

// these variables will change:
int sensorReading = 0;  // variable to store the value read from the sensor pin
int ledState = LOW;     // variable used to store the last LED status, to toggle the light

// Analog input change callback
void onSensorChange()
{
	sensorReading = sensor.GetRawValue();

	if (sensorReading >= threshold) {
		// toggle the status of the led
		ledState = !ledState;
		// update the LED pin itself
		led.SetValue(ledState);
		// send the string "Knock!" back to the computer, followed by newline
		serial.println("Knock!");
	}
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize knock sensor on A0, for 1% change
	sensor.Init(A0, onSensorChange, 1);
	// Will poll the sensor every 100 mSec
	sensor.SetPollInterval(100);

	// Initialize built-in led (generally on line 13)
	led.Init(LED_BUILTIN);

	// Initialize serial, default speed is 115200
	serial.Init();
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
