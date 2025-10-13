#include "PnP_SwitchInterface.h"

extern void EBF_EmptyCallback();

PnP_SwitchInterface::PnP_SwitchInterface()
{
	this->onSwitchOn = EBF_EmptyCallback;
	this->onSwitchOff = EBF_EmptyCallback;
}

void PnP_SwitchInterface::ExecuteCallback() {
	if (GetCurrentInterface()->GetLastValue() == 0) {
		onSwitchOn();
	} else {
		onSwitchOff();
	}
}
