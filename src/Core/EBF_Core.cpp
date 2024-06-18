#include "EBF.h"
#include "EBF_Logic.h"
#include "EBF_HalInstance.h"

// EBF_Core is a wrapper class to hide the usage of the singleton from the user
// and make the APIs a bit easier to use

uint8_t EBF_Core::Init()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->Init(0, 0);
}

uint8_t EBF_Core::Init(uint8_t maxTimers)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->Init(maxTimers, 0);
}

#ifdef EBF_USE_INTERRUPTS
uint8_t EBF_Core::Init(uint8_t maxTimers, uint8_t queueSize)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->Init(maxTimers, queueSize);
}
#endif

uint8_t EBF_Core::AddHalInstance(EBF_HalInstance &instance)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->AddHalInstance(instance);
}

uint8_t EBF_Core::Process()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->Process();
}

EBF_HalInstance *EBF_Core::GetHalInstance(EBF_HalInstance::HAL_Type type, uint8_t id)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->GetHalInstance(type, id);
}

// Timers access functions
uint8_t EBF_Core::InitTimer(uint8_t timerId, EBF_CallbackType callbackPtr, uint16_t milliSec)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->InitTimer(timerId, callbackPtr, milliSec);
}

uint8_t EBF_Core::SetTimeout(uint8_t timerId, uint16_t milliSec)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->SetTimeout(timerId, milliSec);
}

uint8_t EBF_Core::StartTimer(uint8_t timerId)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->StartTimer(timerId);
}

uint8_t EBF_Core::StartTimer(uint8_t timerId, uint16_t milliSec)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->StartTimer(timerId, milliSec);
}

uint8_t EBF_Core::StopTimer(uint8_t timerId)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->StopTimer(timerId);
}

unsigned long EBF_Core::micros()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->micros();
}

unsigned long EBF_Core::millis()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->millis();
}

#ifdef EBF_USE_INTERRUPTS
uint8_t EBF_Core::ProcessInterrupt(EBF_DigitalInput &digitalInput)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->ProcessInterrupt(&digitalInput);
}

uint8_t EBF_Core::InInterrupt()
{
	// Arduino based on ATMega328 doesn't have any register that shows which interrupt is currently running
	// But the CPU clears the Global Interrupt Enable (bit7) of the AVR Status Register (SREG) on
	// ISR entry and sets it back to 1 on ISR exit to allow next interrupt execution
	// That method might be ok to use if interrupts are not disabled manually and the EBF HAL functions are
	// called manually. Should be ok for regular usage.
	if ((SREG & bit(7)) == 0) {
		return 1;
	} else {
		return 0;
	}
}
#endif

#ifdef EBF_SLEEP_IMPLEMENTATION
void EBF_Core::SetSleepMode(EBF_SleepMode mode)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	pLogic->SetSleepMode(mode);
}
#endif
