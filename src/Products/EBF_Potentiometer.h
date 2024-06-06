#ifndef __EBF_POTENTIOMETER_H__
#define __EBF_POTENTIOMETER_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_AnalogInput.h"

class EBF_Potentiometer : protected EBF_AnalogInput {
	public:
		uint8_t Init(uint8_t pinNumber, uint8_t changePercent = 1, uint8_t averagingSamples = 1);
		float GetValue();
		uint16_t GetRawValue() { return averageRawValue; }

		// Setting polling interval in milli-seconds
		void SetPollInterval(uint16_t ms) { pollIntervalMs = ms; }

		void SetOnChange(EBF_CallbackType onChangeCallback) { this->callbackFunc = onChangeCallback; }

	protected:
		uint8_t Process();

		// Callbacks
		// EBF_AnalogInput callback pointer will be used

	private:
		uint16_t numOfSamples;		// Number of samples will be stored back for averaging
		uint16_t averageRawValue;	// Based on all available samples
		uint16_t *pSamples;			// Samples data
		uint8_t currentSample;

		uint16_t GetRawAverage();
};

#endif
