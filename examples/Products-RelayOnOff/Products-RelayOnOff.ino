#include <Arduino.h>
#include "EBF_Products.h"

// Timers enumeration
enum {
	RELAY_TIMER = 0,

	NUMBER_OF_TIMERS
};

// EBF objects creation, should be global
EBF_Core EBF;
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
	EBF.StartTimer(RELAY_TIMER);
}

void setup()
{
	// EBF is the first thing that should be initialized, with the maximum timers to be used
	EBF.Init(NUMBER_OF_TIMERS);

	// Initialize the timer for 1sec (1000 mSec), onTimer function will be called
	EBF.InitTimer(RELAY_TIMER, onTimer, 1000);

	// Start the timer for the first time
	EBF.StartTimer(RELAY_TIMER);

	// Digital line 7 will be used
	relay.Init(7);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
