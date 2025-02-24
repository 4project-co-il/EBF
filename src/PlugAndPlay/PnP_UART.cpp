#include "PnP_UART.h"

PnP_UART::PnP_UART() : EBF_Serial(Serial1)
{
}

uint8_t PnP_UART::Init()
{
	return EBF_Serial::Init();
}
