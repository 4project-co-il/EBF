#ifndef __PnP_MODULE_SPARKFUN_QWIIC_SERLCD_H__
#define __PnP_MODULE_SPARKFUN_QWIIC_SERLCD_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_PlugAndPlayDevice.h"
#include "../Core/EBF_PlugAndPlayManager.h"
#include "../Core/EBF_PlugAndPlayI2C.h"
#include "../Products/EBF_SparkFun_QWIIC_SerLCD.h"
#include <Wire.h>

class PnP_Module_SparkFun_QWIIC_SerLCD : public EBF_SparkFun_QWIIC_SerLCD {
	public:
		PnP_Module_SparkFun_QWIIC_SerLCD();

		uint8_t Init();

};

#endif
