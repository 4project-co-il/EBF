#include "EBF_DigitalOutput.h"
#include "EBF_Core.h"

uint8_t EBF_DigitalOutput::Init(uint8_t pinNumber)
{
	uint8_t rc;

	rc = EBF_HalInstance::Init(HAL_Type::DIGITAL_OUTPUT, pinNumber);
	if (rc!= EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Digital outputs should not be polled
	this->SetPollingInterval(EBF_NO_POLLING);

	this->pinNumber = pinNumber;

	pinMode(pinNumber, OUTPUT);

	EBF_REPORT_AND_RETURN(EBF_OK);
}

uint8_t EBF_DigitalOutput::SetValue(uint8_t value)
{
	digitalWrite(pinNumber, value);

	EBF_REPORT_AND_RETURN(EBF_OK);
}