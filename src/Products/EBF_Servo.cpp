#include "EBF_Servo.h"

// Initializing EBF_Servo class instance.
// The pinNumber should be on hardware line that is supported by Arduino's Servo library.
// For Arduino UNO board, those are lines 9 and 10
uint8_t EBF_Servo::Init(uint8_t pinNumber)
{
	uint8_t rc;

	state = SERVO_IDLE;
	lastPercent = 0;

	rc = EBF_HalInstance::Init(EBF_HalInstance::HAL_Type::PWM_OUTPUT, pinNumber);
	if (rc != EBF_OK) {
		return rc;
	}

	servo.attach(pinNumber);

	return EBF_OK;
}

// Sets the position of the servo motor in percents.
// That command is passed immediatelly to the servo.
// 0% is the max point in one direction, 100% is max point in other direction
// 50% is the middle of the movement.
// For continous rotation servo motors:
// 0% is a max speed in one direction, 100% is max speed in other direction
// 50% is a stop of the rotation
uint8_t EBF_Servo::SetPosition(uint8_t percent)
{
	uint8_t pos;

	if (percent > 100) {
		percent = 0;
	}

	lastPercent = percent;

	pos = percent * 1.8;

	servo.write(pos);

	return EBF_OK;
}

// Move the servo motor to the specified percent position
// Perform the movement during the msDuration within number of steps
uint8_t EBF_Servo::SetPosition(uint8_t percent, uint16_t msDuration, uint8_t steps)
{
	if (percent == lastPercent) {
		// Nothing to do
		return EBF_OK;
	}

	targetPercent = percent;

	if (targetPercent > 100) {
		targetPercent = 100;
	}

	stepPercent = ((int8_t)targetPercent - (int8_t)lastPercent) / (int8_t)steps;
	stepTime = msDuration / steps;

	// When step percent is 0, meaning there are too many steps for that change
	// update it with 1% per step
	if (stepPercent == 0) {

		if (targetPercent > lastPercent) {
			steps = targetPercent - lastPercent;
			stepPercent = 1;
		} else {
			steps = lastPercent - targetPercent;
			stepPercent = -1;
		}

		stepTime = msDuration / steps;
	}

	state = SERVO_MOVING;

	effectStart = micros();

	return EBF_OK;
}

uint8_t EBF_Servo::Process()
{
	uint8_t rc = EBF_OK;
	uint8_t pos;
	int8_t diff;
	unsigned long timePassed;

	if (state == SERVO_MOVING) {
		timePassed = micros() - effectStart;

		if (timePassed > stepTime * 1000) {
			effectStart = micros();
			lastPercent += stepPercent;

			if (lastPercent + stepPercent > 100) {
				lastPercent = 100;
			} else if ((int8_t)lastPercent + stepPercent < 0) {
				lastPercent = 0;
			}

			pos = lastPercent * 1.8;

			servo.write(pos);

			// Destination reached
			if (targetPercent == lastPercent) {
				state = SERVO_IDLE;
			}

			// Last step might be less than the stepPercent, should limit it
			diff = (int8_t)targetPercent - (int8_t)lastPercent;
			if (abs(stepPercent) > abs(diff)) {
				stepPercent = diff;
			}
		}
	}

	return rc;
}