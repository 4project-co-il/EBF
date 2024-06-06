#include <Arduino.h>
#include "EBF_Products.h"

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Potentiometer potentiometer;

// Value change callback
void onValueChange()
{
	// And print it via the serial connection
	Serial.print("Potentiometer value: ");
	Serial.println(potentiometer.GetValue());
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Serial connection will be used to print the switch state
	Serial.begin(115200);

	// Initialize potentiometer on analog line #0,
	// looking for 3% change to call the registered function
	// and keep 4 samples for average value calculation
	potentiometer.Init(A0, 3, 4);

	// Register the callback function
	potentiometer.SetOnChange(onValueChange);

	// We can specify the polling time in milli-seconds
	potentiometer.SetPollInterval(10);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
