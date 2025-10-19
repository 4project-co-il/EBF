#ifndef __EBF_CORE_H__
#define __EBF_CORE_H__

#include <Arduino.h>

#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_HalInstance.h"
#include "EBF_Timers.h"
#include "EBF_DigitalInput.h"
#include "EBF_Serial.h"

// EBF_Logic is a wrapper class to hide the usage of the singleton from the user
// and make the APIs a bit easier for usage
class EBF_Core {
	public:
		uint8_t Init();
		uint8_t Init(uint8_t maxTimers);
		uint8_t Init(uint8_t maxTimers, uint8_t queueSize);

		uint8_t AddHalInstance(EBF_HalInstance &instance);
		uint8_t Process();

		EBF_HalInstance *GetHalInstance(EBF_HalInstance::HAL_Type type, uint8_t id);

#ifndef EBF_REMOVE_DEBUG_CODE
		void SetErrorHandlerSerial(EBF_Serial &serial);
		static void ReportError(const char* pModuleName, uint32_t line, EBF_ERROR_CODE error);
#endif

	public:
		// Timer functions
		uint8_t InitTimer(uint8_t timerId, EBF_CallbackType callbackPtr, uint16_t milliSec);
		uint8_t SetTimeout(uint8_t timerId, uint16_t milliSec);
		uint8_t StartTimer(uint8_t timerId);
		uint8_t StartTimer(uint8_t timerId, uint16_t milliSec);
		uint8_t StopTimer(uint8_t timerId);
		uint8_t RestartTimer(uint8_t timerId);

		unsigned long micros();
		unsigned long millis();

#ifdef EBF_USE_INTERRUPTS
		// Message queue functions
		uint8_t PostponeInterrupt(EBF_DigitalInput &digitalInput);
		static uint8_t UseInterrupts() { return 1; }
		uint8_t InInterrupt();
		uint32_t GetInterruptHint();

		// Message queue debug functions
		uint8_t GetNumberOfMessages();
		uint8_t GetMaxNumberOfMessages();
#else
		uint8_t PostponeInterrupt(EBF_DigitalInput &digitalInput) { return EBF_INVALID_STATE; }
		static uint8_t UseInterrupts() { return 0; }
		uint8_t InInterrupt() { return 0; }
		uint32_t GetInterruptHint() { return 0; }
#endif

#ifdef EBF_SLEEP_IMPLEMENTATION
		void SetSleepMode(EBF_SleepMode mode);
#else
		void SetSleepMode(EBF_SleepMode mode) {}
#endif
};

#endif
