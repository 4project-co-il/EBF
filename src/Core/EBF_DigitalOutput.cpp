#include "EBF_DigitalOutput.h"

uint8_t EBF_DigitalOutput::Init(uint8_t pinNumber)
{
	EBF_HalInstance::Init(HAL_Type::DIGITAL_OUTPUT, pinNumber);

	// Digital outputs should not be polled
	this->pollIntervalMs = EBF_NO_POLLING;

	this->pinNumber = pinNumber;

	pinMode(pinNumber, OUTPUT);

	return 1;
}

uint8_t EBF_DigitalOutput::SetValue(uint8_t value)
{
	digitalWrite(pinNumber, value);

	return 1;
}