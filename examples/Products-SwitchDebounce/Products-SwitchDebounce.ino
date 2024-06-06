#include <Arduino.h>
#include "EBF_Products.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Switch sw;	// Name "switch" can't be used since it's a reserved keyword in C/C++ language
EBF_Led led;

// EBF_Switch implements digital input with software debounce to filter out noise in transition state.
// During that state the line jumps ON and OFF many times.
// Default debounce time is 10 mSec. You can change that time with the SetDebounceTime() function

// Switch change callback
void onSwitchChange()
{
	// Show the state visually
	led.SetValue(sw.GetState());

	// And print it via the serial connection
	Serial.print("Switch state: ");
	Serial.println(sw.GetState());
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Serial connection will be used to print the switch state
	Serial.begin(115200);

	// We will use the built-in LED as an indication for the switch state
	led.Init(LED_BUILTIN);

	// Digital line 2 will be used.
	// Second parameter can specify if internal pull-up resistor should be used
	sw.Init(2);

	// Register the callback on any change of the switch
	sw.SetOnChange(onSwitchChange);

	// Debounce time can be changed with the SetDebounceTime() function
	sw.SetDebounceTime(20);

	// You can also change the polling interval if you don't need maximal responsivnes
	// EBF_Switch implementation temporary changes the polling time when debounce should
	// be measured. For this example, the polling is set higher than the debounce time,
	// so during the change of the input the polling will temporary be changed to 20 mSec
	sw.SetPollInterval(100);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
