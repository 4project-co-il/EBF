#include "PnP_BasicInputInterface.h"

extern void EBF_EmptyCallback();

PnP_BasicInputInterface::PnP_BasicInputInterface()
{
	this->onChangeCallback = EBF_EmptyCallback;
}
