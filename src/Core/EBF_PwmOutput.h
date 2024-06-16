#ifndef __EBF_PWMOUTPUT_H__
#define __EBF_PWMOUTPUT_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_HalInstance.h"

class EBF_PwmOutput : protected EBF_HalInstance {
	public:
		uint8_t Init(uint8_t pinNumber);

		uint8_t SetValue(uint8_t value);
		uint8_t SetValue(int value);
		uint8_t SetValue(float value);

	protected:
		uint8_t pinNumber;
};

#endif
