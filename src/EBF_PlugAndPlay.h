#ifndef __EBF_PLUGANDPLAY_H__
#define __EBF_PLUGANDPLAY_H__

#include <Arduino.h>

#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "EBF.h"

#include "PlugAndPlay/PnP_StatusLed.h"
#include "PlugAndPlay/PnP_Serial.h"
#include "PlugAndPlay/PnP_UART.h"
#ifndef EBF_NO_SPI
#include "PlugAndPlay/PnP_SPI.h"
#endif

#include "PlugAndPlay/PnP_Module_STTS22H_TemperatureSensor.h"
#include "PlugAndPlay/PnP_Module_1Led.h"
#include "PlugAndPlay/PnP_Module_2Led.h"
#include "PlugAndPlay/PnP_Module_1Input.h"
#include "PlugAndPlay/PnP_Module_2Input.h"

#endif
