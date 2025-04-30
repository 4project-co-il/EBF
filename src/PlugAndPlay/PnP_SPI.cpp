#ifndef EBF_REMOVE_SPI_IMPLEMENTATION

#include "PnP_SPI.h"
#include "EBF_Logic.h"

extern void EBF_EmptyCallback();

PnP_SPI::PnP_SPI() : EBF_SPI()
{
}

uint8_t PnP_SPI::Init()
{
	uint8_t rc;

	// Using default constructor
	rc = EBF_SPI::Init();

	// No polling by default. Users can change if Rx processing callback is needed
	pollIntervalMs = EBF_NO_POLLING;

	return rc;
}

#endif	// EBF_REMOVE_SPI_IMPLEMENTATION
