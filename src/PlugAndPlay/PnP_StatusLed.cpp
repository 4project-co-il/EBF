#include "PnP_StatusLed.h"

PnP_StatusLed::PnP_StatusLed()
{
}

uint8_t PnP_StatusLed::Init()
{
	return EBF_Led::Init(LED_BUILTIN);
}
