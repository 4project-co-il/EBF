#include "EBF_Button.h"

extern void EBF_EmptyCallback();

uint8_t EBF_Button::Init(uint8_t pinNumber, bool internelPullup)
{
	uint8_t rc;

	rc = EBF_Switch::Init(pinNumber, internelPullup);
	if (rc != EBF_OK) {
		return rc;
	}

	this->state = (EBF_Button::ButtonState)EBF_Switch::state;

	onPressCallback = EBF_EmptyCallback;
	onLongPressCallback = EBF_EmptyCallback;
	onReleaseCallback = EBF_EmptyCallback;

	// Long press will be 3 sec by default
	longPressTime = 3000;

	return EBF_OK;
}

// Called when the switch changes its state after the debounce
void EBF_Button::ProcessSwitchCallback()
{
	// Change state before calling the callbacks
	if (EBF_Switch::GetState() == EBF_Switch::SWITCH_OFF) {
		this->state = BUTTON_OFF;
	} else {
		this->state = BUTTON_ON;
	}

	EBF_Switch::ProcessSwitchCallback();

	if (this->state == EBF_Button::BUTTON_OFF) {
		// Restore polling interval
		RestorePollInterval();

		onReleaseCallback();
	} else {
		onPressCallback();

		// Start long press delay
		this->state = BUTTON_WAITING_FOR_LONG_PRESS;
		longPressStart = millis();

		// The rest of the processing will be done in Process() function,
		// since it's called every polling interval or change
		// We will set the polling interval to the long press value if it's shorter
		if (longPressTime < pollIntervalMs) {
			pollIntervalMs = longPressTime;
		}
	}
}

uint8_t EBF_Button::Process()
{
	uint8_t rc;

	rc = EBF_Switch::Process();
	if (rc != EBF_OK) {
		return rc;
	}

	// When in waiting for long press state, wait till the long press time
	if (this->state == BUTTON_WAITING_FOR_LONG_PRESS &&
		(millis() - longPressStart) > longPressTime) {
		// change to the regular ON state
		this->state = BUTTON_ON;

		// Restore polling interval
		RestorePollInterval();

		// and fire the callback
		onLongPressCallback();
	}

	return EBF_OK;
}
