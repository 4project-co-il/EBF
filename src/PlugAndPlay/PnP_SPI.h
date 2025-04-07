#ifndef EBF_NO_SPI

#ifndef __PnP_SPI_H__
#define __PnP_SPI_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_SPI.h"

// PnP_SPI class wraps the EBF_SPI implementation, using the defaukt SPI interface
class PnP_SPI : public EBF_SPI {
	public:
		PnP_SPI();

		uint8_t Init();
};

#endif

#endif	// EBF_NO_SPI
