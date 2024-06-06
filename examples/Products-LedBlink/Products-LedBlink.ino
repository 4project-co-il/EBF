#include <Arduino.h>
#include "EBF_Products.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Led led;

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Built-in LED on digital output #13 will be used
	led.Init(LED_BUILTIN);

	// No timers needed to blink the LED, just call he Blink() function!
	// The LED will be ON for 100mSec and OFF for 900mSec
	led.Blink(100, 900);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
