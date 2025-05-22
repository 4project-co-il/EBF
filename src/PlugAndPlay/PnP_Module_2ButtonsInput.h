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
#include "../Core/EBF_ButtonLogic.h"

class PnP_Module_2ButtonsInput : protected EBF_HalInstance {
	public:
		PnP_Module_2ButtonsInput();

		static const uint8_t numberOfButtons = 2;

		uint8_t Init();

		uint8_t GetValue(uint8_t index);
		uint8_t GetValue();

		// Set long-press timeout in mSec for a specific button
		uint8_t SetLongPressTime(uint8_t index, uint16_t msTime);

		// Set callback functions for specific button
		uint8_t SetOnPress(uint8_t index, EBF_CallbackType onPressCallback);
		uint8_t SetOnLongPress(uint8_t index, EBF_CallbackType onLongPressCallback);
		uint8_t SetOnRelease(uint8_t index, EBF_CallbackType onReleaseCallback);

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

	private:
		EBF_PlugAndPlayI2C *pPnPI2C;

		volatile EBF_ButtonLogic::ButtonEvent lastEvent;
		uint8_t currentEventIndex;

		EBF_ButtonLogic button[numberOfButtons];
};

#endif
