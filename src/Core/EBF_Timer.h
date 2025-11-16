#ifndef __EBF_TIMER_H__
#define __EBF_TIMER_H__

#include <Arduino.h>
#include "EBF_Global.h"

class EBF_Timer {
	public:
		friend class EBF_Logic;

	private:
		EBF_DEBUG_MODULE_NAME("EBF_Timer");

	public:
		EBF_Timer();

		~EBF_Timer() {
			this->numberOfTimers--;
		}

	public:
		uint8_t Init(EBF_CallbackType callbackPtr, uint16_t milliSec);
		uint8_t SetTimeout(uint16_t milliSec);
		uint8_t Start();
		uint8_t Start(uint16_t milliSec);
		uint8_t Stop();
		uint8_t Restart();

	protected:
		// Returns number of milliSeconds till the next timer, or 0xFFFFFFFF if there are no timers
		uint32_t Process();
		static uint8_t GetNumberOfTimers() { return EBF_Timer::numberOfTimers; }

	private:
		static uint8_t numberOfTimers;

	private:
		EBF_CallbackType callbackPtr;
		unsigned long startingMicros;
		uint16_t milliSecTimeout;
		uint8_t isRunning;
};

#endif
