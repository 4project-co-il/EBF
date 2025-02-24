#include "PnP_Serial.h"

PnP_Serial::PnP_Serial() : EBF_Serial(SerialUSB)
{
}

uint8_t PnP_Serial::Init()
{
	return EBF_Serial::Init();
}
