#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_PwmOutput redPWM;
EBF_PwmOutput greenPWM;
EBF_PwmOutput bluePWM;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication by default
EBF_Serial serial;
// You can use any other Serial interface if availalbe for your device
//EBF_Serial serial(Serial2);

// Serial data callback function
// Pay attention that we use EBF's serial instance instead of Arduino's global Serial (with capital S)
// Both will work since EBF's serial is derrived from the Arduino's Serial on initialization, but
// it's better to stick to one convention. Having EBF's serial instance name to something else,
// such as 'uart' for example, might help to distinguish between the two.
void onSerial()
{
	// look for the next valid integer in the incoming serial stream:
	int red = serial.parseInt();

	// do it again:
	int green = serial.parseInt();

	// do it again:
	int blue = serial.parseInt();

	// Look for the newline. That's the end of your sentence
	// Some terminals send newline as 2 characters (ASCII 10 + ASCII 13)
	while (serial.read() != '\n')
	{
		;
	}

	// constrain the values to 0 - 255 and invert
	// if you're using a common-cathode LED, just use "constrain(color, 0, 255);"
	red = 255 - constrain(red, 0, 255);
	green = 255 - constrain(green, 0, 255);
	blue = 255 - constrain(blue, 0, 255);

	// fade the red, green, and blue legs of the LED:
	redPWM.SetValue(red);
	greenPWM.SetValue(green);
	bluePWM.SetValue(blue);

	// print the three numbers in one string as hexadecimal:
	serial.print(red, HEX);
	serial.print(green, HEX);
	serial.println(blue, HEX);
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize serial instance #0 with onSerial callback, with 9600 boud rate speed
	serial.Init(0, onSerial, 9600);

	// Initialize RED portion of the RGB led on line 3
	redPWM.Init(3);
	// GREEN on line 5
	greenPWM.Init(5);
	// BLUE on line 6
	bluePWM.Init(6);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
