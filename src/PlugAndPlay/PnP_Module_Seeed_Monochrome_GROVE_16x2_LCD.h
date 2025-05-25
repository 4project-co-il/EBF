#ifndef __PnP_MODULE_SEEED_MONOCHROME_GROVE_16x2_LCD_H__
#define __PnP_MODULE_SEEED_MONOCHROME_GROVE_16x2_LCD_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_PlugAndPlayDevice.h"
#include "../Core/EBF_PlugAndPlayManager.h"
#include "../Core/EBF_PlugAndPlayI2C.h"
#include "../Products/EBF_Seeed_Monochrome_GROVE_16x2_LCD.h"
#include <Wire.h>

class PnP_Module_Seeed_Monochrome_GROVE_16x2_LCD : public EBF_Seeed_Monochrome_GROVE_16x2_LCD {
	public:
		PnP_Module_Seeed_Monochrome_GROVE_16x2_LCD();

		uint8_t Init();

};

#endif
