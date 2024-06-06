#include <Arduino.h>
#include "EBF_Products.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Led led;

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// LED on PWM output #9 will be used
	led.Init(9);

	// You can set the maximal brightness for the LED
	// That functionality will be visible on PWM enabled outputs (with "~" mark on the board)
	// On non-PWM lines the led will be OFF for brighness < 50% and ON for brightness >= 50%
	// This is in fact the default behavior for Arduino's analogWrite() function for values < 128 and >= 128
	// The brightness should be specified in percents (0 - 100%)
	led.SetBrightness(70);

	// No timers needed to blink the LED, just call he Blink() function and everything is done automagically
	// The LED will be ON for 100mSec and OFF for 900mSec
	led.Blink(100, 900);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
