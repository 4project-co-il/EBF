#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Timer delayTimer;
EBF_AnalogInput analongInput;
EBF_DigitalOutput led;

uint16_t sensorValue = 0;        // value read from the pot
uint8_t ledState = LOW;

// Delay timer callback
void onDelayTimeout()
{
	// For LOW state, we will read the analog sensor value and use it as a delay
	// for both HIGH and LOW states of the LED
	if (ledState == LOW) {
		// Read the value from the sensor
		// GetRawValue returns the same value as analogRead without conversion
		sensorValue = analongInput.GetRawValue();

		// Set the LED to HIGH state
		ledState = HIGH;
	} else {
		// Set the LED to LOW state
		ledState = LOW;
	}

	// Set the LED to it's initial state
	led.SetValue(ledState);

	// Re-start the timer based on the sensor value
	delayTimer.Start(sensorValue);
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize and start the timer for 1 mSec as it's first run
	delayTimer.Init(onDelayTimeout, 1);
	delayTimer.Start();

	// Initialize the analog input on line 0, no change callback is needed in that case
	analongInput.Init(A0, NULL);

	// Initialize built-in LED (generally on line 13)
	led.Init(LED_BUILTIN);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
