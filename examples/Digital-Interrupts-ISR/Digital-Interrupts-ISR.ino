#include <Arduino.h>
#include "EBF.h"

// This example demonstrates the advanced use of hardwre interrupts with the Event Based Framework - EBF
// By default, the EBF core doesn't use interrupts and that part of the code is not compiled to save memory.
// Since interrupts are more advanced topic, there are few things to be done in order to use that code.
// To have the interrupts processing code compiled, the library needs the EBF_USE_INTERRUPTS flag to be defined
// project wide.
// For this advanced example, you will also need the EBF_DIRECT_CALL_FROM_ISR flag, which will enable the EBF
// to call your functions directly from the ISR (Interrupt Service Routine).
// If you're using the PlatformIO environment, you could add the following line to the project platform.ini file:
// build_flags =
//     -D EBF_USE_INTERRUPTS
//     -D EBF_DIRECT_CALL_FROM_ISR
//
// Since the Arduino IDE doesn't provide any way to add project wide definitions, there is additional way to add that
// flag to the compilation...
// In the library src directory you will find the "EBF_Config_TEMPLATE.h" file, which include all the configuration
// flags for the EBF library.
// If you're using PlatformIO, you can save the content of that file as "Project_Config.h" file in your project "include"
// directory.
// If you're using Arduino IDE, copy that file and save it's content as "Project_Config.h" file in the library "src" directory
//
// In the "Project_Config.h" file you will find the flag definition commented out:
// #define EBF_USE_INTERRUPTS
// #define EBF_DIRECT_CALL_FROM_ISR
// Uncomment those lines to have the interrupts code to be compiled into the EBF library

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Timer timer;
EBF_DigitalOutput led;
EBF_DigitalInput button;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication by default
EBF_Serial serial;
// You can use any other Serial interface if availalbe for your device
//EBF_Serial serial(Serial2);

// LED Timer callback function
void onTimer()
{
	// We will use the 1sec timer as a timing reference in printouts
	serial.println("1sec passed");

	// EBF timers are one-shot in nature, restart it
	timer.Start();
}

// The callback function for the digital input change
// Since interrupts are enabled for that example, and direct ISR calls as well,
// the function will be called as an ISR - Interrupt Service Routine.
// You can do whatever you need in the ISR, as in regular function, but the operation
// should be fast. It is advised not to use Serial or other communication channels
// directly from the ISR since other interrupts are not processed while the function
// is executed, hence some buffers might overflow. Printing small messages is OK.
// If you have a longer task, you can return the control to EBF, which will use internal
// message queue to pass the execution from ISR level to normal run, and will call the
// same function again to process the logic of the digital input change from the normal run mode.
void onButtonChange()
{
	uint8_t buttonState;

	// If the function is called directly from an interrupt (ISR mode) since the EBF_DIRECT_CALL_FROM_ISR flag is defined
	if (EBF.InInterrupt()) {
		serial.println("ISR!");

		// Pass the processing back to EBF
		EBF.PostponeInterrupt(button);
		// And return from that function
		return;
	}

	// The function will be called again, as a normal run and not from the interrupt

	// We use GetLastValue instead of the GetValue, to use the value that triggered
	// the callback. The digital input might change again since then
	buttonState = button.GetLastValue();

	// Use the LED and serial printouts as a visualization when the change was detected
	led.SetValue(buttonState);
	serial.print("Button changed to: ");
	serial.println(buttonState);
}

void setup()
{
	// EBF is the first thing that should be initialized
	// The parameter of the EBF Init function is the message queue size used to pass messages
	// from interrupt to normal run. Think of it as a global flag flipped to indicate that
	// there was an interrupt and additional processing should be done after it was detected
	// The number of message is something very specific to the project and you will have to
	// experiment with that. There might be one interrupt source that will generate big amount
	// of messages, on the other hand there might be many sources for interrupts that arrive
	// very slow, so the normal run will be able to execute them between the interrupts and
	// only one message might be enough
	//
	// We set the message queue here to 4 for that example.
	// You can use the EBF.GetNumberOfMessages() function to see number of message in the queue
	// at any time. And the EBF.GetMaxNumberOfMessages() function to see the total maximal
	// number the message queue reached during the program run.
	// Those function might help you tune the system to get to the right needed message queue
	// size and save some RAM memory if needed.
	EBF.Init(4);

	// Default Init is enough for printouts via Serial on 115200 boud speed
	serial.Init();

	serial.println("Starting...");

	// Initialize the timer for 1sec (1000 mSec), onTimer function will be called
	timer.Init(onTimer, 1000);

	// Start the LED timer for the first time
	timer.Start();

	// Initialize built-in LED (generally on line 13)
	led.Init(LED_BUILTIN);

	// Initialize button on digital input 2 for any change of the value
	// Internal pullup resistor will be used to minimize bouncing
	button.Init(2, onButtonChange, EBF_DigitalInput::InterruptMode::MODE_CHANGE, true);

	// AttachInterrupt() should be called for the button instance to utilize
	// the hardware interrupt line based on the configuration passed in Init() function
	// On Arduino UNO lines 2 and 3 are capable to produce interrupts.
	// When interrupts are used, polling is disabled on those pins
	// Pay attention that there is no change in the initialization of the digital input
	// object. The same onButtonChange function will be called.
	button.AttachInterrupt();
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}