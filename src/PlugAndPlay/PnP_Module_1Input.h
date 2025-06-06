#ifndef __PnP_MODULE_1INPUT_H__
#define __PnP_MODULE_1INPUT_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_PlugAndPlayDevice.h"
#include "../Core/EBF_HalInstance.h"
#include "../Core/EBF_PlugAndPlayI2C.h"

class PnP_Module_1Input : protected EBF_HalInstance {
	public:
		PnP_Module_1Input();

		uint8_t Init();

		uint8_t GetValue();

		void SetOnChange(EBF_CallbackType onChangeCallback) { this->onChangeCallback = onChangeCallback; }

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

		// Callbacks
		EBF_CallbackType onChangeCallback;
};

#endif
