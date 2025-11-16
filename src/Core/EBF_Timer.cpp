#include "EBF_Core.h"
#include "EBF_Logic.h"

uint8_t EBF_Timer::numberOfTimers = 0;

EBF_Timer::EBF_Timer() {
	this->startingMicros = 0;
	this->isRunning = 0;
	this->callbackPtr = NULL;

	this->numberOfTimers++;
}

uint8_t EBF_Timer::Init(EBF_CallbackType callbackPtr, uint16_t milliSec)
{
	uint8_t rc;

	this->callbackPtr = callbackPtr;
	this->milliSecTimeout = milliSec;

	rc = EBF_Logic::GetInstance()->AddTimer(*this);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

uint8_t EBF_Timer::Start()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	if (this->callbackPtr == NULL) {
		EBF_REPORT_ERROR(EBF_NOT_INITIALIZED);
		return EBF_NOT_INITIALIZED;
	}

	// Timer is already running
	if (this->isRunning != 0) {
		EBF_REPORT_ERROR(EBF_RESOURCE_IS_IN_USE);
		return EBF_RESOURCE_IS_IN_USE;
	}

	// Should use EBF's micros, since it's updated during power save mode
	this->startingMicros = pLogic->micros();

	this->isRunning = 1;

	// Request recalculation of timeouts
	pLogic->Recalculate();

	return EBF_OK;
}

uint8_t EBF_Timer::Start(uint16_t milliSec)
{
	uint8_t rc;

	rc = this->SetTimeout(milliSec);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	rc = this->Start();
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

uint8_t EBF_Timer::Stop()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	this->isRunning = 0;
	this->startingMicros = 0;

	// Request recalculation of timeouts
	pLogic->Recalculate();

	return EBF_OK;
}

uint8_t EBF_Timer::Restart()
{
	uint8_t rc;

	rc = this->Stop();
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	rc = this->Start();
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

uint8_t EBF_Timer::SetTimeout(uint16_t milliSec)
{
	this->milliSecTimeout = milliSec;

	return EBF_OK;
}

uint32_t EBF_Timer::Process()
{
	uint32_t nextMillis = EBF_NO_POLLING;
	uint32_t millisPassed;
	uint32_t millisLeft;
	unsigned long currentMicros;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	// Timer is not running
	if (this->isRunning == 0) {
		return EBF_NO_POLLING;
	}

	// Should use EBF's micros, since it's updated during power save mode
	currentMicros = pLogic->micros();

	millisPassed = (currentMicros - this->startingMicros)/1000L;

	if (millisPassed >= this->milliSecTimeout) {
		// Timer expired
		this->isRunning = 0;

		// Timers are processed from the user space, call the callback
		this->callbackPtr();

		// No more processing is needed
		return EBF_NO_POLLING;
	}

	millisLeft = this->milliSecTimeout - millisPassed;
	if (millisLeft < nextMillis) {
		nextMillis = millisLeft;
	}

	return nextMillis;
}
