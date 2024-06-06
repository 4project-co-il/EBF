#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;

// Array of leds
const uint8_t numberOfLeds = 5;
EBF_DigitalOutput leds[numberOfLeds];

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication
EBF_Serial serial(Serial);

// Serial data callback function
void onSerial()
{
	byte inByte;

	// read the oldest byte in the serial buffer
	inByte = serial.read();

	// do something different depending on the character received.
	// The switch statement expects single number values for each case; in this
	// example, though, you're using single quotes to tell the controller to get
	// the ASCII value for the character. For example 'a' = 97, 'b' = 98,
	// and so forth:
	switch (inByte)
	{
	case 'a':
		leds[0].SetValue(HIGH);
		break;

	case 'b':
		leds[1].SetValue(HIGH);
		break;

	case 'c':
		leds[2].SetValue(HIGH);
		break;

	case 'd':
		leds[3].SetValue(HIGH);
		break;

	case 'e':
		leds[4].SetValue(HIGH);
		break;

	default:
		// turn all the LEDs off
		for (int ledNum = 0; ledNum < numberOfLeds; ledNum++) {
			leds[ledNum].SetValue(LOW);
		}
	}
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize serial instance #0 with onSerial callback, with 9600 boud rate speed
	serial.Init(0, onSerial, 9600);

	// Initialize the leds, starting from line 2 and up
	for (uint8_t ledNum = 0; ledNum < numberOfLeds; ledNum++)
	{
		leds[ledNum].Init(ledNum + 2);
	}
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
