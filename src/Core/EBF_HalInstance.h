#ifndef __EBF_HALINSTANCE_H__
#define __EBF_HALINSTANCE_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

class EBF_HalInstance {
	public:
		friend class EBF_Logic;

	public:
		EBF_HalInstance();

		~EBF_HalInstance() {
			this->numberOfInstances--;
		}

	protected:
		enum HAL_Type {
			DIGITAL_INPUT = 0,
			DIGITAL_OUTPUT,
			ANALOG_INPUT,
			PWM_OUTPUT,
			UART,
			I2C,
			SPI
		};

		uint8_t Init(HAL_Type type, uint8_t id);
		virtual uint8_t Process() { return 1; };

		static uint8_t GetNumberOfInstances();
		uint16_t GetPollingInterval();
		unsigned long GetLastPollMillis();
		void SetLastPollMillis(unsigned long ms);

		HAL_Type GetType() { return type; };
		uint8_t GetId() { return id; };

		uint16_t pollIntervalMs;		// in milli-Sec
		unsigned long lastPollMillis;

	private:
		static uint8_t numberOfInstances;

		HAL_Type type;
		uint8_t id;
};

#endif
