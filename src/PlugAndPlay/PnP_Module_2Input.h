#ifndef __PnP_MODULE_2INPUT_H__
#define __PnP_MODULE_2INPUT_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_PlugAndPlayDevice.h"
#include "../Core/EBF_HalInstance.h"
#include "../Core/EBF_PlugAndPlayI2C.h"

class PnP_Module_2Input : protected EBF_HalInstance {
	public:
		PnP_Module_2Input();

		uint8_t Init();

		uint8_t GetValue(uint8_t index);
		uint8_t GetValue();

		void SetOnChange1(EBF_CallbackType onChangeCallback) { this->onChangeCallback1 = onChangeCallback; }
		void SetOnChange2(EBF_CallbackType onChangeCallback) { this->onChangeCallback2 = onChangeCallback; }

#ifdef EBF_USE_INTERRUPTS
		uint8_t PostponeProcessing();
		uint8_t InInterrupt() {
			EBF_Logic *pLogic = EBF_Logic::GetInstance();
			return pLogic->IsRunFromIsr();
		}
#endif

	private:
		uint8_t Process();

	 	uint8_t GetIntLine(uint8_t line, uint8_t &value);

	private:
		EBF_PlugAndPlayI2C *pPnPI2C;

		// Callbacks
		EBF_CallbackType onChangeCallback1;
		EBF_CallbackType onChangeCallback2;

#ifdef EBF_USE_INTERRUPTS
		void ProcessInterrupt();
#endif

};

#endif
