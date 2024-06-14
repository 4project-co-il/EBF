#ifndef __EBF_SERVO_H__
#define __EBF_SERVO_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_HalInstance.h"

// That class uses Arduino's Servo library that have to be added to the project
#include <Servo.h>

class EBF_Servo : protected EBF_HalInstance {
	public:
		uint8_t Init(uint8_t pinNumber);
		uint8_t SetPosition(uint8_t percent);
		uint8_t SetPosition(uint8_t percent, uint16_t msDuration, uint8_t steps);

		uint8_t GetPosition() { return lastPercent; }

	private:
		enum ServoState {
			SERVO_IDLE = 0,
			SERVO_MOVING,
		};

		ServoState state;
		Servo servo;
		uint8_t lastPercent;		// In percents 0% - 100%
		uint8_t targetPercent;		// In percents 0% - 100%
		int8_t stepPercent;			// Percents to add every step
		uint16_t stepTime;			// mSec of every step
		unsigned long effectStart;	// in micro-Sec

		uint8_t Process();
};

#endif
