#ifndef __EBF_SPI_H__
#define __EBF_SPI_H__

#ifndef EBF_REMOVE_SPI_IMPLEMENTATION

#include <Arduino.h>

#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_HalInstance.h"
#include "SPI.h"

class EBF_SPI : protected EBF_HalInstance {
	public:
#if defined(ARDUINO_ARCH_SAMD)
		EBF_SPI();
		EBF_SPI(SPIClassSAMD &spiInstance);
#endif
		uint8_t Init(
			uint8_t serialNumber = 0,
			uint8_t ssPin = SS,
			EBF_CallbackType callbackFunc = NULL,
			uint8_t clockDivider = SPI_CLOCK_DIV64
		);
		uint8_t Process();

		void SetPollInterval(uint32_t ms);
		void SetCallback(EBF_CallbackType callbackFunc) { this->callbackFunc = callbackFunc; }

		inline void ssSelect() { digitalWrite(ssPin, LOW); }
		inline void ssRelease() { digitalWrite(ssPin, HIGH); }
		inline byte transfer(uint8_t data) { return pSpiInstance->transfer(data); }

	private:
		EBF_CallbackType callbackFunc;

		uint8_t ssPin;

#if defined(ARDUINO_ARCH_SAMD)
		SPIClassSAMD* pSpiInstance;
#endif

};

#endif

#endif	// EBF_REMOVE_SPI_IMPLEMENTATION
