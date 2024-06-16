#ifndef __EBF_HCSR04_DISTANCESENSOR_H__
#define __EBF_HCSR04_DISTANCESENSOR_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_DigitalInput.h"
#include "../Core/EBF_DigitalOutput.h"

class EBF_HCSR04_DistanceSensor : protected EBF_DigitalInput {
	public:
		uint8_t Init(uint8_t triggerPinNumber, uint8_t echoPinNumber);
		float GetValue();
		float GetLastValue() { return lastValue; }

		// Setting polling interval in milli-seconds
		void SetPollInterval(uint16_t ms) { pollIntervalMs = ms; }

		void SetOnChange(EBF_CallbackType onChangeCallback, uint8_t changePercent = 5);

	protected:
		uint8_t Process();

		// Callbacks
		// EBF_DigitalInput callback pointer will be used

	private:
		EBF_DigitalOutput trigger;
		float lastValue;
		uint8_t changePercent;
};

#endif
