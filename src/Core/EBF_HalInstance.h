#ifndef __EBF_HALINSTANCE_H__
#define __EBF_HALINSTANCE_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "EBF_Global.h"

class EBF_HalInstance {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_HalInstance");

	public:
		friend class EBF_Logic;
		friend class EBF_Core;
		friend class PnP_PlugAndPlayHub;

	public:
		EBF_HalInstance();

		~EBF_HalInstance() {
			this->numberOfInstances--;
		}

		// Time functions to use EBF's power-saving correction
		unsigned long micros();
		unsigned long millis();

	public:
		enum HAL_Type : uint8_t {
			DIGITAL_INPUT = 0,
			DIGITAL_OUTPUT,
			ANALOG_INPUT,
			PWM_OUTPUT,
			UART_INTERFACE,
			I2C_INTERFACE,
			SPI_INTERFACE,
			PnP_DEVICE
		};

		HAL_Type GetType() { return type; };
		uint32_t GetId() { return id; };

		uint32_t GetPollingInterval();
		virtual void SetPollingInterval(uint32_t ms);

	protected:
		uint8_t Init(HAL_Type type, uint32_t id);
		// Virtual function that will be called to process the data
		virtual uint8_t Process() = 0;
		// Virtual function that will be called to process the interrupt
		virtual void ProcessInterrupt() { }

		static uint8_t GetNumberOfInstances();
		unsigned long GetLastPollMillis();
		void SetLastPollMillis(unsigned long ms);

		uint32_t pollIntervalMs;		// in milli-Sec
		unsigned long lastPollMillis;

		HAL_Type type;
		uint32_t id;

	private:
		static uint8_t numberOfInstances;

};

#endif
