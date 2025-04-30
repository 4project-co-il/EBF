#include "EBF_ButtonLogic.h"

extern void EBF_EmptyCallback();

EBF_ButtonLogic::EBF_ButtonLogic()
{
	this->state = EBF_ButtonLogic::BUTTON_LOGICAL_1;

	onPressCallback = EBF_EmptyCallback;
	onLongPressCallback = EBF_EmptyCallback;
	onReleaseCallback = EBF_EmptyCallback;

	// Long press will be 3 sec by default
	longPressTime = 3000;
}

uint8_t EBF_ButtonLogic::Init()
{
	return EBF_OK;
}

void EBF_ButtonLogic::ExecuteCallback(ButtonState state)
{
	switch(state) {
		case BUTTON_LOGICAL_0:
			onPressCallback();
			break;

		case BUTTON_LOGICAL_1:
			onReleaseCallback();
			break;

		case BUTTON_WAITING_FOR_LONG_PRESS:
			onLongPressCallback();
			break;
	}
}

uint8_t EBF_ButtonLogic::Process(EBF_HalInstance* pHal)
{
	// When in waiting for long press state, wait till the long press time
	if (this->state == BUTTON_WAITING_FOR_LONG_PRESS &&
		((pHal->millis() - longPressStart) >= longPressTime)) {
			// and fire the callback
			onLongPressCallback();

			// change to the regular "0" state after the callback
			this->state = BUTTON_LOGICAL_1;

			// disable the polling since PnP is based on interrupts
			// Should be done after the state change, since PnP_Module_* that uses
			// the EBF_ButtonLogic class overrides the SetPollingInterval and checks the state of each button
			pHal->SetPollingInterval(EBF_NO_POLLING);
	}

	return EBF_OK;
}

uint8_t EBF_ButtonLogic::Process(uint8_t value, EBF_HalInstance* pHal)
{
	// Change state before calling the callbacks
	if (value == 0) {
		this->state = BUTTON_LOGICAL_0;
	} else {
		this->state = BUTTON_LOGICAL_1;
	}

	// Logical "0"/OFF (connected to GND) is used as a press indication
	if (this->state == EBF_ButtonLogic::BUTTON_LOGICAL_1) {
		pHal->SetPollingInterval(EBF_NO_POLLING);

		onReleaseCallback();
	} else {
		onPressCallback();

		// Start long press delay
		this->state = BUTTON_WAITING_FOR_LONG_PRESS;
		longPressStart = pHal->millis();

		// Process() function is called every polling interval or change
		// We will poll every 100mSec till the long press will be detected
		if (longPressTime < pHal->GetPollingInterval()) {
			pHal->SetPollingInterval(100);
		}
	}

	return this->Process(pHal);
}
