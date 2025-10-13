#ifndef __PNP_SWITCH_INTERFACE_H__
#define __PNP_SWITCH_INTERFACE_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/PnP_InputInterface.h"

class PnP_SwitchInterface : public PnP_InputInterface {
	public:
		PnP_SwitchInterface();

		virtual InputInterface_Type GetType() { return InputInterface_Type::SWITCH; }

		uint8_t SetOnSwitchOn(EBF_CallbackType onCallback) { this->onSwitchOn = onCallback; return EBF_OK; }

		uint8_t SetOnSwitchOff(EBF_CallbackType offCallback) { this->onSwitchOff = offCallback; return EBF_OK; }

	protected:
		EBF_CallbackType onSwitchOn;
		EBF_CallbackType onSwitchOff;

		virtual void ExecuteCallback();
};

#endif
