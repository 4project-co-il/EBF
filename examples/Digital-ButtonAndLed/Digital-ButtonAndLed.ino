#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_DigitalInput button;
EBF_DigitalOutput led;

// Button change callback function
void onButtonChange()
{
	// Read the button state
	uint8_t buttonState = button.GetValue();

	// Set the LED based on the button state
	led.SetValue(buttonState);
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize digital input button on line 2, onButtonChange function will be called on change
	button.Init(2, onButtonChange);

	// Initialize built-in LED (generally line 13)
	led.Init(LED_BUILTIN);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
