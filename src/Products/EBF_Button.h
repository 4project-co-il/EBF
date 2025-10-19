#ifndef __EBF_BUTTON_H__
#define __EBF_BUTTON_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "EBF_Switch.h"

class EBF_Button : public EBF_Switch {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_Button");

	public:
		enum ButtonState : uint8_t {
			BUTTON_OFF = 0,
			BUTTON_ON,
			BUTTON_WAITING_FOR_LONG_PRESS
		};

		uint8_t Init(uint8_t pinNumber, bool internelPullup = true);
		uint8_t GetState() { return state; }

		// Time in milli-Sec that will trigger onLongPress callback if the button is kept pressed
		void SetLongPressTime(uint16_t msTime) { this->longPressTime = msTime; }

		void SetOnPress(EBF_CallbackType onPressCallback) { this->onPressCallback = onPressCallback; }
		void SetOnLongPress(EBF_CallbackType onLongPressCallback) { this->onLongPressCallback = onLongPressCallback; }
		void SetOnRelease(EBF_CallbackType onReleaseCallback) { this->onReleaseCallback = onReleaseCallback; }

		// Debouncing is implemented in the EBF_Switch

	protected:
		ButtonState state;

		uint16_t longPressTime;			// in milli-Sec
		unsigned long longPressStart;	// in milli-Sec

		uint8_t Process();
		void ProcessSwitchCallback();

		// Callbacks
		EBF_CallbackType onPressCallback;
		EBF_CallbackType onLongPressCallback;
		EBF_CallbackType onReleaseCallback;

};

#endif
