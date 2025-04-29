#ifndef __EBF_LOGIC_H__
#define __EBF_LOGIC_H__

#include <Arduino.h>
#include <wiring_private.h>		// For EXTERNAL_NUM_INTERRUPTS
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
		inline uint8_t InitTimer(uint8_t timerId, EBF_CallbackType callbackPtr, uint16_t milliSec)
		{
			return timers.InitTimer(timerId, callbackPtr, milliSec);
		}

		inline uint8_t SetTimeout(uint8_t timerId, uint16_t milliSec)
		{
			return timers.SetTimeout(timerId, milliSec);
		}

		inline uint8_t StartTimer(uint8_t timerId)
		{
			return timers.StartTimer(timerId);
		}

		inline uint8_t StartTimer(uint8_t timerId, uint16_t milliSec)
		{
			return timers.StartTimer(timerId, milliSec);
		}

		inline uint8_t StopTimer(uint8_t timerId)
		{
			return timers.StopTimer(timerId);
		}

#ifdef EBF_SLEEP_IMPLEMENTATION
		inline unsigned long micros() { return ::micros() + microsAddition; }
		inline unsigned long millis() { return ::millis() + microsAddition/1000; }
#else
		inline unsigned long micros() { return ::micros(); }
		inline unsigned long millis() { return ::millis(); }
#endif

#ifdef EBF_USE_INTERRUPTS
		// Message queue functions
		uint8_t IsRunFromIsr() { return isRunFromISR; }
		uint32_t GetInterruptHint() { return interruptHint; }
		uint8_t IsPostInterruptProcessing() { return isPostInterruptProcessing; }
		uint8_t AttachInterrupt(uint8_t interruptNumber, EBF_HalInstance *pHalInstance, uint8_t mode);
		uint8_t AttachInterrupt(uint8_t interruptNumber, EBF_HalInstance *pHalInstance, uint8_t mode, uint32_t hint);
		uint8_t ProcessInterrupt(EBF_HalInstance *pHalInstance);
		uint8_t ProcessInterrupt(EBF_HalInstance *pHalInstance, uint32_t param1);
		uint32_t GetLastMessageParam1() { return lastMessage.param1; }
		void HandleIsr(uint8_t interruptNumber);

		// Message queue debug functions
		uint8_t GetNumberOfMessages() { return msgQueue.GetMessagesNumber(); }
		uint8_t GetMaxNumberOfMessages() { return msgQueue.GetMaxMessagesNumber(); }
#else
		uint8_t IsRunFromIsr() { return 0; }
		uint32_t GetInterruptHint() { return 0; }
		uint8_t IsPostInterruptProcessing() { return 0; }
		uint32_t GetLastMessageParam1() { return 0; }
		uint8_t AttachInterrupt(uint8_t interruptNumber, EBF_HalInstance *pHalInstance, uint8_t mode) { return EBF_INVALID_STATE; }
		uint8_t AttachInterrupt(uint8_t interruptNumber, EBF_HalInstance *pHalInstance, uint8_t mode, uint32_t hint) { return EBF_INVALID_STATE; }
		uint8_t ProcessInterrupt(EBF_HalInstance *pHalInstance) { return EBF_INVALID_STATE; }
		uint8_t ProcessInterrupt(EBF_HalInstance *pHalInstance, uint32_t param1) { return EBF_INVALID_STATE; }
#endif

#ifdef EBF_SLEEP_IMPLEMENTATION
		void SetSleepMode(EBF_SleepMode mode) { sleepMode = mode; }

		// Will be called from ISR to restore HW after sleep
		void ExitSleep();
#endif

	private:
		static EBF_Logic *pStaticInstance;
		EBF_Timers timers;
#ifdef EBF_USE_INTERRUPTS
		EBF_MessageQueue msgQueue;
		EBF_MessageQueue::MessageEntry lastMessage;
		// Flag specifying when the HAL Process() calls are from ISR
		volatile uint8_t isRunFromISR;
		volatile uint32_t interruptHint;
		// Flag specifiying that the HAL Process() is called after interrupt processing
		uint8_t isPostInterruptProcessing;

		// Table of HAL pointers that have to be called from ISRs
		EBF_HalInstance* pHalIsr[EXTERNAL_NUM_INTERRUPTS];
		uint32_t isrHint[EXTERNAL_NUM_INTERRUPTS];
#endif

#ifdef EBF_SLEEP_IMPLEMENTATION
		void SleepConstructor();
		uint8_t InitSleep();
		uint8_t EnterSleep(uint32_t msSleep);

		EBF_SleepMode sleepMode;
		uint32_t sleepMs;	// The time in mSec sleep was intended

		// microSeconds that have to be added to Arduino's micros() and millis() due to sleeping time
		unsigned long microsAddition;
		uint8_t restoreUSBDevice;
		uint32_t apbBMask;
		uint32_t apbCMask;
#endif
		EBF_HalInstance **pHalInstances;
		uint8_t halIndex;
};

#endif
