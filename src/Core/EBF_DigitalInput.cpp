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
	this->interruptAttached = 0;

	if (internelPullup) {
		pinMode(pinNumber, INPUT_PULLUP);
	} else {
		pinMode(pinNumber, INPUT);
	}

	if (callbackFunc != NULL) {
		lastValue = digitalRead(pinNumber);
	} else {
		// No callback. No need to poll in that case
		pollIntervalMs = EBF_NO_POLLING;
	}

	return EBF_OK;
}

uint8_t EBF_DigitalInput::AttachInterrupt()
{
#ifdef EBF_USE_INTERRUPTS
		uint8_t interruptNumber = NOT_AN_INTERRUPT;

		if (EBF_Core::UseInterrupts()) {
			interruptNumber = digitalPinToInterrupt(pinNumber);
			if (interruptNumber != NOT_AN_INTERRUPT) {
				EBF_Logic *pLogic = EBF_Logic::GetInstance();

				// We can use interrupt for that pin
				pLogic->AttachInterrupt(interruptNumber, this, isrMode);
				interruptAttached = 1;

				// No need to poll
				pollIntervalMs = EBF_NO_POLLING;
			}

			return EBF_OK;
		}
#endif

	return EBF_INVALID_STATE;
}

void EBF_DigitalInput::SetPollInterval(uint32_t ms)
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
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	// Callback might not be set, nothing to do in that case
	if (callbackFunc == NULL) {
		return EBF_OK;
	}

	// Process postponed call
	if (pLogic->IsPostInterruptProcessing()) {
		callbackFunc();
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

#if defined(ARDUINO_ARCH_SAMD)
	case InterruptMode::MODE_LOW:
		if (currentValue == 0) {
			lastValue = currentValue;
			ProcessCallback();
		} else {
			lastValue = currentValue;
		}
		break;

	case InterruptMode::MODE_HIGH:
		if (currentValue == 1) {
			lastValue = currentValue;
			ProcessCallback();
		} else {
			lastValue = currentValue;
		}
		break;
#endif

	}

	return EBF_OK;
}

void EBF_DigitalInput::ProcessInterrupt()
{
	// This function is called from ISR.
	// For DigitalInput, call the callback function to pass the control to user
	// The user can call EBF.PostponeInterrupt() function to pass the control
	// back to the EBF, which will call user callback again from normal run
	Process();

	// The interrupt register is cleared by the arduino code
}

uint8_t EBF_DigitalInput::GetValue()
{
	return digitalRead(pinNumber);
}

uint8_t EBF_DigitalInput::GetLastValue()
{
	return lastValue;
}

void EBF_DigitalInput::ProcessCallback()
{
#ifdef EBF_DIRECT_CALL_FROM_ISR
	callbackFunc();
#else
	if (interruptAttached) {
		EBF_Logic *pLogic = EBF_Logic::GetInstance();
		pLogic->PostponeInterrupt(this);
	} else {
		callbackFunc();
	}
#endif
}
