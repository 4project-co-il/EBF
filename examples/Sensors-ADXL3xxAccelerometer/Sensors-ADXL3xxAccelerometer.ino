#include <Arduino.h>
#include "EBF.h"

enum {
	SENSOR_TIMER = 0,

	NUMBER_OF_TIMERS
};

// EBF objects creation, should be global
EBF_Core EBF;
EBF_AnalogInput xAxis;
EBF_AnalogInput yAxis;
EBF_AnalogInput zAxis;

EBF_DigitalOutput sensorGnd;
EBF_DigitalOutput sensorPower;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication by default
EBF_Serial serial;
// You can use any other Serial interface if availalbe for your device
//EBF_Serial serial(Serial2);

// Timer callback function
void onTimer()
{
  // print the sensor values:
  serial.print(xAxis.GetValue());

  // print a tab between values:
  serial.print("\t");

  serial.print(yAxis.GetValue());

  // print a tab between values:
  serial.print("\t");

  serial.print(zAxis.GetValue());

  serial.println();

  // Restart the timer
  EBF.StartTimer(SENSOR_TIMER);
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init(NUMBER_OF_TIMERS);

	// Initialize sensor analog inputs, no callback is used for AI change
	xAxis.Init(A3);
	yAxis.Init(A2);
	zAxis.Init(A1);

	// Initialize digital outputs that will be used as a power source for the sensor
	sensorGnd.Init(18);
	sensorPower.Init(19);

	sensorGnd.SetValue(LOW);
	sensorPower.SetValue(HIGH);

	// Initialize serial, default speed is 115200
	serial.Init();

	// Initialize and start the 100 mSec polling timer
	EBF.InitTimer(SENSOR_TIMER, onTimer, 100);
	EBF.StartTimer(SENSOR_TIMER);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
