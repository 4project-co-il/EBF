#ifndef __PnP_MODULE_2BUTTONS_INPUT_H__
#define __PnP_MODULE_2BUTTONS_INPUT_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_PlugAndPlayDevice.h"
#include "../Core/EBF_HalInstance.h"
#include "../Core/EBF_PlugAndPlayI2C.h"
#include "../Core/PnP_InputInterface.h"
#include "../Core/PnP_InputInterfaceProvider.h"

class PnP_Module_2ButtonsInput : protected EBF_HalInstance, public PnP_InputInterfaceProvider {
	private:
		EBF_DEBUG_MODULE_NAME("PnP_Module_2ButtonsInput");

	public:
		PnP_Module_2ButtonsInput();

		static const uint8_t numberOfButtons = 2;

		uint8_t Init();

		uint8_t GetValue(uint8_t index);
		uint8_t GetValue();
		uint8_t GetLastValue(uint8_t index);
		PnP_InputInterface* GetCurrentInterface();

		// Set callback functions for specific button
		uint8_t SetOnChange(uint8_t index, EBF_CallbackType onPressCallback);

		// Assign interface instance to specified input index
		uint8_t AssignInterface(uint8_t index, PnP_InputInterface* pIfInstance);
		uint8_t AssignInterface(uint8_t index, PnP_InputInterface& IfInstance) {
			return AssignInterface(index, &IfInstance);
		}

		// Returns button index that caused the callback function call
		// You can have the same callback function for all the button's press events
		// where you can call the GetEventIndex to know which button was actually pressed
		uint8_t GetEventIndex();

		typedef union {
			struct {
				uint32_t index : 3;		// up to 8 buttons
				uint32_t event : 8;		// button event that should be executed
				uint32_t reserved : 21;
			} fields;
			uint32_t uint32;
		} PostponedInterruptData;

		uint8_t PostponeProcessing();
		uint8_t InInterrupt() {
			EBF_Logic *pLogic = EBF_Logic::GetInstance();
			return pLogic->IsRunFromIsr();
		}

	private:
		void SetPollingInterval(uint32_t ms);
		uint8_t Process();
		void ProcessInterrupt();

	 	uint8_t GetIntLine(uint8_t line, uint8_t &value);

		unsigned long millis_IIP() { return this->millis(); }
		void SetPollingInterval_IIP(uint32_t ms) { this->SetPollingInterval(ms); }
		uint32_t GetPollingInterval_IIP() { return this->GetPollingInterval(); }

		// The onChangeCallback should be treated as a pointer to an interface instance after assignment
		PnP_InputInterface* GetAsInputInterface(uint8_t index) { return (PnP_InputInterface*)(onChangeCallback[index]); }

	private:
		EBF_PlugAndPlayI2C *pPnPI2C;

		uint8_t lastValue;
		uint8_t currentEventIndex;

		// Callbacks
		EBF_CallbackType onChangeCallback[numberOfButtons];

		// Interface assigned flag
		uint8_t isInterfaceAssigned;
};

#endif
