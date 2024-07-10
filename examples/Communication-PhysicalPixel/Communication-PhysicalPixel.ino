#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_DigitalOutput led;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication by default
EBF_Serial serial;
// You can use any other Serial interface if availalbe for your device
//EBF_Serial serial(Serial2);

// Serial data callback function
void onSerial()
{
	byte incomingByte;

	// read the oldest byte in the serial buffer
	incomingByte = serial.read();

	// if it's a capital H (ASCII 72), turn on the LED:
	if (incomingByte == 'H') {
	  led.SetValue(HIGH);
	}

	// if it's an L (ASCII 76) turn off the LED:
	if (incomingByte == 'L') {
	  led.SetValue(LOW);
	}
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize serial instance #0 with onSerial callback, with 9600 boud rate speed
	serial.Init(0, onSerial, 9600);

	// Initialize built-in LED (generally on line 13)
	led.Init(LED_BUILTIN);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
