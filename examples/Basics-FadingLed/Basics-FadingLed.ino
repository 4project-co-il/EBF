#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Timer fadingTimer;
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
	fadingTimer.Start();
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize fading timer for 30 mSec
	fadingTimer.Init(onFadingTimer, 30);
	fadingTimer.Start();

	// Initialize built-in LED. Should be changed if your development board internal led is not PWM-capable
	pwmLed.Init(LED_BUILTIN);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
