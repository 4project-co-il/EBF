#include <Arduino.h>
#include "EBF_Products.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Timer relayTimer;
EBF_Relay relay;

// EBF_Relay are digital objects that can be turned ON and OFF
// The name of the class doesn't limit them to relays only
// Those can be used for simple LED control, MOSFET transistors and etc...

uint8_t relayState = 0;

// Relay Timer callback function
void onTimer()
{
	// Change the state every time the timer expires
	relayState = !relayState;

	// On() and Off() functions are used in that example, but SetValue() can be used as well in that way:
	// relay.SetValue(relayState);

	if (relayState) {
		// Turn the relay ON
		relay.On();
	} else {
		// Turn the relay OFF
		relay.Off();
	}

	// EBF timers are one-shot in nature, restart it
	relayTimer.Start();
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize the timer for 1sec (1000 mSec), onTimer function will be called
	relayTimer.Init(onTimer, 1000);

	// Start the timer for the first time
	relayTimer.Start();

	// Digital line 7 will be used
	relay.Init(7);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
