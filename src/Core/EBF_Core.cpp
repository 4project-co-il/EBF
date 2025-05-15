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

uint8_t EBF_Core::Init(uint8_t maxTimers, uint8_t queueSize)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->Init(maxTimers, queueSize);
}

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

uint8_t EBF_Core::RestartTimer(uint8_t timerId)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->RestartTimer(timerId);
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
// Returns 1 when HAL's Process() function (and any HAL callback()) is run from the interrupt (ISR mode)
uint8_t EBF_Core::InInterrupt()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->IsRunFromIsr();
}

uint32_t EBF_Core::GetInterruptHint()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->GetInterruptHint();
}

uint8_t EBF_Core::PostponeInterrupt(EBF_DigitalInput &digitalInput)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->PostponeInterrupt(&digitalInput);
}

uint8_t EBF_Core::GetNumberOfMessages()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->GetNumberOfMessages();
}

uint8_t EBF_Core::GetMaxNumberOfMessages()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	return pLogic->GetMaxNumberOfMessages();
}

#endif

#ifdef EBF_SLEEP_IMPLEMENTATION
void EBF_Core::SetSleepMode(EBF_SleepMode mode)
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	pLogic->SetSleepMode(mode);
}
#endif
