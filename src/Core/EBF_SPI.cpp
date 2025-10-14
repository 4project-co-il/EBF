#include "EBF_SPI.h"
#include "EBF_Core.h"

#ifndef EBF_REMOVE_SPI_IMPLEMENTATION

#if defined(ARDUINO_ARCH_SAMD)
EBF_SPI::EBF_SPI()
{
	pSpiInstance = &SPI;
}

EBF_SPI::EBF_SPI(SPIClassSAMD &spiInstance)
{
	pSpiInstance = &spiInstance;
}
#endif

uint8_t EBF_SPI::Init(
	uint8_t serialNumber,
	uint8_t ssPin,
	EBF_CallbackType callbackFunc,
	uint8_t clockDivider
)
{
	uint8_t rc;

	rc = EBF_HalInstance::Init(HAL_Type::SPI_INTERFACE, serialNumber);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	this->callbackFunc = callbackFunc;

	if (callbackFunc == NULL) {
		// No callback. No need to poll in that case
		pollIntervalMs = EBF_NO_POLLING;
	}

	this->ssPin = ssPin;
	pinMode(ssPin, OUTPUT);
	ssRelease();

	pSpiInstance->begin();
	pSpiInstance->setClockDivider(clockDivider);

	return EBF_OK;
}

void EBF_SPI::SetPollInterval(uint32_t ms)
{
	// No polling needed if there is no callback to call
	if (callbackFunc == NULL) {
		pollIntervalMs = EBF_NO_POLLING;
	} else {
		pollIntervalMs = ms;
	}
}

uint8_t EBF_SPI::Process()
{
	// Nothing to do for SPI
	return EBF_OK;
}

#endif // EBF_REMOVE_SPI_IMPLEMENTATION
