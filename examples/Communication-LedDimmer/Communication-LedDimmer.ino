#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_PwmOutput led;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication by default
EBF_Serial serial;
// You can use any other Serial interface if availalbe for your device
//EBF_Serial serial(Serial2);

// Serial data callback function
void onSerial()
{
	byte brightness;

	// read the most recent byte (which will be from 0 to 255):
    brightness = serial.read();

    // set the brightness of the LED:
    led.SetValue(brightness);
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize serial instance #0 with onSerial callback, with 9600 boud rate speed
	serial.Init(0, onSerial, 9600);

	// Initialize PWM output on line 9
	led.Init(9);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
