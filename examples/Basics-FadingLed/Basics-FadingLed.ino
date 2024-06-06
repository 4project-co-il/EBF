#include <Arduino.h>
#include "EBF.h"

enum {
	FADING_TIMER = 0,

	NUMBER_OF_TIMERS
};

// EBF objects creation, should be global
EBF_Core EBF;
EBF_PwmOutput pwmLed;

int brightness = 0; // how bright the LED is
int fadeAmount = 5;	// how many points to fade the LED by

void onFadingTimer()
{
	// Set the brightness of the LED
	pwmLed.SetValue(brightness);

	// change the brightness for next time through the loop
	brightness = brightness + fadeAmount;

	// reverse the direction of the fading at the ends of the fade
	if (brightness <= 0 || brightness >= 255) {
		fadeAmount = -fadeAmount;
	}

	// Restart the fading timer
	EBF.StartTimer(FADING_TIMER);
}

void setup()
{
	// EBF is the first thing that should be initialized, with the maximum timers to be used
	EBF.Init(NUMBER_OF_TIMERS);

	// Initialize fading timer for 30 mSec
	EBF.InitTimer(FADING_TIMER, onFadingTimer, 30);
	EBF.StartTimer(FADING_TIMER);

	// Initialize LED connected to PWM output on line 9
	pwmLed.Init(9);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
