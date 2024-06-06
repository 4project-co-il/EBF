#include <Arduino.h>
#include "EBF.h"

enum {
	DEBOUNCE_TIMER = 0,

	NUMBER_OF_TIMERS
};

// EBF objects creation, should be global
EBF_Core EBF;
EBF_DigitalInput button;
EBF_DigitalOutput led;

// Variables will change:
uint8_t buttonState = LOW;		// the current reading from the input pin
uint8_t lastButtonState = LOW;	// the previous reading from the input pin

// Button change callback function
void onButtonChange()
{
	// read the state of the switch into a local variable:
	lastButtonState = button.GetValue();

	// Since the digital input callback is called only on change
	// we can just re-start the timer
	EBF.StopTimer(DEBOUNCE_TIMER);
	EBF.StartTimer(DEBOUNCE_TIMER);

	// When the timer will arrive, we'll be in the situation where the lastButtonState
	// lasted for the debounce time and didn't change, otherwise we would re-start the timer
}

// Debounce timer
void onDebounceTimer()
{
	// The timer will arrive after the timout passed without a change in the button state
	// lastButtonState is the state we should use now, as it's stable for the debounce time
	buttonState = lastButtonState;

	led.SetValue(buttonState);
}

void setup()
{
	// EBF is the first thing that should be initialized, with the maximum timers to be used
	EBF.Init(NUMBER_OF_TIMERS);

	// Initialize debounce timer for 50 mSec, it will be started and stopped from onButtonChange function
	EBF.InitTimer(DEBOUNCE_TIMER, onDebounceTimer, 50);

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
