#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Timer ledTimer;
EBF_DigitalOutput led;

bool ledState = LOW;

// LED Timer callback function
void onTimer()
{
	// Set the LED digital output to the new state
	ledState = !ledState;
	led.SetValue(ledState);

	// EBF timers are one-shot in nature, restart it for specified time based on the ledState
	if (ledState == LOW) {
		// Long delay in OFF state
		ledTimer.Start(900);
	} else {
		// Short blink in ON state
		ledTimer.Start(100);
	}
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize the timer for 1sec (1000 mSec), onTimer function will be called
	ledTimer.Init(onTimer, 1000);

	// Start the LED timer for the first time
	ledTimer.Start();

	// Initialize built-in LED (generally on line 13)
	led.Init(LED_BUILTIN);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
