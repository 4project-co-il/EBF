#include "EBF_AnalogInput.h"
#include "EBF_Core.h"

uint8_t EBF_AnalogInput::Init(
	uint8_t pinNumber,
	EBF_CallbackType callbackFunc,
	uint8_t changePercent
)
{
	uint8_t rc;

	rc = EBF_HalInstance::Init(HAL_Type::ANALOG_INPUT, pinNumber);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	this->pinNumber = pinNumber;
	this->callbackFunc = callbackFunc;
	this->changePercent = changePercent;

	if (callbackFunc != NULL) {
		lastValue = analogRead(pinNumber);
	} else {
		// No callback. No need to poll in that case
		SetPollingInterval(EBF_NO_POLLING);
	}

	return EBF_OK;
}

void EBF_AnalogInput::SetPollingInterval(uint32_t ms)
{
	// No polling needed if there is no callback to call
	if (callbackFunc == NULL) {
		EBF_HalInstance::SetPollingInterval(EBF_NO_POLLING);
	} else {
		EBF_HalInstance::SetPollingInterval(ms);
	}
}

uint8_t EBF_AnalogInput::Process()
{
	uint16_t currentValue;
	int32_t change;

	// Callback might not be set, nothing to do in that case
	if (callbackFunc == NULL) {
		return EBF_OK;
	}

	currentValue = analogRead(pinNumber);
	change = ((int32_t)currentValue - (int32_t)lastValue) * 100 / (int32_t)adcResolution;

	if (abs(change) > changePercent) {
		lastValue = currentValue;
		ProcessCallback();
	}

	return EBF_OK;
}

float EBF_AnalogInput::GetValue()
{
	return (int32_t)analogRead(pinNumber) * 100.0 / adcResolution;
}

uint16_t EBF_AnalogInput::GetRawValue()
{
	return analogRead(pinNumber);
}

float EBF_AnalogInput::GetLastValue()
{
	return (int32_t)lastValue * 100.0 / adcResolution;
}

uint16_t EBF_AnalogInput::GetRawLastValue()
{
	return lastValue;
}
