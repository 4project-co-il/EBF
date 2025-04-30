#ifndef __PnP_STATUS_LED_H__
#define __PnP_STATUS_LED_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Products/EBF_Led.h"

// PnP_StatusLed class wraps the EBF_Led implementation, hiding the initialization pin
// Current default implementation is to use LED_BUILTIN pin
class PnP_StatusLed : public EBF_Led {
	public:
		PnP_StatusLed();

		uint8_t Init();

};

#endif
