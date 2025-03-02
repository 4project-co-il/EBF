#include "PnP_Module_1Input.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_PlugAndPlayManager.h"
#include "../Core/EBF_PlugAndPlayI2C.h"

PnP_Module_1Input::PnP_Module_1Input()
{
	this->type = HAL_Type::PnP;
	this->id = PnP_DeviceId::PNP_ID_1INPUT;
}

extern void EBF_EmptyCallback();

uint8_t PnP_Module_1Input::Init()
{
	uint8_t rc = EBF_OK;
	PnP_DeviceInfo deviceInfo;
	uint8_t endpointIndex;
	EBF_PlugAndPlayI2C *pPnPI2C;
	EBF_PlugAndPlayHub *pAssignedHub;

	EBF_PlugAndPlayManager *pPnpManager = EBF_PlugAndPlayManager::GetInstance();

	onChangeCallback = EBF_EmptyCallback;

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
	this->id = PnP_DeviceId::PNP_ID_1INPUT;

	// Attach interrupt lines for that device
	rc = pAssignedHub->AssignInterruptLines(pPnPI2C->GetPortNumber(), endpointIndex, deviceInfo);
	if (rc != EBF_OK) {
		return rc;
	}

	return rc;
}

uint8_t PnP_Module_1Input::Process()
{
	uint8_t intBits;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	// Process interrupt detected logic
	if (pLogic->IsPostInterruptProcessing()) {
		intBits = pLogic->GetLastMessageParam1();

		if(intBits & 0x01) {
			onChangeCallback();
		}
	}

	return EBF_OK;
}

// Returns 1 if input is HIGH
uint8_t PnP_Module_1Input::GetValue()
{
	uint8_t rc;
	uint8_t value;

	rc = GetIntLine(0, value);
	if (rc != EBF_OK) {
		return 0;
	}

	return value;
}

uint8_t PnP_Module_1Input::GetIntLine(uint8_t line, uint8_t &value)
{
	uint8_t rc = EBF_OK;
	EBF_PlugAndPlayHub *pHub = pPnPI2C->GetHub();

	rc = pHub->GetIntLine(*pPnPI2C, pPnPI2C->GetPortNumber(), line, value);
	if (rc != EBF_OK) {
		return rc;
	}

	return EBF_OK;
}


#ifdef EBF_USE_INTERRUPTS
void PnP_Module_1Input::ProcessInterrupt()
{
	onChangeCallback();
}
#endif

// PostponeProcessing should be called to postpone the callback processing later in the normal loop
uint8_t PnP_Module_1Input::PostponeProcessing()
{
	uint8_t rc;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	// Pass the control back to EBF, so it will call the Process() function from normal run

	// 0x01 = first bit set, indicating interrupt #1 fired
	rc = pLogic->ProcessInterrupt(this, 0x01);

	return rc;
}
