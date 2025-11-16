#include <Arduino.h>
#include "EBF_Products.h"

// This example replaces the Arduino's Sweep example where the servo motor
// is slowly moved from one end to the other.
// The EBF_Servo class uses Arduino's Servo library, so you will have to install
// it in order to compile the example.

// EBF objects creation, should be global
EBF_Core EBF;
EBF_Timer servoTimer;
EBF_Servo servo;

// Starting from 0% value with positive step direction
int percent = 0;
int step = 1;

// Servo timer callback function
void onServoTimer()
{
	// Move servo to currently saved position (in percents from 0% to 100%)
	servo.SetPosition(percent);

	// Calculate next step value
	percent += step;

	// Reached 100% end
	if (percent >= 100) {
		// Reverse the direction and limit to the maximal value of 100%
		step = -step;
		percent = 100;
	}

	// Reached the 0% end
	if (percent <= 0) {
		// Reverse the direction and limit to the minimal value of 0%
		step = -step;
		percent = 0;
	}

	// EBF timers are one-shot in nature, restart it
	servoTimer.Start();
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Initialize servo timer for 20 mSec
	servoTimer.Init(onServoTimer, 20);

	// Start the timer
	servoTimer.Start();

	// Initialize servo on line 9
	servo.Init(9);
}


void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
