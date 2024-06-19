#ifndef __EBF_DIGITALINPUT_H__
#define __EBF_DIGITALINPUT_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_HalInstance.h"

class EBF_DigitalInput : protected EBF_HalInstance {
	public:
		friend class EBF_Core;

		enum InterruptMode : uint8_t {
			MODE_CHANGE = CHANGE,
			MODE_RISING = RISING,
			MODE_FALLING = FALLING
		};

		uint8_t Init(
			uint8_t pinNumber,
			EBF_CallbackType callbackFunc,
			InterruptMode isrMode = InterruptMode::MODE_CHANGE,
			bool internelPullup = false
		);

		uint8_t GetValue();
		uint8_t GetLastValue();

		void SetPollInterval(uint16_t ms);

	protected:
		uint8_t Process();
		virtual void ProcessCallback() { callbackFunc(); }
		EBF_CallbackType callbackFunc;
		uint8_t pinNumber;

	private:
		uint8_t lastValue;
		InterruptMode isrMode;
};

#endif
