#include "PnP_Serial.h"

extern void EBF_EmptyCallback();

PnP_Serial::PnP_Serial() : EBF_Serial(SerialUSB)
{
}

uint8_t PnP_Serial::Init(uint32_t boudRate, uint16_t config)
{
	uint8_t rc;

	// Serial number = 0 for SerialUSB
	rc = EBF_Serial::Init(0, EBF_EmptyCallback, boudRate, config);

	// No polling by default. Users can change if Rx processing callback is needed
	pollIntervalMs = EBF_NO_POLLING;

	return rc;
}
