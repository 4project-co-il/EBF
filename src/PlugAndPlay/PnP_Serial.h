#ifndef __PnP_SERIAL_H__
#define __PnP_SERIAL_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Serial.h"

// PnP_Serial class wraps the EBF_Serial implementation, using the SerialUSB interface
// Default settings are 115200 bps, 8N1
class PnP_Serial : public EBF_Serial {
	public:
		PnP_Serial();

		uint8_t Init();
};

#endif
