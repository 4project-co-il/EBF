#include "PnP_Module_2ButtonsInput.h"

PnP_Module_2ButtonsInput::PnP_Module_2ButtonsInput()
{
	this->type = HAL_Type::PnP_DEVICE;
	this->id = PnP_DeviceId::PNP_ID_2BUTTONS_INPUT;
}

extern void EBF_EmptyCallback();

uint8_t PnP_Module_2ButtonsInput::Init()
{
	uint8_t rc = EBF_OK;
	PnP_DeviceInfo deviceInfo;
	uint8_t endpointIndex;
	EBF_PlugAndPlayI2C *pPnPI2C;
	EBF_PlugAndPlayHub *pAssignedHub;

	EBF_PlugAndPlayManager *pPnpManager = EBF_PlugAndPlayManager::GetInstance();

	for (uint8_t i=0; i<numberOfButtons; i++) {
		button[i].Init();
	}

	// Assign the current instance to physical PnP device and get all needed information
	rc = pPnpManager->AssignDevice(this, deviceInfo, endpointIndex, &pPnPI2C, &pAssignedHub);
	if(rc != EBF_OK) {
		return rc;
	}

	// Save the I2C instance, although this device doesn't communicate via I2C, but via the HUBs
	// The PlugAndPlayI2C class have pointer to the HUB and port number, which are needed for interrupt lines manipulation
	this->pPnPI2C = pPnPI2C;

	// Initialize the instance
	rc = EBF_HalInstance::Init(HAL_Type::PnP_DEVICE, PnP_DeviceId::PNP_ID_2BUTTONS_INPUT);
	if (rc != EBF_OK) {
		return rc;
	}

	// PnP is interrupt driven, no polling is needed
	this->pollIntervalMs = EBF_NO_POLLING;

	// Attach interrupt lines for that device
	rc = pAssignedHub->AssignInterruptLines(pPnPI2C->GetPortNumber(), endpointIndex, deviceInfo);
	if (rc != EBF_OK) {
		return rc;
	}

	return rc;
}

// Called by the EBF from normal run to take care of the events
uint8_t PnP_Module_2ButtonsInput::Process()
{
	uint8_t rc;

	EBF_Logic *pLogic = EBF_Logic::GetInstance();
	PostponedInterruptData data = {0};

	// Process interrupt detected logic
	if (pLogic->IsPostInterruptProcessing()) {
		data.uint32 = pLogic->GetLastMessageParam1();

		// Re-execute postponed event, this time from the normal run
		button[data.fields.index].ExecuteCallback((EBF_ButtonLogic::ButtonState)data.fields.state);
	}

	for (uint8_t i=0; i<numberOfButtons; i++) {
		rc = button[i].Process(this);

		if (rc != EBF_OK) {
			return rc;
		}
	}

	return EBF_OK;
}

// Returns 1 if input is HIGH for a specific interrupt line
uint8_t PnP_Module_2ButtonsInput::GetValue(uint8_t index)
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
uint8_t PnP_Module_2ButtonsInput::GetValue()
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

uint8_t PnP_Module_2ButtonsInput::GetIntLine(uint8_t line, uint8_t &value)
{
	uint8_t rc = EBF_OK;
	EBF_PlugAndPlayHub *pHub = pPnPI2C->GetHub();

	rc = pHub->GetIntLine(*pPnPI2C, pPnPI2C->GetPortNumber(), line, value);
	if (rc != EBF_OK) {
		return rc;
	}

	return EBF_OK;
}

// Called directly from the ISR
void PnP_Module_2ButtonsInput::ProcessInterrupt()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();
	EBF_PlugAndPlayHub::InterruptHint hint;
	uint8_t value;

	// Hint will tell us what interrupt arrived
	hint.uint32 = pLogic->GetInterruptHint();

	value = GetValue(hint.fields.interruptNumber);
	// Process the relevant button
	button[hint.fields.interruptNumber].Process(value, this);
}

// PostponeProcessing should be called to execute the callback processing later in the normal loop
uint8_t PnP_Module_2ButtonsInput::PostponeProcessing()
{
	uint8_t rc;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();
	EBF_PlugAndPlayHub::InterruptHint hint = {0};
	PostponedInterruptData data = {0};

	hint.uint32 = pLogic->GetInterruptHint();

	data.fields.index = hint.fields.interruptNumber;
	data.fields.state = button[data.fields.index].GetState();

	// Pass the control back to EBF, so it will call the Process() function from normal run
	rc = pLogic->ProcessInterrupt(this, data.uint32);

	return rc;
}

uint8_t PnP_Module_2ButtonsInput::SetLongPressTime(uint8_t index, uint16_t msTime)
{
	if (index > numberOfButtons) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	button[index].SetLongPressTime(msTime);

	return EBF_OK;
}

uint8_t PnP_Module_2ButtonsInput::SetOnPress(uint8_t index, EBF_CallbackType onPressCallback)
{
	if (index > numberOfButtons) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	button[index].SetOnPress(onPressCallback);

	return EBF_OK;
}

uint8_t PnP_Module_2ButtonsInput::SetOnLongPress(uint8_t index, EBF_CallbackType onLongPressCallback)
{
	if (index > numberOfButtons) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	button[index].SetOnLongPress(onLongPressCallback);

	return EBF_OK;
}

uint8_t PnP_Module_2ButtonsInput::SetOnRelease(uint8_t index, EBF_CallbackType onReleaseCallback)
{
	if (index > numberOfButtons) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	button[index].SetOnRelease(onReleaseCallback);

	return EBF_OK;
}
