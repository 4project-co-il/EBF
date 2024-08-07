#ifndef __PnP_STTS22H_TEMPERATURESENSOR_H__
#define __PnP_STTS22H_TEMPERATURESENSOR_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_PlugAndPlayDevice.h"
#include "../Products/EBF_STTS22H_TemperatureSensor.h"
#include <Wire.h>

class PnP_STTS22H_TemperatureSensor : public EBF_STTS22H_TemperatureSensor, public EBF_PlugAndPlayDevice {
	public:
		PnP_STTS22H_TemperatureSensor() : EBF_STTS22H_TemperatureSensor(NULL), EBF_PlugAndPlayDevice(PNP_ID_STTS22H_TEMPERATURE_SENSOR) {}

		uint8_t Init();

};

#endif
