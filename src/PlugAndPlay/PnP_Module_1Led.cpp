#include "PnP_Module_1Led.h"
#include "../Core/EBF_PlugAndPlayManager.h"
#include "../Core/EBF_PlugAndPlayI2C.h"

PnP_Module_1Led::PnP_Module_1Led()
{
	this->type = HAL_Type::PnP;
	this->id = PnP_DeviceId::PNP_ID_1LED;
}

uint8_t PnP_Module_1Led::Init()
{
	uint8_t rc = EBF_OK;
	PnP_DeviceInfo deviceInfo;
	uint8_t endpointIndex;
	EBF_PlugAndPlayI2C *pPnPI2C;
	EBF_PlugAndPlayHub *pAssignedHub;

	EBF_PlugAndPlayManager *pPnpManager = EBF_PlugAndPlayManager::GetInstance();

	// Assign the current instance to physical PnP device and get all needed information
	rc = pPnpManager->AssignDevice(this, deviceInfo, endpointIndex, &pPnPI2C, &pAssignedHub);
	if(rc != EBF_OK) {
		return rc;
	}

	// Save the I2C instance, although this device doesn't communicate via I2C, but via the HUBs
	// The PlugAndPlayI2C class have pointer to the HUB and port number, which are needed for interrupt lines manipulation
	this->pPnPI2C = pPnPI2C;

	// Fix type and ID after the EBF_Instance init
	this->type = HAL_Type::PnP;
	this->id = PnP_DeviceId::PNP_ID_1LED;

	// Attach interrupt lines for that device
	// Current device don't produce interrupts, but all the initializations are done in AssignInterruptLines
	rc = pAssignedHub->AssignInterruptLines(pPnPI2C->GetPortNumber(), endpointIndex, deviceInfo);
	if (rc != EBF_OK) {
		return rc;
	}

	return rc;
}

uint8_t PnP_Module_1Led::Process()
{
	// Nothing to do
	// No polling needed
	pollIntervalMs = EBF_NO_POLLING;

	return EBF_OK;
}

// Turns the LED ON.
uint8_t PnP_Module_1Led::On()
{
	return SetIntLine(0, 1);
}

// Turns the LED OFF.
uint8_t PnP_Module_1Led::Off()
{
	return SetIntLine(0, 0);
}

uint8_t PnP_Module_1Led::SetIntLine(uint8_t line, uint8_t value)
{
	uint8_t rc = EBF_OK;
	EBF_PlugAndPlayHub *pHub = pPnPI2C->GetHub();

	// Line can be only 0 or 1 (the interrupt line number)
	if (line > 1) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	rc = pHub->SetIntLine(*pPnPI2C, pPnPI2C->GetPortNumber(), line, value & 0x03);
	if (rc != EBF_OK) {
		return rc;
	}

	return rc;
}