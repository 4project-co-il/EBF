#include "PnP_ButtonInterface.h"

extern void EBF_EmptyCallback();

PnP_ButtonInterface::PnP_ButtonInterface()
{
	this->onPressCallback = EBF_EmptyCallback;
	this->onLongPressCallback = EBF_EmptyCallback;
	this->onReleaseCallback = EBF_EmptyCallback;

	// Long press will be 3 sec by default
	longPressTime = 3000;

	state = BUTTON_STATE_RELEASED;
}

PnP_ButtonInterface::ButtonEvent PnP_ButtonInterface::ProcessInput(uint8_t value)
{
	// Value "0" is considered as pressed
	// Value "1" is considered as released

	switch(this->state) {
		case BUTTON_STATE_PRESSED:
			if (value == 1) {	// Button was released
				this->state = BUTTON_STATE_RELEASED;
				pInputProvider->SetPollingInterval_IIP(EBF_NO_POLLING);

				return BUTTON_EVENT_RELEASE;
			}
		break;

		case BUTTON_STATE_RELEASED:
			if (value == 0) {	// Button was pressed
				// Wait for long-press timeout
				this->state = BUTTON_STATE_WAITING_FOR_LONG_PRESS;

				longPressStart = pInputProvider->millis_IIP();

				// Process() function is called every polling interval or on change
				// We will poll every 100mSec till the long press will be detected
				if (longPressTime < pInputProvider->GetPollingInterval_IIP()) {
					pInputProvider->SetPollingInterval_IIP(100);
				}

				return BUTTON_EVENT_PRESS;
			}
		break;

		case BUTTON_STATE_WAITING_FOR_LONG_PRESS:
			if (value == 1) {	// Button was released
				this->state = BUTTON_STATE_RELEASED;

				pInputProvider->SetPollingInterval_IIP(EBF_NO_POLLING);

				return BUTTON_EVENT_RELEASE;
			} else {
				// Wait for long-press timeout
				if((pInputProvider->millis_IIP() - longPressStart) >= longPressTime) {
					// It's time...
					this->state = BUTTON_STATE_PRESSED;

					// Polling is no more needed
					pInputProvider->SetPollingInterval_IIP(EBF_NO_POLLING);

					return BUTTON_EVENT_LONG_PRESS;
				}
			}
		break;
	}

	// Nothing happened
	return BUTTON_EVENT_NONE;
}

void PnP_ButtonInterface::ExecuteCallback() {
	ExecuteCallbackEx(pInputProvider->GetLastValue(providerIndex));
}

void PnP_ButtonInterface::ExecuteCallbackEx(uint8_t value) {
	ButtonEvent event = ProcessInput(value);

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

uint8_t PnP_ButtonInterface::IsProcessingNeeded() {
	// Button still need the Process calls from PnP module class
	if (state == BUTTON_STATE_WAITING_FOR_LONG_PRESS) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t PnP_ButtonInterface::Process() {
	ExecuteCallbackEx(pInputProvider->GetValue(providerIndex));

	return EBF_OK;
}
