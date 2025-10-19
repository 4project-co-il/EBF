#ifndef __EBF_BUTTON_LOGIC_H__
#define __EBF_BUTTON_LOGIC_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_HalInstance.h"

class EBF_ButtonLogic {
	public:
		friend class PnP_Module_2ButtonsInput;

	protected:
		enum ButtonState : uint8_t {
			BUTTON_STATE_PRESSED = 0,
			BUTTON_STATE_RELEASED,
			BUTTON_STATE_WAITING_FOR_LONG_PRESS
		};

		enum ButtonEvent : uint8_t {
			BUTTON_EVENT_NONE = 0,
			BUTTON_EVENT_PRESS,
			BUTTON_EVENT_RELEASE,
			BUTTON_EVENT_LONG_PRESS
		};

		EBF_ButtonLogic();

		uint8_t Init();

		uint8_t GetState() { return state; }

		// Called to execute callblack based on the specified event
		// This function is used to re-execute callback again after postponing the interrupt from ISR to normal run
		void ExecuteCallback(ButtonEvent event);

		// Time in milli-Sec that will trigger onLongPress callback if the button is kept pressed
		void SetLongPressTime(uint16_t msTime) { this->longPressTime = msTime; }

		void SetOnPress(EBF_CallbackType onPressCallback) { this->onPressCallback = onPressCallback; }
		void SetOnLongPress(EBF_CallbackType onLongPressCallback) { this->onLongPressCallback = onLongPressCallback; }
		void SetOnRelease(EBF_CallbackType onReleaseCallback) { this->onReleaseCallback = onReleaseCallback; }

	private:
		ButtonState state;

		uint16_t longPressTime;			// in milli-Sec
		unsigned long longPressStart;	// in milli-Sec

		EBF_ButtonLogic::ButtonEvent Process(uint8_t value, EBF_HalInstance* pHal);

		// Callbacks
		EBF_CallbackType onPressCallback;
		EBF_CallbackType onLongPressCallback;
		EBF_CallbackType onReleaseCallback;
};

#endif
