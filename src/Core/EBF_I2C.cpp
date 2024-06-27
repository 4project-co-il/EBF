#include "EBF_I2C.h"

uint8_t EBF_I2C::Init(EBF_CallbackType callbackFunc, uint8_t address)
{
	uint8_t rc;

	rc = EBF_HalInstance::Init(HAL_Type::I2C, address);
	if (rc != EBF_OK) {
		return rc;
	}

	this->callbackFunc = callbackFunc;

	if (callbackFunc == NULL) {
		// No callback. No need to poll in that case
		pollIntervalMs = EBF_NO_POLLING;
	}

	if (address == 0) {
		i2c.begin();
	} else {
		i2c.begin(address);
	}

	return EBF_OK;
}

void EBF_I2C::SetPollInterval(uint32_t ms)
{
	// No polling needed if there is no callback to call
	if (callbackFunc == NULL) {
		pollIntervalMs = EBF_NO_POLLING;
	} else {
		pollIntervalMs = ms;
	}
}

uint8_t EBF_I2C::Process()
{
	// Callback might not be set, nothing to do in that case
	if (callbackFunc == NULL) {
		return EBF_OK;
	}

	// The stream have data, call the callback
	if (i2c.available()) {
		callbackFunc();
	}

	return EBF_OK;
}
