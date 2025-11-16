#include <Arduino.h>
#include "EBF_Products.h"

// In this example the EBF_Servo is doing everything needed to move the servo slowly.
// The timer is used to change the direction of the movement and not step-by-step updates.
// The EBF_Servo class uses Arduino's Servo library, so you will have to install
// it in order to compile the example.

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Timer servoTimer;
EBF_Servo servo;

// Servo timer callback function
void onServoTimer()
{
	if (servo.GetPosition() == 0) {
		Serial.println("Moving from 0% to 100%");
		// Let EBF_Servo handle the slow move from 0% to 100% during 2 seconds, with 100 steps (20 mSec between the updates)
		servo.SetPosition(100, 2000, 100);
	} else {
		Serial.println("Moving from 100% to 0%");
		// Let EBF_Servo handle the slow move from 100% to 0% during 2 seconds, with 100 steps (20 mSec between the updates)
		servo.SetPosition(0, 2000, 100);
	}

	// EBF timers are one-shot in nature, restart it
	servoTimer.Start();
}

void setup()
{
	Serial.begin(115200);

	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize servo timer for 3 seconds
	servoTimer.Init(onServoTimer, 2000);

	// Start the timer
	servoTimer.Start();

	// Initialize servo on line 9
	servo.Init(9);

	// Start servo move, the timer will continue from there
	servo.SetPosition(100, 2000, 100);
}


void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
