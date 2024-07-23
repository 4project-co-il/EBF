#include <Arduino.h>
#include "EBF.h"

// Timers enumeration
enum {
	SEC_TIMER = 0,

	NUMBER_OF_TIMERS
};

// EBF objects creation, should be global
EBF_Core EBF;
EBF_DigitalOutput led;
EBF_DigitalInput button;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication by default
EBF_Serial serial;
// You can use any other Serial interface if availalbe for your device
//EBF_Serial serial(Serial2);

// LED Timer callback function
void onTimer()
{
	// We will use the 1sec timer as a timing reference in printouts
	serial.println("1sec passed");

	// EBF timers are one-shot in nature, restart it
	EBF.StartTimer(SEC_TIMER);
}

// Set LED state based on the button state
void onButtonChange()
{
	uint8_t buttonState;

	// We use GetLastValue instead of the GetValue, to use the value that triggered
	// the callback. The digital input might change again since then
	buttonState = button.GetLastValue();

	// Use the LED and serial printouts as a visualization when the change was detected
	led.SetValue(buttonState);
	serial.print("Button changed to: ");
	serial.println(buttonState);
}

void setup()
{
	// EBF is the first thing that should be initialized, with the maximum timers to be used
	EBF.Init(NUMBER_OF_TIMERS);

	// Default Init is enough for printouts via Serial on 115200 boud speed
	serial.Init();

	// Initialize the timer for 1sec (1000 mSec), onTimer function will be called
	EBF.InitTimer(SEC_TIMER, onTimer, 1000);

	// Start the LED timer for the first time
	EBF.StartTimer(SEC_TIMER);

	// Built-in LED on digital output #13 will be used
	led.Init(13);

	// Initialize button on digital input 2
	// Internal pullup resistor will be used to minimize bouncing
	button.Init(2, onButtonChange, EBF_DigitalInput::InterruptMode::MODE_CHANGE, true);

	// Polling time is intentially set to high value (500 mSec).
	// By playing with the button you will see the delay in the response time
	// since EBF will do nothing in non-needed times (between the pollings and timers).
	// The "nothing" could be a delay or power save when possible.
	//
	// You could decrease the polling interval to it's minimal, but that will not allow
	// to save power, if it was possible.
	// The solution is to use interrupts (if possible). See "Digital-Interrupts" example.
	button.SetPollInterval(500);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
