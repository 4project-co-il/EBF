#ifndef __EBF_ANALOGINPUT_H__
#define __EBF_ANALOGINPUT_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif
#include "EBF_Global.h"
#include "EBF_HalInstance.h"

class EBF_AnalogInput : protected EBF_HalInstance {
	public:
		uint8_t Init(
			uint8_t pinNumber,
			EBF_CallbackType callbackFunc = NULL,
			uint8_t changePercent = 1
		);

		float GetValue();
		uint16_t GetRawValue();
		float GetLastValue();
		uint16_t GetRawLastValue();

		void SetPollInterval(uint16_t ms);

	protected:
		uint8_t Process();
		virtual void ProcessCallback() { callbackFunc(); }

		uint8_t pinNumber;
		EBF_CallbackType callbackFunc;
		uint16_t lastValue;
		uint8_t changePercent;

		// TODO: should IFDEF here for different processors
		const uint16_t adcResolution = 1023;
};

#endif
