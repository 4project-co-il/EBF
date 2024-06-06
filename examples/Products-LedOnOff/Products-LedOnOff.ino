#include <Arduino.h>
#include "EBF_Products.h"

// Timers enumeration
enum {
	LED_TIMER = 0,

	NUMBER_OF_TIMERS
};

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Led led;

uint8_t ledState = 0;

// LED Timer callback function
void onTimer()
{
	// Change the state every time the timer expires
	ledState = !ledState;

	// On() and Off() functions are used in that example, but SetValue() can be used as well in that way:
	// led.SetValue(ledState);

	if (ledState) {
		// Turn the LED ON
		led.On();
	} else {
		// Turn the LED OFF
		led.Off();
	}

	// EBF timers are one-shot in nature, restart it
	EBF.StartTimer(LED_TIMER);
}

void setup()
{
	// EBF is the first thing that should be initialized, with the maximum timers to be used
	EBF.Init(NUMBER_OF_TIMERS);

	// Initialize the timer for 1sec (1000 mSec), onTimer function will be called
	EBF.InitTimer(LED_TIMER, onTimer, 1000);

	// Start the timer for the first time
	EBF.StartTimer(LED_TIMER);

	// Built-in LED on digital output #13 will be used
	led.Init(LED_BUILTIN);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
