#include "PnP_UART.h"
#include "EBF_Logic.h"

extern void EBF_EmptyCallback();

PnP_UART::PnP_UART() : EBF_Serial(Serial1)
{
}

uint8_t PnP_UART::Init(uint32_t boudRate, uint16_t config)
{
	// Serial number = 1 for Serial1
	return EBF_Serial::Init(1, EBF_EmptyCallback, boudRate, config);
}
