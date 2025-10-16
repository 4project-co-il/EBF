#include "PnP_UART.h"

extern void EBF_EmptyCallback();

PnP_UART::PnP_UART() : EBF_Serial(Serial1)
{
}

uint8_t PnP_UART::Init(uint32_t boudRate, uint16_t config)
{
	uint8_t rc;

	// Serial number = 1 for Serial1
	rc = EBF_Serial::Init(1, EBF_EmptyCallback, boudRate, config);

	// No polling by default. Users can change if Rx processing callback is needed
	pollIntervalMs = EBF_NO_POLLING;

	return rc;
}
