#ifndef __PnP_UART_H__
#define __PnP_UART_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Serial.h"

// PnP_UART class wraps the EBF_Serial implementation, using the Serial1 interface
// Default settings are 115200 bps, 8N1
class PnP_UART : public EBF_Serial {
	public:
		PnP_UART();

		uint8_t Init();
};

#endif
