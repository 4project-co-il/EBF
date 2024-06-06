#include "EBF_Relay.h"

uint8_t EBF_Relay::Init(uint8_t pinNumber)
{
	EBF_DigitalOutput::Init(pinNumber);

	state = RELAY_OFF;

	return 1;
}

// SetValue acts as an ON/OFF function, value == 0 will perform as OFF, any other value as ON
uint8_t EBF_Relay::SetValue(uint8_t value)
{
	if (value == 0) {
		state = RELAY_OFF;
	} else {
		state = RELAY_ON;
		value = 1;
	}

	return EBF_DigitalOutput::SetValue(value);
}

uint8_t EBF_Relay::On()
{
	state = RELAY_ON;

	return EBF_DigitalOutput::SetValue(1);
}

uint8_t EBF_Relay::Off()
{
	state = RELAY_OFF;

	return EBF_DigitalOutput::SetValue(0);
}

uint8_t EBF_Relay::Process()
{
	// Nothing to do
	// No polling needed
	pollIntervalMs = EBF_NO_POLLING;

	return 1;
}
