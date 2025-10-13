#include "PnP_InputInterface.h"

PnP_InputInterfaceProvider* PnP_InputInterface::pCurrentProvider = NULL;

PnP_InputInterface::PnP_InputInterface()
{
	this->pInputProvider = NULL;
	this->providerIndex = 0;
}

uint8_t PnP_InputInterface::AssignInterfaceProvider(PnP_InputInterfaceProvider* pProvider, uint8_t index)
{
	this->pInputProvider = pProvider;
	this->providerIndex = index;

	return EBF_OK;
}