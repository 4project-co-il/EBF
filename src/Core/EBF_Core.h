#ifndef __EBF_CORE_H__
#define __EBF_CORE_H__

#include <Arduino.h>

#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_HalInstance.h"
#include "EBF_Timers.h"
#include "EBF_DigitalInput.h"

// EBF_Logic is a wrapper class to hide the usage of the singleton from the user
// and make the APIs a bit easier for usage
class EBF_Core {
	public:
		uint8_t Init();
		uint8_t Init(uint8_t maxTimers);
#ifdef EBF_USE_INTERRUPTS
		uint8_t Init(uint8_t maxTimers, uint8_t queueSize);
#endif
		uint8_t AddHalInstance(EBF_HalInstance &instance);
		uint8_t Process();

		EBF_HalInstance *GetHalInstance(EBF_HalInstance::HAL_Type type, uint8_t id);

	public:
		// Timer functions
		uint8_t InitTimer(uint8_t timerId, EBF_CallbackType callbackPtr, uint16_t milliSec);
		uint8_t SetTimeout(uint8_t timerId, uint16_t milliSec);
		uint8_t StartTimer(uint8_t timerId);
		uint8_t StartTimer(uint8_t timerId, uint16_t milliSec);

		uint8_t StopTimer(uint8_t timerId);

#ifdef EBF_USE_INTERRUPTS
		// Message queue functions
		uint8_t ProcessInterrupt(EBF_DigitalInput &digitalInput);
		uint8_t InInterrupt();
		static uint8_t UseInterrupts() { return 1; }
#else
		static uint8_t UseInterrupts() { return 0; }
#endif
};

#endif
