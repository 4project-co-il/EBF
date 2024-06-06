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

	if (ledState) {
		// The fading will be done during 900 mSec with 50 steps
		led.FadeIn(900, 50);
	} else {
		led.FadeOut(900, 50);
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

	// LED on PWM output #9 will be used
	led.Init(9);

	// You can fade-in (from OFF to ON) and fade-out (from ON to OFF) the LED
	// with FadeIn() and FadeOut() functions.
	// No timers are needed, the effect is implemented into the EBF_Led object.
	// We will use the timer in that example to toggle between the in and out fading
	// to create a "breathing" LED effect.

	// led.SetBrighness() can be used for that effect to set the maximal brighness the LED will reach

	// That functionality will be visible on PWM enabled outputs (with "~" mark on the board)
	// On non-PWM lines the led will be OFF for brighness < 50% and ON for brightness >= 50%
	// This is in fact the default behavior for Arduino's analogWrite() function for values < 128 and >= 128
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
