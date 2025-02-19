#include <Arduino.h>
#include "EBF.h"

// Timers enumeration
enum {
	LED_TIMER = 0,

	NUMBER_OF_TIMERS
};

// EBF objects creation, should be global
EBF_Core EBF;
EBF_DigitalOutput led;

bool ledState = LOW;

// LED Timer callback function
void onTimer()
{
	// Set the LED digital output to the new state
	ledState = !ledState;
	led.SetValue(ledState);

	// EBF timers are one-shot in nature, restart it
	EBF.StartTimer(LED_TIMER);
}

void setup()
{
	// EBF is the first thing that should be initialized, with the maximum timers to be used
	EBF.Init(NUMBER_OF_TIMERS);

	// Initialize the timer for 1sec (1000 mSec), onTimer function will be called
	EBF.InitTimer(LED_TIMER, onTimer, 1000);

	// Start the LED timer for the first time
	EBF.StartTimer(LED_TIMER);

	// Initialize built-in LED (generally on line 13)
	led.Init(LED_BUILTIN);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
