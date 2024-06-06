#include "EBF_Serial.h"

uint8_t EBF_Serial::Init(
	uint8_t serialNumber,
	EBF_CallbackType callbackFunc,
	uint32_t boudRate,
	uint8_t config
)
{
	EBF_HalInstance::Init(HAL_Type::UART, serialNumber);

	this->hwNumber = hwNumber;
	this->callbackFunc = callbackFunc;

	if (callbackFunc == NULL) {
		// No callback. No need to poll in that case
		pollIntervalMs = EBF_NO_POLLING;
	}

	serial.begin(boudRate, config);

	return 1;
}

void EBF_Serial::SetPollInterval(uint16_t ms)
{
	// No polling needed if there is no callback to call
	if (callbackFunc == NULL) {
		pollIntervalMs = EBF_NO_POLLING;
	} else {
		pollIntervalMs = ms;
	}
}

uint8_t EBF_Serial::Process()
{
	// Callback might not be set, nothing to do in that case
	if (callbackFunc == NULL) {
		return 0;
	}

	// The stream have data, call the callback
	if (serial.available()) {
		callbackFunc();
	}

	return 1;
}

