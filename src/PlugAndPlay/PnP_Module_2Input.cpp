#include "PnP_Module_2Input.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_PlugAndPlayManager.h"
#include "../Core/EBF_PlugAndPlayI2C.h"

PnP_Module_2Input::PnP_Module_2Input()
{
	this->type = HAL_Type::PnP_DEVICE;
	this->id = PnP_DeviceId::PNP_ID_2INPUT;
}

extern void EBF_EmptyCallback();

uint8_t PnP_Module_2Input::Init()
{
	uint8_t rc = EBF_OK;
	PnP_DeviceInfo deviceInfo;
	uint8_t endpointIndex;
	EBF_PlugAndPlayI2C *pPnPI2C;
	EBF_PlugAndPlayHub *pAssignedHub;

	EBF_PlugAndPlayManager *pPnpManager = EBF_PlugAndPlayManager::GetInstance();

	onChangeCallback1 = EBF_EmptyCallback;
	onChangeCallback2 = EBF_EmptyCallback;

	// Assign the current instance to physical PnP device and get all needed information
	rc = pPnpManager->AssignDevice(this, deviceInfo, endpointIndex, &pPnPI2C, &pAssignedHub);
	if(rc != EBF_OK) {
		return rc;
	}

	// Save the I2C instance, although this device doesn't communicate via I2C, but via the HUBs
	// The PlugAndPlayI2C class have pointer to the HUB and port number, which are needed for interrupt lines manipulation
	this->pPnPI2C = pPnPI2C;

	// Fix type and ID after the EBF_Instance init
	this->type = HAL_Type::PnP_DEVICE;
	this->id = PnP_DeviceId::PNP_ID_2INPUT;

	// Attach interrupt lines for that device
	rc = pAssignedHub->AssignInterruptLines(pPnPI2C->GetPortNumber(), endpointIndex, deviceInfo);
	if (rc != EBF_OK) {
		return rc;
	}

	return rc;
}

uint8_t PnP_Module_2Input::Process()
{
	uint8_t intBits;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	// Process interrupt detected logic
	if (pLogic->IsPostInterruptProcessing()) {
		intBits = pLogic->GetLastMessageParam1();

		if(intBits & 0x01) {
			onChangeCallback1();
		}

		if(intBits & 0x02) {
			onChangeCallback2();
		}
	}

	return EBF_OK;
}

// Returns 1 if input is HIGH for a specific interrupt line
uint8_t PnP_Module_2Input::GetValue(uint8_t index)
{
	uint8_t rc;
	uint8_t value;

	rc = GetIntLine(index, value);
	if (rc != EBF_OK) {
		return 0;
	}

	return value;
}

// Returns bits 0 and 1 as HIGH or LOW for both interrupt lines
uint8_t PnP_Module_2Input::GetValue()
{
	uint8_t rc = EBF_OK;
	EBF_PlugAndPlayHub *pHub = pPnPI2C->GetHub();
	uint8_t value;

	rc = pHub->GetIntLinesValue(*pPnPI2C, pPnPI2C->GetPortNumber(), value);
	if (rc != EBF_OK) {
		return 0;
	}

	return value;

}

uint8_t PnP_Module_2Input::GetIntLine(uint8_t line, uint8_t &value)
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
void PnP_Module_2Input::ProcessInterrupt()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();
	EBF_PlugAndPlayHub::InterruptHint hint;

	hint.uint32 = pLogic->GetInterruptHint();

	if(hint.fields.interruptNumber == 0) {
		onChangeCallback1();
	}

	if(hint.fields.interruptNumber == 1) {
		onChangeCallback2();
	}
}
#endif

// PostponeProcessing should be called to postpone the callback processing later in the normal loop
uint8_t PnP_Module_2Input::PostponeProcessing()
{
	uint8_t rc;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();
	EBF_PlugAndPlayHub::InterruptHint hint;

	hint.uint32 = pLogic->GetInterruptHint();

	// Pass the control back to EBF, so it will call the Process() function from normal run

	// 0x01 = first bit set, indicating interrupt #1 fired
	// 0x02 = second bit set, indicating interrupt #2 fired
	rc = pLogic->ProcessInterrupt(this, 0x01 << hint.fields.interruptNumber);

	return rc;
}
