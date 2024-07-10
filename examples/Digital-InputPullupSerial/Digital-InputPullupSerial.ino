#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_DigitalInput button;
EBF_DigitalOutput led;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication by default
EBF_Serial serial;
// You can use any other Serial interface if availalbe for your device
//EBF_Serial serial(Serial2);

// Button change callback function
void onButtonChange()
{
	// Read the button state
	uint8_t buttonState = button.GetValue();

	// Print out the state of the button
	serial.println(buttonState);

	// Set the state of the LED
	led.SetValue(buttonState);
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Default Init is enough for printouts via Serial on 115200 boud speed
	serial.Init();

	// Initialize digital input button on line 2, onButtonChange function will be called
	// on every change (MODE_CHANGE), use internal pull-up resistor (true as the last parameter)
	button.Init(2, onButtonChange, EBF_DigitalInput::InterruptMode::MODE_CHANGE, true);

	// Initialize built-in LED (generallay on line 13)
	led.Init(LED_BUILTIN);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
