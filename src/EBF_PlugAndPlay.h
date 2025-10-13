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
#ifndef EBF_REMOVE_SPI_IMPLEMENTATION
#include "PlugAndPlay/PnP_SPI.h"
#endif

#include "Core/PnP_InputInterface.h"
#include "Core/PnP_SwitchInterface.h"
#include "Core/PnP_ButtonInterface.h"
#include "PlugAndPlay/PnP_Module_STTS22H_TemperatureSensor.h"
#include "PlugAndPlay/PnP_Module_1Led.h"
#include "PlugAndPlay/PnP_Module_2Led.h"
#include "PlugAndPlay/PnP_Module_1Input.h"
#include "PlugAndPlay/PnP_Module_2Input.h"
#include "PlugAndPlay/PnP_Module_2ButtonsInput.h"
#include "PlugAndPlay/PnP_Module_SparkFun_QWIIC_SerLCD.h"
#include "PlugAndPlay/PnP_Module_Seeed_Monochrome_GROVE_16x2_LCD.h"

#endif
