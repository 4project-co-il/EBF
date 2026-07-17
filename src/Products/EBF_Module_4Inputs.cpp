#include "EBF_Module_4Inputs.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_DigitalInput.h"
#include "../Core/EBF_Core.h"

extern void EBF_EmptyCallback();

// Initializing EBF_Module_4Inputs class instance.
// The i2cAddress should specify the device I2C address
uint8_t EBF_Module_4Inputs::Init(uint8_t i2cAddress)
{
	uint8_t rc;

	chip.i2cAddress = i2cAddress;

	for (uint8_t i=0; i<numberOfInputs; i++) {
		onChangeCallback[i] = EBF_EmptyCallback;
	}

	rc = EBF_HalInstance::Init(HAL_Type::I2C_INTERFACE, i2cAddress);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// Configure the chip to all inputs, latch enable and interrupt mask enabled
	rc = chip.SetConfiguration(0xFF);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	rc = chip.SetLatching(0xFF);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// Enable for 4 LSBits since it's a 4input module implementation
	rc = chip.SetInterruptMask(0xF0);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// Get initial input lines status and reset the interrupts
	rc = chip.GetInput(lastValues);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

#ifdef EBF_USE_INTERRUPTS
uint8_t EBF_Module_4Inputs::AttachInterrupt(uint8_t interruptPin)
{
	uint8_t rc;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	rc = pLogic->AttachInterrupt(interruptPin, this, EBF_DigitalInput::InterruptMode::MODE_CHANGE);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// No polling is needed when interrupt is used
	SetPollingInterval(EBF_NO_POLLING);

	return EBF_OK;
}
#endif

// Called to process the instance after pollInterval
uint8_t EBF_Module_4Inputs::Process()
{
	EBF_Logic *pLogic = EBF_Logic::GetInstance();
	PostponedInterruptData data = {0};

#ifdef EBF_USE_INTERRUPTS
	// Process interrupt detected logic
	if (pLogic->IsPostInterruptProcessing()) {
		data.uint32 = pLogic->GetLastMessageParam1();

		// Set current interface provider and event index before the callbacks are called
		currentEventIndex = data.fields.index;
		lastValues = data.fields.event;

		// Callback from the normal run mode
		ExecuteCallback();
	}
#else
	// No interrupts are used, read the interrupt register to see if anything should be processed
	uint8_t rc;
	uint8_t intStatus;

	// Read interrupt register from the chip to know what input fired the interrupt
	rc = chip.GetInterruptStatus(intStatus);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return;
	}

	// We have something to process
	if (intStatus != 0) {
		// Read current inputs, it will reset the interrupts
		rc = chip.GetInput(lastValues);
		if (rc != EBF_OK) {
			EBF_REPORT_ERROR(rc);
			return;
		}

		// Loop on all the lines to find the changes. Several lines might change together
		for (currentEventIndex=0; currentEventIndex<numberOfInputs; currentEventIndex++) {
			if (intStatus & 1<<currentEventIndex) {
				ExecuteCallback();
			}
		}
	}
#endif

	return EBF_OK;
}

#ifdef EBF_USE_INTERRUPTS
void EBF_Module_4Inputs::ProcessInterrupt()
{
	uint8_t rc;
	uint8_t intStatus;
	uint8_t inputs;

	// Read interrupt register from the chip to know what input fired the interrupt
	rc = chip.GetInterruptStatus(intStatus);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return;
	}

	// Read current inputs, it will reset the interrupts
	rc = chip.GetInput(inputs);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return;
	}

	// Loop on all the lines to find the changes. Several lines might change together
	for (uint8_t i=0; i<numberOfInputs; i++) {
		if (intStatus & 1<<i) {
#ifdef EBF_DIRECT_CALL_FROM_ISR
			// Set currentEventIndex and lastValues before actual call to the callbacks
			currentEventIndex = i;
			lastValues = inputs;

			ExecuteCallback();
#else
			// Postpone the processing so the event will be handled from the normal run
			PostponeProcessing(i, inputs);
#endif
		}
	}
}

// PostponeProcessing should be called to postpone the callback processing later in the normal loop
uint8_t EBF_Module_4Inputs::PostponeProcessing(uint8_t eventIndex, uint8_t inputValues)
{
	uint8_t rc;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();
	PostponedInterruptData data = {0};

	data.fields.index = eventIndex;
	data.fields.event = inputValues;

	// Pass the control back to EBF, so it will call the Process() function from normal run
	rc = pLogic->PostponeInterrupt(this, data.uint32);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}
#endif

uint8_t EBF_Module_4Inputs::SetOnChange(uint8_t index, EBF_CallbackType onChangeCallback)
{
	if (index >= numberOfInputs) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	this->onChangeCallback[index] = onChangeCallback;

	return EBF_OK;
}

// Returns current value of the specified input line
uint8_t EBF_Module_4Inputs::GetValue(uint8_t index)
{
	if (GetValues() & 1<<index) {
		return 1;
	} else {
		return 0;
	}
}

// Returns current values of all the input lines
uint8_t EBF_Module_4Inputs::GetValues()
{
	uint8_t rc;
	uint8_t values;

	rc = chip.GetInput(values);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return 0;
	}

	return values;
}

// Returns last value of the specified input line as it appeared while reading from the chip
uint8_t EBF_Module_4Inputs::GetLastValue(uint8_t index)
{
	if (lastValues & 1<<index) {
		return 1;
	} else {
		return 0;
	}
}

// Returns the value of the input line as it was registered during last interrupt
uint8_t EBF_Module_4Inputs::GetLastValues()
{
	return lastValues;
}

void EBF_Module_4Inputs::ExecuteCallback()
{
	onChangeCallback[currentEventIndex]();
}