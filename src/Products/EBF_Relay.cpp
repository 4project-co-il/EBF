#include "EBF_Relay.h"
#include "../Core/EBF_Core.h"

uint8_t EBF_Relay::Init(uint8_t pinNumber)
{
	uint8_t rc;

	rc = EBF_DigitalOutput::Init(pinNumber);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	state = RELAY_OFF;

	EBF_REPORT_AND_RETURN(EBF_OK);
}

// SetValue acts as an ON/OFF function, value == 0 will perform as OFF, any other value as ON
uint8_t EBF_Relay::SetValue(uint8_t value)
{
	uint8_t rc;

	if (value == 0) {
		state = RELAY_OFF;
	} else {
		state = RELAY_ON;
		value = 1;
	}

	rc = EBF_DigitalOutput::SetValue(value);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Relay::On()
{
	uint8_t rc;

	state = RELAY_ON;

	rc = EBF_DigitalOutput::SetValue(1);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Relay::Off()
{
	uint8_t rc;

	state = RELAY_OFF;

	rc = EBF_DigitalOutput::SetValue(0);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Relay::Process()
{
	// Nothing to do
	// No polling needed
	SetPollingInterval(EBF_NO_POLLING);

	EBF_REPORT_AND_RETURN(EBF_OK);
}
