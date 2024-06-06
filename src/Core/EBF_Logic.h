#ifndef __EBF_LOGIC_H__
#define __EBF_LOGIC_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif
#include "EBF_Global.h"
#include "EBF_Timers.h"
#include "EBF_MessageQueue.h"
#include "EBF_HalInstance.h"

class EBF_Logic {
	public:
		EBF_Logic();

		uint8_t Init(uint8_t maxTimers, uint8_t queueSize);
		uint8_t AddHalInstance(EBF_HalInstance &instance);
		uint8_t Process();

		static EBF_Logic *GetInstance();
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
		uint8_t ProcessInterrupt(EBF_HalInstance *pHalInstance);

		// Message queue debug functions
		uint8_t GetNumberOfMessages() { return msgQueue.GetMessagesNumber(); }
		uint8_t GetMaxNumberOfMessages() { return msgQueue.GetMaxMessagesNumber(); }
#endif

	private:
		static EBF_Logic *pStaticInstance;
		EBF_Timers timers;
#ifdef EBF_USE_INTERRUPTS
		EBF_MessageQueue msgQueue;
#endif

		EBF_HalInstance **pHalInstances;
		uint8_t halIndex;
};

#endif
