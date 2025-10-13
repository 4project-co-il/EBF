#ifndef __PNP_INPUT_INTERFACE_H__
#define __PNP_INPUT_INTERFACE_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/PnP_InputInterfaceProvider.h"
#include "../PlugAndPlay/PnP_Module_2ButtonsInput.h"

class PnP_InputInterface {
	public:
		friend class PnP_Module_2ButtonsInput;

		PnP_InputInterface();

		enum InputInterface_Type : uint8_t {
			INPUT = 0,
			SWITCH,
			BUTTON,
			ENCODER,
		};

		virtual InputInterface_Type GetType() = 0;

		uint8_t GetValue() { return pInputProvider->GetValue(providerIndex); }
		uint8_t GetLastValue() { return pInputProvider->GetLastValue(providerIndex); }

		void SetUserData(uint32_t data) { this->userData = data; }
		uint32_t GetUserData() { return this->userData; }

		static PnP_InputInterface* GetCurrentInterface() { return pCurrentProvider->GetCurrentInterface(); }

	protected:
		PnP_InputInterfaceProvider* pInputProvider;
		uint8_t providerIndex;

		virtual void ExecuteCallback() = 0;
		virtual uint8_t AssignInterfaceProvider(PnP_InputInterfaceProvider* pProvider, uint8_t index);
		virtual uint8_t IsProcessingNeeded() { return 0; }
		virtual uint8_t Process() { return EBF_OK; };

		// User data
		uint32_t userData;

		// Current input provider that generates the callbacks
		static PnP_InputInterfaceProvider* pCurrentProvider;
};

#endif
