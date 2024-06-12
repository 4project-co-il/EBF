#include "EBF_Logic.h"
#include "EBF_HalInstance.h"

/// EBF_Logic implementation
EBF_Logic *EBF_Logic::pStaticInstance = new EBF_Logic();

EBF_Logic::EBF_Logic()
{
	pHalInstances = NULL;
	halIndex = 0;
}

EBF_Logic *EBF_Logic::GetInstance()
{
	return pStaticInstance;
}

uint8_t EBF_Logic::Init(uint8_t maxTimers, uint8_t queueSize)
{
	uint8_t rc;

#ifdef EBF_USE_INTERRUPTS
	rc = this->msgQueue.Init(queueSize);
	if (rc != EBF_NO_ERROR) {
		return rc;
	}

	rc = this->timers.Init(maxTimers, &msgQueue);
#else
	rc = this->timers.Init(maxTimers, NULL);
#endif
	if (rc != EBF_OK) {
		return rc;
	}

	if (EBF_HalInstance::GetNumberOfInstances() > 0) {
		pHalInstances = (EBF_HalInstance**)malloc(sizeof(EBF_HalInstance*) * EBF_HalInstance::GetNumberOfInstances());

		if (pHalInstances == NULL) {
			return EBF_NOT_ENOUGH_MEMORY;
		}
	}

	return EBF_OK;
}

uint8_t EBF_Logic::AddHalInstance(EBF_HalInstance &instance)
{
	if (halIndex >= EBF_HalInstance::GetNumberOfInstances()) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	pHalInstances[halIndex] = &instance;
	halIndex++;

	return EBF_OK;
}

uint8_t EBF_Logic::Process()
{
	uint8_t i;
	uint16_t delayWanted = EBF_NO_POLLING;
	EBF_HalInstance *pHal;
	unsigned long ms;

	// Start counting time before the execution of the callbacks, that might take some time
	uint32_t start = micros();

	// Process timers
	delayWanted = timers.Process();

	// Process HALs
	for (i=0; i<halIndex; i++) {
		pHal = pHalInstances[i];

		if (pHal->GetPollingInterval() == EBF_NO_POLLING) {
			continue;
		}

		ms = millis();

		if (ms - pHal->GetLastPollMillis() > pHal->GetPollingInterval()) {
			pHal->SetLastPollMillis(ms);

			pHal->Process();

			if (pHal->GetPollingInterval() < delayWanted) {
				delayWanted = pHal->GetPollingInterval();
			}
		} else {
			uint16_t pollWanted = pHal->GetPollingInterval() - (ms - pHal->GetLastPollMillis());

			if (pollWanted < delayWanted) {
				delayWanted = pollWanted;
			}
		}
	}

	// Should give other things some CPU time
	if (delayWanted == 0) {
		delayWanted = 1;
	}

	//Serial.print("Wanted delay: ");
	//Serial.println(delayWanted);

	// TODO: Try to power down the CPU for some time...

	// Implementing our own delay loop, so we could check the messages from interrupts in the loop
	while (delayWanted > 0) {
		yield();

#ifdef EBF_USE_INTERRUPTS
		EBF_MessageQueue::MessageEntry msg;
		uint16_t rc;
		EBF_HalInstance *pHalInstance;

		// Messages are used to pass information from interrupts to normal run
		if (msgQueue.GetMessagesNumber() > 0) {
			rc = msgQueue.GetMessage(msg);

			if (rc > 0) {
				pHalInstance = (EBF_HalInstance*)msg.param1;

				pHalInstance->Process();
			}
		}
#endif
		while ( delayWanted > 0 && (micros() - start) >= 1000) {
			delayWanted--;
			start += 1000;
		}
	}

	return EBF_OK;
}

EBF_HalInstance *EBF_Logic::GetHalInstance(EBF_HalInstance::HAL_Type type, uint8_t id)
{
	uint8_t i;

	for (i=0; i<halIndex; i++) {
		if (pHalInstances[i]->GetType() == type && pHalInstances[i]->GetId() == id) {
			return pHalInstances[i];
		}
	}

	return NULL;
}

// Timers access functions
uint8_t EBF_Logic::InitTimer(uint8_t timerId, EBF_CallbackType callbackPtr, uint16_t milliSec)
{
	return timers.InitTimer(timerId, callbackPtr, milliSec);
}

uint8_t EBF_Logic::SetTimeout(uint8_t timerId, uint16_t milliSec)
{
	return timers.SetTimeout(timerId, milliSec);
}

uint8_t EBF_Logic::StartTimer(uint8_t timerId)
{
	return timers.StartTimer(timerId);
}

uint8_t EBF_Logic::StartTimer(uint8_t timerId, uint16_t milliSec)
{
	return timers.StartTimer(timerId, milliSec);
}

uint8_t EBF_Logic::StopTimer(uint8_t timerId)
{
	return timers.StopTimer(timerId);
}

#ifdef EBF_USE_INTERRUPTS
uint8_t EBF_Logic::ProcessInterrupt(EBF_HalInstance *pHalInstance)
{
	EBF_MessageQueue::MessageEntry msg;

	memset(&msg, 0, sizeof(EBF_MessageQueue::MessageEntry));
	msg.param1 = (uint32_t)pHalInstance;

	return msgQueue.AddMessage(msg);
}
#endif