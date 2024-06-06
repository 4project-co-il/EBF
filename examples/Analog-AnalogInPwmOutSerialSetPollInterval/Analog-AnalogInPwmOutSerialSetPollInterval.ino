#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_AnalogInput analongInput;
EBF_PwmOutput pwmOutput;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication
EBF_Serial serial(Serial);

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

// Analog input change
void onAnalog0Change()
{
	// read the analog in value:
	sensorValue = analongInput.GetRawValue();

	// map it to the range of the analog out:
	outputValue = map(sensorValue, 0, 1023, 0, 255);

	// change the analog out value:
	pwmOutput.SetValue(outputValue);

	// print the results to the Serial Monitor:
	Serial.print("sensor = ");
	Serial.print(sensorValue);
	Serial.print("\t output = ");
	Serial.println(outputValue);

	// Don't use delay() with the Event Based Framework!
	// There is a small delay in the main loop to let everthing work properly
	// If the changes are comming too fast, you can use SetPollInterval to limit polling speed
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Default Init is enough for printouts via Serial on 115200 boud speed
	serial.Init();

	// Initialize the analog input on line 0, for any change (0% threshold)
	analongInput.Init(A0, onAnalog0Change, 0);
	// Limit polling of that analog input to once in 50 mSec
	analongInput.SetPollInterval(50);

	// Initialize PWM output on line 9
	pwmOutput.Init(9);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
