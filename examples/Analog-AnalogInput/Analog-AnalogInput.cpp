#include <Arduino.h>
#include "EBF.h"

enum {
	DELAY_TIMER = 0,

	NUMBER_OF_TIMERS
};

// EBF objects creation, should be global
EBF_Core EBF;
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
	EBF.StartTimer(DELAY_TIMER, sensorValue);
}

void setup()
{
	// EBF is the first thing that should be initialized, with the maximum timers to be used
	EBF.Init(NUMBER_OF_TIMERS);

	// Initialize and start the timer for 1 mSec as it's first run
	EBF.InitTimer(DELAY_TIMER, onDelayTimeout, 1);
	EBF.StartTimer(DELAY_TIMER);

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
