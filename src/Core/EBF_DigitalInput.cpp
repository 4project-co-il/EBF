#include "EBF_DigitalInput.h"
#include "EBF_Core.h"

uint8_t EBF_DigitalInput::Init(
	uint8_t pinNumber,
	EBF_CallbackType callbackFunc,
	InterruptMode isrMode,
	bool internelPullup
)
{
	EBF_HalInstance::Init(HAL_Type::DIGITAL_INPUT, pinNumber);

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

		if (EBF_Core::UseInterrupts()) {
			if (digitalPinToInterrupt(pinNumber) != NOT_AN_INTERRUPT) {
				// We can use interrupt for that pin
				attachInterrupt(digitalPinToInterrupt(pinNumber), callbackFunc, isrMode);

				// No need to poll
				pollIntervalMs = EBF_NO_POLLING;
			}
		}
	} else {
		// No callback. No need to poll in that case
		pollIntervalMs = EBF_NO_POLLING;
	}
	return 1;
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
		return 0;
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

	return 1;
}

uint8_t EBF_DigitalInput::GetValue()
{
	return digitalRead(pinNumber);
}

uint8_t EBF_DigitalInput::GetLastValue()
{
	return lastValue;
}
