#include "EBF_ButtonLogic.h"

extern void EBF_EmptyCallback();

EBF_ButtonLogic::EBF_ButtonLogic()
{
	this->state = EBF_ButtonLogic::BUTTON_STATE_RELEASED;

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

void EBF_ButtonLogic::ExecuteCallback(EBF_ButtonLogic::ButtonEvent event)
{
	switch(event) {
		case BUTTON_EVENT_NONE:
			// Nothing should be done
			break;

		case BUTTON_EVENT_PRESS:
			onPressCallback();
			break;

		case BUTTON_EVENT_RELEASE:
			onReleaseCallback();
			break;

		case BUTTON_EVENT_LONG_PRESS:
			onLongPressCallback();
			break;
	}
}

EBF_ButtonLogic::ButtonEvent EBF_ButtonLogic::Process(uint8_t value, EBF_HalInstance* pHal)
{
	// Value "0" is considered as pressed
	// Value "1" is considered as released

	switch(this->state) {
		case BUTTON_STATE_PRESSED:
			if (value == 1) {	// Button was released
				this->state = BUTTON_STATE_RELEASED;
				pHal->SetPollingInterval(EBF_NO_POLLING);

				return BUTTON_EVENT_RELEASE;
			}
		break;

		case BUTTON_STATE_RELEASED:
			if (value == 0) {	// Button was pressed
				// Wait for long-press timeout
				this->state = BUTTON_STATE_WAITING_FOR_LONG_PRESS;

				longPressStart = pHal->millis();

				// Process() function is called every polling interval or on change
				// We will poll every 100mSec till the long press will be detected
				if (longPressTime < pHal->GetPollingInterval()) {
					pHal->SetPollingInterval(100);
				}

				return BUTTON_EVENT_PRESS;
			}
		break;

		case BUTTON_STATE_WAITING_FOR_LONG_PRESS:
			if (value == 1) {	// Button was released
				this->state = BUTTON_STATE_RELEASED;
				pHal->SetPollingInterval(EBF_NO_POLLING);

				return BUTTON_EVENT_RELEASE;
			} else {
				// Wait for long-press timeout
				if((pHal->millis() - longPressStart) >= longPressTime) {
					// It's time...
					this->state = BUTTON_STATE_PRESSED;

					// Polling is no more needed
					pHal->SetPollingInterval(EBF_NO_POLLING);

					return BUTTON_EVENT_LONG_PRESS;
				}
			}
		break;
	}

	// Nothing happened
	return BUTTON_EVENT_NONE;
}
