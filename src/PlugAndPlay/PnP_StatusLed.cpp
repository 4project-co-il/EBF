#include "PnP_StatusLed.h"

PnP_StatusLed::PnP_StatusLed()
{
}

uint8_t PnP_StatusLed::Init()
{
	// Polling intervals are calculated by the EBF_Led Process function based on its state
	return EBF_Led::Init(LED_BUILTIN);
}
