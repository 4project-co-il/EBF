#include "EBF_DigitalInput.h"
#include "EBF_Logic.h"
#include "EBF_Core.h"

uint8_t EBF_DigitalInput::Init(
	uint8_t pinNumber,
	EBF_CallbackType callbackFunc,
	InterruptMode isrMode,
	bool internelPullup
)
{
	uint8_t rc;

	rc = EBF_HalInstance::Init(HAL_Type::DIGITAL_INPUT, pinNumber);
	if (rc != EBF_OK) {
		return rc;
	}

	this->pinNumber = pinNumber;
	this->callbackFunc = callbackFunc;
	this->isrMode = isrMode;

	if (internelPullup) {
		pinMode(pinNumber, INPUT_PULLUP);
	} else {
		pinMode(pinNumber, INPUT);
	}

	if (callbackFunc != NULL) {
		lastValue = digitalRead(pinNumber);

#ifdef EBF_USE_INTERRUPTS
		uint8_t interruptNumber = NOT_AN_INTERRUPT;

		if (EBF_Core::UseInterrupts()) {
			interruptNumber = digitalPinToInterrupt(pinNumber);
			if (interruptNumber != NOT_AN_INTERRUPT) {
				EBF_Logic *pLogic = EBF_Logic::GetInstance();

				// We can use interrupt for that pin
				pLogic->AttachInterrupt(interruptNumber, this, isrMode);

				// No need to poll
				pollIntervalMs = EBF_NO_POLLING;
			}
		}
#endif
	} else {
		// No callback. No need to poll in that case
		pollIntervalMs = EBF_NO_POLLING;
	}

	return EBF_OK;
}

void EBF_DigitalInput::SetPollInterval(uint16_t ms)
{
	// No polling needed if there is no callback to call
	if (callbackFunc == NULL) {
		pollIntervalMs = EBF_NO_POLLING;
	} else {
		pollIntervalMs = ms;
	}
}

uint8_t EBF_DigitalInput::Process()
{
	uint8_t currentValue;

	// Callback might not be set, nothing to do in that case
	if (callbackFunc == NULL) {
		return EBF_OK;
	}

	currentValue = digitalRead(pinNumber);

	switch (isrMode)
	{
	case InterruptMode::MODE_FALLING:
		if (lastValue == 1 && currentValue == 0) {
			lastValue = currentValue;
			ProcessCallback();
		} else {
			lastValue = currentValue;
		}
		break;

	case InterruptMode::MODE_RISING:
		if (lastValue == 0 && currentValue == 1) {
			lastValue = currentValue;
			ProcessCallback();
		} else {
			lastValue = currentValue;
		}
		break;

	case InterruptMode::MODE_CHANGE:
		if (currentValue != lastValue) {
			lastValue = currentValue;
			ProcessCallback();
		} else {
			lastValue = currentValue;
		}
		break;
	}

	return EBF_OK;
}

uint8_t EBF_DigitalInput::GetValue()
{
	return digitalRead(pinNumber);
}

uint8_t EBF_DigitalInput::GetLastValue()
{
	return lastValue;
}

