#include <Arduino.h>
#include "EBF_Products.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Timer ledTimer;
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
	ledTimer.Start();
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize the timer for 1sec (1000 mSec), onTimer function will be called
	ledTimer.Init(onTimer, 1000);

	// Start the timer for the first time
	ledTimer.Start();

	// Built-in LED on digital output #13 will be used
	led.Init(LED_BUILTIN);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
