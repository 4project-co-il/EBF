#include <Arduino.h>
#include "EBF_Products.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Button button;
EBF_Led led;

// EBF_Button implements digital input with software debounce to filter out noise in transition state.
// And have callbacks for Pressed, Released and LongPress events, As well as Changed event similary to EBF_Button class.
// Default debounce time is 10 mSec. You can change that time with the SetDebounceTime() function
// Default long press time is 3 seconds. You can change that time with the SetLongPressTime() function

// Button change callback
void onButtonChange()
{
	// And print it via the serial connection
	Serial.print("Button state: ");
	Serial.println(button.GetState());
}

// On pressed callback
void onPressed()
{
	Serial.println("Pressed");

	led.On();
}

// On released callback
void onReleased()
{
	Serial.println("Released");

	led.Off();
}

// On long-press callback
void onLongPress()
{
	Serial.println("Long press");

	led.Blink(50, 50);
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
	button.Init(2);

	// Register the callbacks. You don't have to register them all. Just the ones you need.
	button.SetOnChange(onButtonChange);
	button.SetOnPress(onPressed);
	button.SetOnRelease(onReleased);
	button.SetOnLongPress(onLongPress);

	// Debounce time can be changed with the SetDebounceTime() function
	button.SetDebounceTime(20);

	// You can also change the polling interval if you don't need maximal responsivnes
	// EBF_Button implementation temporary changes the polling time when debounce should
	// be measured.
	button.SetPollInterval(5);

	// You can change the long-press time
	button.SetLongPressTime(5000);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
