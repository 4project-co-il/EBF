#include "EBF_PwmOutput.h"

uint8_t EBF_PwmOutput::Init(uint8_t pinNumber)
{
	uint8_t rc;

	rc = EBF_HalInstance::Init(HAL_Type::PWM_OUTPUT, pinNumber);
	if (rc != EBF_OK) {
		return rc;
	}

	/*
	if (!digitalPinHasPWM(pinNumber)) {
		// The pin doesn't support hardware PWM
		// TODO: Implement software PWM
	}
	*/

	// Hardware PWM outputs should not be polled
	this->pollIntervalMs = EBF_NO_POLLING;

	this->pinNumber = pinNumber;

	pinMode(pinNumber, OUTPUT);

	return EBF_OK;
}

uint8_t EBF_PwmOutput::SetValue(uint8_t value)
{
	analogWrite(pinNumber, value);

	return EBF_OK;
}

uint8_t EBF_PwmOutput::SetValue(int value)
{
	analogWrite(pinNumber, value);

	return EBF_OK;
}

uint8_t EBF_PwmOutput::SetValue(float value)
{
	uint8_t uint8Value = (uint8_t)(value / 100.0 * 255);

	analogWrite(pinNumber, uint8Value);

	return EBF_OK;
}