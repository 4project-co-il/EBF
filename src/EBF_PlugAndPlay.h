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

#include "PlugAndPlay/PnP_STTS22H_TemperatureSensor.h"

#endif
