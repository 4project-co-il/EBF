#ifndef __EBF_BUTTON_LOGIC_H__
#define __EBF_BUTTON_LOGIC_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_HalInstance.h"

class EBF_ButtonLogic {
	public:
		friend class PnP_Module_2ButtonsInput;

	protected:
		enum ButtonState : uint8_t {
			BUTTON_LOGICAL_0 = 0,
			BUTTON_LOGICAL_1,
			BUTTON_WAITING_FOR_LONG_PRESS
		};

		EBF_ButtonLogic();

		uint8_t Init();

		uint8_t GetState() { return state; }

		// Can be called to execute callblack based on a known state
		// This funftion is used to re-execute callback again after postponing the interrupt from ISR to normal run
		void ExecuteCallback(ButtonState state);

		// Time in milli-Sec that will trigger onLongPress callback if the button is kept pressed
		void SetLongPressTime(uint16_t msTime) { this->longPressTime = msTime; }

		void SetOnPress(EBF_CallbackType onPressCallback) { this->onPressCallback = onPressCallback; }
		void SetOnLongPress(EBF_CallbackType onLongPressCallback) { this->onLongPressCallback = onLongPressCallback; }
		void SetOnRelease(EBF_CallbackType onReleaseCallback) { this->onReleaseCallback = onReleaseCallback; }

	private:
		ButtonState state;

		uint16_t longPressTime;			// in milli-Sec
		unsigned long longPressStart;	// in milli-Sec

		uint8_t Process(EBF_HalInstance* pHal);
		uint8_t Process(uint8_t value, EBF_HalInstance* pHal);

		// Callbacks
		EBF_CallbackType onPressCallback;
		EBF_CallbackType onLongPressCallback;
		EBF_CallbackType onReleaseCallback;
};

#endif
