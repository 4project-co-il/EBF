#include "EBF_Timers.h"

uint8_t EBF_Timers::Init(uint8_t maxTimers, EBF_MessageQueue *pMsgQueue)
{
	this->maxTimers = maxTimers;
	this->pMsgQueue = pMsgQueue;

	// Init timers data
	this->timersData = (TimerData*)malloc(sizeof(TimerData) * this->maxTimers);
	memset(this->timersData, 0, sizeof(TimerData) * this->maxTimers);

	lastMicros = micros();

	return EBF_OK;
}

uint8_t EBF_Timers::InitTimer(uint8_t timerId, EBF_CallbackType callbackPtr, uint16_t milliSec)
{
	if (timerId >= this->maxTimers) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	TimerData &timer = this->timersData[timerId];
	timer.callbackPtr = callbackPtr;
	timer.microsLeft = 0;
	timer.milliSecTimeout = milliSec;

	return EBF_OK;
}

uint8_t EBF_Timers::StartTimer(uint8_t timerId)
{
	if (timerId >= this->maxTimers) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	TimerData &timer = this->timersData[timerId];

	if (timer.callbackPtr == NULL) {
		return EBF_NOT_INITIALIZED;
	}

	// Timer is already running
	if (timer.microsLeft != 0) {
		return EBF_RESOURCE_IS_IN_USE;
	}

	// Timer is not running, set ticks to be used
	timer.microsLeft = timer.milliSecTimeout*1000L;
	// Need a positive value to run, 0 means the timer is not running
	if (timer.microsLeft == 0) {
		timer.microsLeft = 1;
	}

	return EBF_OK;
}

uint8_t EBF_Timers::StartTimer(uint8_t timerId, uint16_t milliSec)
{
	uint8_t rc;

	rc = this->SetTimeout(timerId, milliSec);
	if (rc != EBF_OK) {
		return rc;
	}

	rc = this->StartTimer(timerId);
	if (rc != EBF_OK) {
		return rc;
	}

	return EBF_OK;
}

uint16_t EBF_Timers::Process()
{
	uint8_t i;
	uint16_t nextMillis = 0xFFFF;
	uint32_t microsPassed;
	unsigned long current;

	current = micros();
	microsPassed = current - lastMicros;
	lastMicros = current;

	for (i=0; i < this->maxTimers; i++) {
		TimerData &timer = this->timersData[i];

		// Non-active timer
		if (timer.microsLeft == 0) {
			continue;
		}

		if (microsPassed >= timer.microsLeft) {
			// Found timeout
			timer.microsLeft = 0;

			// Timers are processed from the user space, call the callback
			timer.callbackPtr();
		} else {
			// Not yet, reduce the passed ticks for next run calculation
			timer.microsLeft -= microsPassed;
		}

		if (timer.microsLeft > 0 && timer.microsLeft/1000L < nextMillis) {
			nextMillis = timer.microsLeft/1000L;
		}
	}

	return nextMillis;
}

uint8_t EBF_Timers::StopTimer(uint8_t timerId)
{
	if (timerId >= this->maxTimers) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	TimerData &timer = this->timersData[timerId];

	timer.microsLeft = 0;

	return EBF_OK;
}

uint8_t EBF_Timers::SetTimeout(uint8_t timerId, uint16_t milliSec)
{
	if (timerId >= this->maxTimers) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	TimerData &timer = this->timersData[timerId];

	timer.milliSecTimeout = milliSec;

	return EBF_OK;
}