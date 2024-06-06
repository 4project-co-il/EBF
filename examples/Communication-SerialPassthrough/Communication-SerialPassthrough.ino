#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication
EBF_Serial uart0(Serial);
EBF_Serial uart1(Serial1);

// Everything arriving to serial #0 will be sent to serial #1
void onSerial0()
{
	uart1.write(uart0.read());
}

// Everything arriving to serial #1 will be sent to serial #0
void onSerial1()
{
	uart0.write(uart1.read());
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize serial instance #0 with onSerial0 callback
	uart0.Init(0, onSerial0);
	// Initialize serial instance #1 with onSerial1 callback
	uart1.Init(1, onSerial1);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
