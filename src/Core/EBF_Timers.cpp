#include "EBF_Timers.h"
#include "EBF_Core.h"
#include "EBF_Logic.h"

uint8_t EBF_Timers::Init(uint8_t maxTimers, EBF_MessageQueue *pMsgQueue)
{
	this->maxTimers = maxTimers;
	this->pMsgQueue = pMsgQueue;

	// Init timers data
	this->timersData = (TimerData*)malloc(sizeof(TimerData) * this->maxTimers);
	memset(this->timersData, 0, sizeof(TimerData) * this->maxTimers);

	return EBF_OK;
}

uint8_t EBF_Timers::InitTimer(uint8_t timerId, EBF_CallbackType callbackPtr, uint16_t milliSec)
{
	if (timerId >= this->maxTimers) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	TimerData &timer = this->timersData[timerId];
	timer.callbackPtr = callbackPtr;
	timer.startingMicros = 0;
	timer.isRunning = 0;
	timer.milliSecTimeout = milliSec;

	return EBF_OK;
}

uint8_t EBF_Timers::StartTimer(uint8_t timerId)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	if (timerId >= this->maxTimers) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	TimerData &timer = this->timersData[timerId];

	if (timer.callbackPtr == NULL) {
		EBF_REPORT_ERROR(EBF_NOT_INITIALIZED);
		return EBF_NOT_INITIALIZED;
	}

	// Timer is already running
	if (timer.isRunning != 0) {
		EBF_REPORT_ERROR(EBF_RESOURCE_IS_IN_USE);
		return EBF_RESOURCE_IS_IN_USE;
	}

	// Should use EBF's micros, since it's updated during power save mode
	timer.startingMicros = pLogic->micros();

	timer.isRunning = 1;

	// Request recalculation of timeouts
	pLogic->Recalculate();

	return EBF_OK;
}

uint8_t EBF_Timers::StartTimer(uint8_t timerId, uint16_t milliSec)
{
	uint8_t rc;

	rc = this->SetTimeout(timerId, milliSec);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	rc = this->StartTimer(timerId);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

uint32_t EBF_Timers::Process()
{
	uint8_t i;
	uint32_t nextMillis = 0xFFFFFFFF;
	uint32_t millisPassed;
	uint32_t millisLeft;
	unsigned long currentMicros;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	// Should use EBF's micros, since it's updated during power save mode
	currentMicros = pLogic->micros();

	for (i=0; i < this->maxTimers; i++) {
		TimerData &timer = this->timersData[i];

		// Non-active timer
		if (timer.isRunning == 0) {
			continue;
		}

		millisPassed = (currentMicros - timer.startingMicros)/1000L;

		if (millisPassed >= timer.milliSecTimeout) {
			// Timer expired
			timer.isRunning = 0;

			// Timers are processed from the user space, call the callback
			timer.callbackPtr();

			// Continue to next timer, so we won't consider it for calculations
			continue;
		}

		millisLeft = timer.milliSecTimeout - millisPassed;
		if (millisLeft < nextMillis) {
			nextMillis = millisLeft;
		}
	}

	return nextMillis;
}

uint8_t EBF_Timers::StopTimer(uint8_t timerId)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	if (timerId >= this->maxTimers) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	TimerData &timer = this->timersData[timerId];

	timer.isRunning = 0;
	timer.startingMicros = 0;

	// Request recalculation of timeouts
	pLogic->Recalculate();

	return EBF_OK;
}

uint8_t EBF_Timers::SetTimeout(uint8_t timerId, uint16_t milliSec)
{
	if (timerId >= this->maxTimers) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	TimerData &timer = this->timersData[timerId];

	timer.milliSecTimeout = milliSec;

	return EBF_OK;
}

uint8_t EBF_Timers::RestartTimer(uint8_t timerId)
{
	uint8_t rc;

	rc = StopTimer(timerId);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	rc = StartTimer(timerId);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}
