#ifndef __PnP_MODULE_1LED_H__
#define __PnP_MODULE_1LED_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_PlugAndPlayDevice.h"
#include "../Core/EBF_HalInstance.h"
#include "../Core/EBF_PlugAndPlayI2C.h"

class PnP_Module_1Led : protected EBF_HalInstance {
	private:
		EBF_DEBUG_MODULE_NAME("PnP_Module_1Led");

	public:
		PnP_Module_1Led();

		uint8_t Init();

		uint8_t On();
		uint8_t Off();

	private:
		uint8_t Process();

	 	uint8_t SetIntLine(uint8_t line, uint8_t value);

	private:
		EBF_PlugAndPlayI2C *pPnPI2C;
};

#endif
