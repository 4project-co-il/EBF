#ifndef __EBF_SWITCH_H__
#define __EBF_SWITCH_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_DigitalInput.h"

class EBF_Switch : protected EBF_DigitalInput {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_Switch");

	public:
		enum SwitchState : uint8_t {
			SWITCH_OFF = 0,
			SWITCH_ON,
		};

		uint8_t Init(uint8_t pinNumber, bool internelPullup = true);
		uint8_t GetState() { return state; }

		// Setting debounce time in milli-seconds
		void SetDebounceTime(uint8_t ms) { debounceTime = ms; }
		// Setting polling interval in milli-seconds
		void SetPollInterval(uint32_t ms);
		void RestorePollInterval();

		void SetOnChange(EBF_CallbackType onChangeCallback) { this->onChangeCallback = onChangeCallback; }

	protected:
		SwitchState state;
		SwitchState lastState;
		uint8_t debounceTime;			// in milli-Sec
		unsigned long debounceStart;	// in milli-Sec

		uint8_t Process();
		void ProcessCallback();
		virtual void ProcessSwitchCallback() { onChangeCallback(); }

		// Callbacks
		EBF_CallbackType onChangeCallback;

	private:
		uint32_t savedPollingInterval;
};

#endif
