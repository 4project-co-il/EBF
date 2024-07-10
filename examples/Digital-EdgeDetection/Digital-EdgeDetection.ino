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

uint16_t buttonPushCounter = 0;	// counter for the number of button presses

// Button state raise (OFF -> ON) callback function
void onButtonRise()
{
	// Another button push detected.
	// Will work correctly only with hardware debounce circuit (capacitor and resistors)
	buttonPushCounter++;

	serial.print("number of button pushes: ");
	serial.println(buttonPushCounter);

	// turns on the LED every four button pushes by checking the modulo of the
	// button push counter. the modulo function gives you the remainder of the
	// division of two numbers
	if (buttonPushCounter % 4 == 0) {
		led.SetValue(HIGH);
	} else {
		led.SetValue(LOW);
	}
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Default Init is enough for printouts via Serial on 115200 boud speed
	serial.Init();

	// Initialize digital input button on line 2, onButtonChange function will be called on state rise (OFF -> ON)
	button.Init(2, onButtonRise, EBF_DigitalInput::InterruptMode::MODE_RISING, true);

	// Initialize built-in LED (generally line 13)
	led.Init(LED_BUILTIN);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
