#ifndef __PNP_INPUT_INTERFACE_PROVIDER_H__
#define __PNP_INPUT_INTERFACE_PROVIDER_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/PnP_InputInterface.h"

class PnP_InputInterface;

class PnP_InputInterfaceProvider {
	public:
		friend class PnP_ButtonInterface;

		// Assign interface instance to input provider
		virtual uint8_t AssignInterface(uint8_t index, PnP_InputInterface* pIfInstance) = 0;

		// Get current value of the input
		virtual uint8_t GetValue(uint8_t index) = 0;
		// Get the value as was registered during latest interrupt
		virtual uint8_t GetLastValue(uint8_t index) = 0;

		// Get pointer to current interface instance
		virtual PnP_InputInterface* GetCurrentInterface() = 0;

	private:
		// Access to current millis value of the HAL instance via InputInterfaceProvider
		virtual unsigned long millis_IIP() = 0;

		// Access to polling interval of the HAL instance via InputInterfaceProvider
		virtual void SetPollingInterval_IIP(uint32_t ms) = 0;

		// Access to polling interval of the HAL instance via InputInterfaceProvider
		virtual uint32_t GetPollingInterval_IIP() = 0;

};

#endif
