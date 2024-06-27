#ifndef __EBF_TIMERS_H__
#define __EBF_TIMERS_H__

#include <Arduino.h>
#include "EBF_Global.h"
#include "EBF_MessageQueue.h"

class EBF_Timers {
	public:
		uint8_t Init(uint8_t maxTimers, EBF_MessageQueue *pMsgQueue);
		uint8_t InitTimer(uint8_t timerId, EBF_CallbackType callbackPtr, uint16_t milliSec);
		uint8_t SetTimeout(uint8_t timerId, uint16_t milliSec);
		uint8_t StartTimer(uint8_t timerId);
		uint8_t StartTimer(uint8_t timerId, uint16_t milliSec);
		uint8_t StopTimer(uint8_t timerId);

		// Returns number of milliSeconds till the next timer, or 0xFFFFFFFF if there are no timers
		uint32_t Process(unsigned long current);

	private:
		typedef struct
		{
			EBF_CallbackType callbackPtr;
			uint32_t microsLeft;
			uint16_t milliSecTimeout;
		} TimerData;

	private:
		EBF_MessageQueue *pMsgQueue;
		uint8_t maxTimers;
		TimerData* timersData;
		unsigned long lastMicros;
};

#endif
