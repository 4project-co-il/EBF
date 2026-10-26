#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;

EBF_Serial serial;
EBF_SerialCommander commander;

void onTurnOn()
{
	serial.println("Led ON");
}

void onTurnOff()
{
	serial.println("Led OFF");
}

void onSetColor()
{
	float r=0,g=0,b=0;

	serial.println("Led SetColor");

	r = commander.GetNextParameterAsFloat();
	g = commander.GetNextParameterAsFloat();
	b = commander.GetNextParameterAsFloat();

	serial.print("RED:");
	serial.print(r);
	serial.println("%");

	serial.print("GREEN:");
	serial.print(g);
	serial.println("%");

	serial.print("BLUE:");
	serial.print(b);
	serial.println("%");
}

void onBlink()
{
	int times;

	serial.print("Led Blink ");

	times = commander.GetNextParameterAsInt();

	serial.print(times);
	serial.println(" times");
}

void onPrint()
{
	serial.print("Printing string:");
	serial.println(commander.GetNextParameterAsString());
}

void onUnknownCommand()
{
	serial.print("Unknown command: ");
	serial.println(commander.GetCommand());
}

// Commands list that will be accepted via the serial interface.
// The parameters will be accepted with "," as a separator.
// For current example you could send:
// "color,80,23.5,100" - to set LED color for R=80%, G=23.5%, B=100% (float parameters)
// "blink,5" - blink the LED 5 times (integer parameters)
// "print,Hello World!" - print "Hello World" to the serial interface (string parameters)
const EBF_SerialCommander::CommandsMap myCommands[] = {
	{"on", onTurnOn},
	{"off", onTurnOff},
	{"color", onSetColor},
	{"blink", onBlink},
	{"print", onPrint}
};

void setup()
{
	// Uncomment that line if you want to wait for establishment of the serial connection
	//while (!serial) {}

	// EBF error reporting interface should be set before the initialization
	EBF.SetErrorHandlerSerial(serial);

	// EBF is the first thing that should be initialized, with the maximum timers to be used
	EBF.Init(16);

	serial.Init();

	commander.Init(serial, myCommands, onUnknownCommand);

	serial.println("Starting...");
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}