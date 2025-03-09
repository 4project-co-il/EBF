#include "PnP_Serial.h"

extern void EBF_EmptyCallback();

PnP_Serial::PnP_Serial() : EBF_Serial(SerialUSB)
{
}

uint8_t PnP_Serial::Init(uint32_t boudRate, uint16_t config)
{
	// Serial number = 0 for SerialUSB
	return EBF_Serial::Init(0, EBF_EmptyCallback, boudRate, config);
}
