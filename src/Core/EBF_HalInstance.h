#ifndef __EBF_HALINSTANCE_H__
#define __EBF_HALINSTANCE_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "EBF_Global.h"

class EBF_HalInstance {
	public:
		friend class EBF_Logic;
		friend class EBF_Core;
		friend class EBF_PlugAndPlayHub;

	public:
		EBF_HalInstance();

		~EBF_HalInstance() {
			this->numberOfInstances--;
		}

		// Time functions to use EBF's power-saving correction
		unsigned long micros();
		unsigned long millis();

	protected:
		enum HAL_Type : uint8_t {
			DIGITAL_INPUT = 0,
			DIGITAL_OUTPUT,
			ANALOG_INPUT,
			PWM_OUTPUT,
			UART,
			I2C,
			SPI
		};

		uint8_t Init(HAL_Type type, uint8_t id);
		// Virtual function that will be called to process the data
		virtual uint8_t Process() = 0;
		// Virtual function that will be called to process the interrupt
		virtual void ProcessInterrupt() { }

		static uint8_t GetNumberOfInstances();
		uint32_t GetPollingInterval();
		unsigned long GetLastPollMillis();
		void SetLastPollMillis(unsigned long ms);

		HAL_Type GetType() { return type; };
		uint8_t GetId() { return id; };

		uint32_t pollIntervalMs;		// in milli-Sec
		unsigned long lastPollMillis;

	private:
		static uint8_t numberOfInstances;

		HAL_Type type;
		uint8_t id;
};

#endif
