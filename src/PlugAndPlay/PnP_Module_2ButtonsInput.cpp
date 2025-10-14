#include "PnP_Module_2ButtonsInput.h"
#include "../Core/EBF_Core.h"

extern void EBF_EmptyCallback();

PnP_Module_2ButtonsInput::PnP_Module_2ButtonsInput()
{
	this->type = HAL_Type::PnP_DEVICE;
	this->id = PnP_DeviceId::PNP_ID_2BUTTONS_INPUT;

	isInterfaceAssigned = 0;
	lastValue = 0;
	currentEventIndex = 0;

	for (uint8_t i=0; i<numberOfButtons; i++) {
		onChangeCallback[i] = EBF_EmptyCallback;
	}
}

uint8_t PnP_Module_2ButtonsInput::Init()
{
	uint8_t rc;
	PnP_DeviceInfo deviceInfo;
	uint8_t endpointIndex;
	EBF_PlugAndPlayI2C *pPnPI2C;
	EBF_PlugAndPlayHub *pAssignedHub;

	EBF_PlugAndPlayManager *pPnpManager = EBF_PlugAndPlayManager::GetInstance();

	// Assign the current instance to physical PnP device and get all needed information
	rc = pPnpManager->AssignDevice(this, deviceInfo, endpointIndex, &pPnPI2C, &pAssignedHub);
	if(rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// Save the I2C instance, although this device doesn't communicate via I2C, but via the HUBs
	// The PlugAndPlayI2C class have pointer to the HUB and port number, which are needed for interrupt lines manipulation
	this->pPnPI2C = pPnPI2C;

	// Initialize the instance
	rc = EBF_HalInstance::Init(HAL_Type::PnP_DEVICE, PnP_DeviceId::PNP_ID_2BUTTONS_INPUT);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// PnP is interrupt driven, no polling is needed
	this->pollIntervalMs = EBF_NO_POLLING;

	// Attach interrupt lines for that device
	rc = pAssignedHub->AssignInterruptLines(pPnPI2C->GetPortNumber(), endpointIndex, deviceInfo);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Called by the EBF from normal run to take care of the events
uint8_t PnP_Module_2ButtonsInput::Process()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();
	PostponedInterruptData data = {0};

	// Process interrupt detected logic
	if (pLogic->IsPostInterruptProcessing()) {
		data.uint32 = pLogic->GetLastMessageParam1();

		// Set current interface provider and event index before the callbacks are called
		PnP_InputInterface::pCurrentProvider = this;
		currentEventIndex = data.fields.index;
		lastValue = data.fields.event;

		if (isInterfaceAssigned & 1<<data.fields.index) {
			// The onChangeCallback should be treated as a pointer to an interface instance
			PnP_InputInterface* pInput = GetAsInputInterface(data.fields.index);

			pInput->ExecuteCallback();
		} else {
			// Callback from the normal run mode
			onChangeCallback[data.fields.index]();
		}
	}

	// Processing is relevant only to the assigned input interfaces (long-press for example)
	if (isInterfaceAssigned != 0) {
		for (currentEventIndex=0; currentEventIndex<numberOfButtons; currentEventIndex++) {
			// Call the processing function of input interface instance
			if (isInterfaceAssigned & 1<<currentEventIndex) {
				// Set current interface provider and event index before the callbacks are called
				PnP_InputInterface::pCurrentProvider = this;
				// currentEventIndex is advanced in the loop

				PnP_InputInterface* pInput = GetAsInputInterface(currentEventIndex);

				pInput->Process();
			}
		}
	}

	return EBF_OK;
}

// Returns 1 if input is HIGH for a specific input line
uint8_t PnP_Module_2ButtonsInput::GetValue(uint8_t index)
{
	uint8_t rc;
	uint8_t value;

	rc = GetIntLine(index, value);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return 0;
	}

	return value;
}

// Returns HIGH or LOW for all input lines
uint8_t PnP_Module_2ButtonsInput::GetValue()
{
	uint8_t rc;
	EBF_PlugAndPlayHub *pHub = pPnPI2C->GetHub();
	uint8_t value;

	rc = pHub->GetIntLinesValue(pPnPI2C, pPnPI2C->GetPortNumber(), value);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return 0;
	}

	return value;

}

