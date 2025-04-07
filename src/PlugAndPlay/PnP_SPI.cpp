#include "PnP_SPI.h"
#include "EBF_Logic.h"

extern void EBF_EmptyCallback();

PnP_SPI::PnP_SPI() : EBF_SPI()
{
}

uint8_t PnP_SPI::Init()
{
	// Using default constructor
	return EBF_SPI::Init();
}
