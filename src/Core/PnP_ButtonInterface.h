#ifndef __PNP_BUTTON_INTERFACE_H__
#define __PNP_BUTTON_INTERFACE_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/PnP_InputInterface.h"

class PnP_ButtonInterface : public PnP_InputInterface {
	public:
		PnP_ButtonInterface();

		virtual InputInterface_Type GetType() { return InputInterface_Type::BUTTON; }

		// Time in milli-Sec that will trigger onLongPress callback if the button is kept pressed
		void SetLongPressTime(uint16_t msTime) { this->longPressTime = msTime; }

		void SetOnPress(EBF_CallbackType onPressCallback) { this->onPressCallback = onPressCallback; }
		void SetOnRelease(EBF_CallbackType onReleaseCallback) { this->onReleaseCallback = onReleaseCallback; }
#ifndef EBF_DIRECT_CALL_FROM_ISR
		// Long press logic can't be implemented with direct calls from ISRs since it
		// requires timers, which are called from normal run and can be interrupted by the ISRs
		void SetOnLongPress(EBF_CallbackType onLongPressCallback) { this->onLongPressCallback = onLongPressCallback; }
#endif

	protected:
		EBF_CallbackType onPressCallback;
		EBF_CallbackType onLongPressCallback;
		EBF_CallbackType onReleaseCallback;

		virtual void ExecuteCallback();
		void ExecuteCallbackEx(uint8_t value);
		virtual uint8_t IsProcessingNeeded();
		virtual uint8_t Process();

	private:
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

	 	ButtonEvent ProcessInput(uint8_t value);

		ButtonState state;

		uint16_t longPressTime;			// in milli-Sec
		unsigned long longPressStart;	// in milli-Sec
};

#endif