// Returns the value of the input line as it was registered during last interrupt
uint8_t PnP_Module_2ButtonsInput::GetLastValue(uint8_t index)
{
	return lastValue;
}

// Returns pointer to current interface instance, if it was assigned
PnP_InputInterface* PnP_Module_2ButtonsInput::GetCurrentInterface()
{
	if (isInterfaceAssigned & 1<<currentEventIndex) {
		return GetAsInputInterface(currentEventIndex);
	}

	return NULL;
}

uint8_t PnP_Module_2ButtonsInput::GetIntLine(uint8_t line, uint8_t &value)
{
	uint8_t rc;
	EBF_PlugAndPlayHub *pHub = pPnPI2C->GetHub();

	rc = pHub->GetIntLine(pPnPI2C, pPnPI2C->GetPortNumber(), line, value);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Called directly from the ISR
void PnP_Module_2ButtonsInput::ProcessInterrupt()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();
	EBF_PlugAndPlayHub::InterruptHint hint;

	// Hint will tell us what interrupt arrived
	hint.uint32 = pLogic->GetInterruptHint();

	lastValue = GetValue(hint.fields.interruptNumber);

#ifdef EBF_DIRECT_CALL_FROM_ISR
	PnP_InputInterface::pCurrentProvider = this;
	currentEventIndex = hint.fields.interruptNumber;

	if (isInterfaceAssigned & 1<<hint.fields.interruptNumber) {
		// The onChangeCallback should be treated as a pointer to an interface instance
		PnP_InputInterface* pInput = GetAsInputInterface(hint.fields.interruptNumber);

		pInput->ExecuteCallback();
	} else {
		// Should treat the onChangeCallback pointer as a pointer to the interface instance
		onChangeCallback[hint.fields.interruptNumber]();
	}
#else
	// Postpone the processing so the event will be handled from the normal run
	PostponeProcessing();
#endif
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
	data.fields.event = lastValue;

	// Pass the control back to EBF, so it will call the Process() function from normal run
	rc = pLogic->PostponeInterrupt(this, data.uint32);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

uint8_t PnP_Module_2ButtonsInput::SetOnChange(uint8_t index, EBF_CallbackType onChangeCallback)
{
	if (index > numberOfButtons) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	this->onChangeCallback[index] = onChangeCallback;

	return EBF_OK;
}

uint8_t PnP_Module_2ButtonsInput::AssignInterface(uint8_t index, PnP_InputInterface* pIfInstance)
{
	if (index > numberOfButtons) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	onChangeCallback[index] = (EBF_CallbackType)pIfInstance;
	isInterfaceAssigned |= 1<<index;

	return pIfInstance->AssignInterfaceProvider(this, index);
}

// This is an override of the default function
void PnP_Module_2ButtonsInput::SetPollingInterval(uint32_t ms)
{
	// Since we have multiple input/buttons that might need the polling at the same time
	// we can't just change the value. Need to check if there is an interface instance that might
	// still need the low value
	if (ms == EBF_NO_POLLING && isInterfaceAssigned != 0) {
		for (uint8_t i=0; i<numberOfButtons; i++) {
			if (isInterfaceAssigned & 1<<i) {
				PnP_InputInterface* pInput = GetAsInputInterface(i);

				// The input instance still need processing
				if (pInput->IsProcessingNeeded()) {
					return;
				}
			}
		}
	}

	EBF_HalInstance::SetPollingInterval(ms);
}

// Returns button index that caused the callback function call
// You can have the same callback function for all onChange events
// where you can call the GetEventIndex to know which button was actually pressed
uint8_t PnP_Module_2ButtonsInput::GetEventIndex()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();
	EBF_PlugAndPlayHub::InterruptHint hint;

	if (InInterrupt()) {
		// Hint will tell us what interrupt arrived
		hint.uint32 = pLogic->GetInterruptHint();

		return hint.fields.interruptNumber;
	} else {
		return currentEventIndex;
	}
}