#ifndef __EBF_ANALOGINPUT_H__
#define __EBF_ANALOGINPUT_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif
#include "EBF_Global.h"
#include "EBF_HalInstance.h"

class EBF_AnalogInput : protected EBF_HalInstance {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_AnalogInput");

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

		void SetPollingInterval(uint32_t ms);

	protected:
		uint8_t Process();
		virtual void ProcessCallback() { callbackFunc(); }

		uint8_t pinNumber;
		EBF_CallbackType callbackFunc;
		uint16_t lastValue;
		uint8_t changePercent;

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega88) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
		// Those boards have 10bit ADC
		const uint16_t adcResolution = 1023;
#elif defined(ARDUINO_ARCH_SAMD)
		// Those boards have 12bit ADC
		const uint16_t adcResolution = 4095;
#else
	#error Current board type is not supported
#endif
};

#endif
