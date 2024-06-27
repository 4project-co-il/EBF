#include "EBF_Switch.h"

extern void EBF_EmptyCallback();

uint8_t EBF_Switch::Init(uint8_t pinNumber, bool internelPullup)
{
	uint8_t rc;

	// Use empty callback function to allow Process and ProcessCallback calls
	rc = EBF_DigitalInput::Init(pinNumber, EBF_EmptyCallback, EBF_DigitalInput::InterruptMode::MODE_CHANGE, internelPullup);
	if (rc != EBF_OK) {
		return rc;
	}

	if (EBF_DigitalInput::GetValue()) {
		state = SWITCH_ON;
	} else {
		state = SWITCH_OFF;
	}

	lastState = state;

	// Default debounce
	debounceTime = 10;	// milliSec

	onChangeCallback = EBF_EmptyCallback;

	return EBF_OK;
}

// Setting polling interval in milli-seconds
void EBF_Switch::SetPollInterval(uint32_t ms) {
	pollIntervalMs = ms;
	savedPollingInterval = pollIntervalMs;
}

// Called when digital input change is detected
void EBF_Switch::ProcessCallback()
{
	SwitchState readState;

	if (GetLastValue() == 0) {
		readState = SWITCH_OFF;
	} else {
		readState = SWITCH_ON;
	}

	// If the switch changed, due to noise or pressing
	if (readState != lastState) {
		// reset the debouncing timer
		debounceStart = millis();
		lastState = readState;

		// The rest of the processing will be done in Process() function,
		// since it's called every polling interval or change
		// We will set the polling interval to the debounce value if it's shorter
		if (debounceTime < pollIntervalMs) {
			pollIntervalMs = debounceTime;
		}
	}
}

uint8_t EBF_Switch::Process()
{
	uint8_t rc;

	rc = EBF_DigitalInput::Process();
	if (rc != EBF_OK) {
		return rc;
	}

	if ((millis() - debounceStart) > debounceTime) {
		if (state != lastState) {
			state = lastState;
			// restore polling interval to its original value
			pollIntervalMs = savedPollingInterval;

			ProcessSwitchCallback();
		}
	}

	return EBF_OK;
}
