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

		uint8_t Init();

		uint8_t GetValue(uint8_t index);
		uint8_t GetValue();

		uint8_t SetLongPressTime(uint8_t index, uint16_t msTime);

		uint8_t SetOnPress(uint8_t index, EBF_CallbackType onPressCallback);
		uint8_t SetOnLongPress(uint8_t index, EBF_CallbackType onLongPressCallback);
		uint8_t SetOnRelease(uint8_t index, EBF_CallbackType onReleaseCallback);

		typedef union {
			struct {
				uint32_t index : 3;		// up to 8 buttons
				uint32_t state : 8;	// button state when the interrupt happened
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
		uint8_t Process();
		void ProcessInterrupt();

	 	uint8_t GetIntLine(uint8_t line, uint8_t &value);

	private:
		EBF_PlugAndPlayI2C *pPnPI2C;

		static const uint8_t numberOfButtons = 2;

		EBF_ButtonLogic button[numberOfButtons];
};

#endif
