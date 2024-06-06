#ifndef __EBF_LED_H__
#define __EBF_LED_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_PwmOutput.h"

class EBF_Led : protected EBF_PwmOutput {
	public:
		uint8_t Init(uint8_t pinNumber);
		uint8_t SetValue(uint8_t value);

		uint8_t On();
		uint8_t Off();
		uint8_t SetBrightness(uint8_t percent);
		uint8_t Blink(uint16_t msOn, uint16_t msOff);
		uint8_t FadeIn(uint16_t msDuration, uint8_t steps);
		uint8_t FadeOut(uint16_t msDuration, uint8_t steps);

	private:
		enum LedState {
			LED_OFF = 0,
			LED_ON,
			LED_BLINKING_ON,
			LED_BLINKING_OFF,
			LED_FADING_IN,
			LED_FADING_OUT,
		};

		LedState state;
		uint8_t brightness;			// 0-255 values
		unsigned long effectStart;	// in micro-Sec
		uint16_t onDuration;		// in milli-Sec
		uint16_t offDuration;		// in milli-Sec
		uint8_t fadeStep;

		uint8_t Process();
};

#endif
