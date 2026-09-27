#include "EBF_TMP102_TemperatureSensor.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_DigitalInput.h"
#include "../Core/EBF_Core.h"

extern void EBF_EmptyCallback();

// Initializing EBF_TMP102_TemperatureSensor class instance.
// The i2cAddress should specify the device I2C address
// The mode should specify the operation mode of the device
uint8_t EBF_TMP102_TemperatureSensor::Init(uint8_t i2cAddress, OperationMode mode)
{
	uint8_t rc;

	this->state = InstanceState::STATE_IDLE;
	this->interruptAttached = 0;
	this->highThreshold = 0.0;	// Will be set later with SetThresholdHigh()
	this->lowThreshold = 0.0;	// will be set later with SetThresholdLow()

	onChangeCallback = EBF_EmptyCallback;
	onThresholdHigh = EBF_EmptyCallback;
	onThresholdLow = EBF_EmptyCallback;

	rc = EBF_HalInstance::Init(HAL_Type::I2C_INTERFACE, i2cAddress);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Init the TMP102 chip
	rc = chip.Init(i2cAddress);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Set thresholds to the extreme values, so the interrupt won't arrive if not set normal values
	// Defaults are too close to realistic values, tHigh=80 degC, tLow=75 degC
	rc = SetThresholdHigh(125.0);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	GetThresholdLow();
	rc = SetThresholdLow(-40.0);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	rc = SetOperationMode(mode);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(EBF_OK);
}

#ifdef EBF_USE_INTERRUPTS
uint8_t EBF_TMP102_TemperatureSensor::AttachInterrupt(uint8_t interruptPin)
{
	uint8_t rc;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	rc = pLogic->AttachInterrupt(interruptPin, this, EBF_DigitalInput::InterruptMode::MODE_LOW);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	interruptAttached = 1;

	UpdatePollInterval();

	EBF_REPORT_AND_RETURN(EBF_OK);
}
#endif

void EBF_TMP102_TemperatureSensor::UpdatePollInterval()
{
	uint8_t needPolling = 0;

	// No need to poll, unless some callbacks are needed and there is no interrupt attached
	SetPollingInterval(EBF_NO_POLLING);

	if (onChangeCallback != NULL && onChangeCallback != EBF_EmptyCallback) {
		needPolling = 1;
	}

	if (!interruptAttached) {
		if (onThresholdHigh != NULL && onThresholdHigh != EBF_EmptyCallback) {
			needPolling = 1;
		}

		if (onThresholdLow != NULL && onThresholdLow != EBF_EmptyCallback) {
			needPolling = 1;
		}
	}

	if (needPolling) {
		switch (operationMode)
		{
		case OperationMode::POWER_DOWN:
		case OperationMode::MODE_ONE_SHOT:
			SetPollingInterval(EBF_NO_POLLING);
			break;

		case OperationMode::MODE_0_25HZ:
			SetPollingInterval(4000);
			break;

		case OperationMode::MODE_1HZ:
			SetPollingInterval(1000);
			break;

		case OperationMode::MODE_4HZ:
			SetPollingInterval(1000 / 4);
			break;

		case OperationMode::MODE_8HZ:
			SetPollingInterval(1000 / 8);
			break;
		}
	}
}

// Changes device operation mode
uint8_t EBF_TMP102_TemperatureSensor::SetOperationMode(OperationMode mode)
{
	uint8_t rc;

	operationMode = mode;

	switch (operationMode)
	{
	case OperationMode::POWER_DOWN:
		state = InstanceState::STATE_IDLE;

		rc = chip.PowerDown();
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;

	case OperationMode::MODE_ONE_SHOT:
		// The device moves automatically to power down state after one-shot measurement
		// There is no interrupt after the measurement is done. Polling can be done with IsBusy() function
		state = InstanceState::STATE_IDLE;

		rc = chip.SetOneShotMode();
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;

	case OperationMode::MODE_0_25HZ:
		state = InstanceState::STATE_MEASURING;

		rc = chip.SetConversionRate(EBF_HAL_TMP102::CONVERSION_RATE_0_25HZ);
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;

	case OperationMode::MODE_1HZ:
		state = InstanceState::STATE_MEASURING;

		rc = chip.SetConversionRate(EBF_HAL_TMP102::CONVERSION_RATE_1HZ);
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;

	case OperationMode::MODE_4HZ:
		state = InstanceState::STATE_MEASURING;

		rc = chip.SetConversionRate(EBF_HAL_TMP102::CONVERSION_RATE_4HZ);
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;

	case OperationMode::MODE_8HZ:
		state = InstanceState::STATE_MEASURING;

		rc = chip.SetConversionRate(EBF_HAL_TMP102::CONVERSION_RATE_8HZ);
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;
	}

	UpdatePollInterval();

	EBF_REPORT_AND_RETURN(EBF_OK);
}

// Returns the measured temperature in Celsius
float EBF_TMP102_TemperatureSensor::GetValueC()
{
	uint8_t rc;
	float value;

	rc = chip.GetValue(value);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
	}

	return value;
}

// Returns the measured temperature in Fahrenheit
float EBF_TMP102_TemperatureSensor::GetValueF()
{
	return (GetValueC() + 1.8) + 32.0;
}

// Returns the measured temperature in Kelvin
float EBF_TMP102_TemperatureSensor::GetValueK()
{
	return (GetValueC() + 273.15);
}

// Called to process the instance after pollInterval
// We can't use ProcessCallbacks in that case since we need to detect the changes
// in distance and not for every change of the digital input line
uint8_t EBF_TMP102_TemperatureSensor::Process()
{
	float value;
	float change;
	PostponedInterruptData interruptData;

	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	// Process interrupt detected logic
	if (pLogic->IsPostInterruptProcessing()) {
		interruptData.uint32 = pLogic->GetLastMessageParam1();

		ExecuteCallback(interruptData);

		EBF_REPORT_AND_RETURN(EBF_OK);
	}

	switch (state)
	{
	case InstanceState::STATE_IDLE:
		// Do nothing
		break;

	case InstanceState::STATE_MEASURING:
		value = GetValueC();

		if (onChangeCallback != EBF_EmptyCallback) {
			change = (value - lastValue) * 100.0 / value;

			if (abs(change) > changePercent) {
				lastValue = value;
				onChangeCallback();
			}
		}

		if(value >= highThreshold) {
			onThresholdHigh();
		}

		if(value < lowThreshold) {
			onThresholdLow();
		}
		break;
	}

	EBF_REPORT_AND_RETURN(EBF_OK);
}

#ifdef EBF_USE_INTERRUPTS
void EBF_TMP102_TemperatureSensor::ProcessInterrupt()
{
	float value;

	// There is no interrupt flags in the chip that can tell what condition was met
	// We'll read current temperature value and compare with the thresholds
	value = GetValueC();

	// Save the interrupt flags
	currentInterruptData.uint32 = 0;

	if (value >= highThreshold) {
		currentInterruptData.fields.highThreshold = 1;
	}

	if (value < lowThreshold) {
		currentInterruptData.fields.lowThreshold = 1;
	}

	// We have an interrupt signaled by the chip
	if (currentInterruptData.uint32 != 0) {
#ifdef EBF_DIRECT_CALL_FROM_ISR
		ExecuteCallback(currentInterruptProcessing);
#else
		PostponeProcessing();
#endif
	}
}

// PostponeProcessing should be called to postpone the callback processing later in the normal loop
uint8_t EBF_TMP102_TemperatureSensor::PostponeProcessing()
{
	uint8_t rc;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	// Pass the control back to EBF, so it will call the Process() function from normal run
	rc = pLogic->PostponeInterrupt(this, currentInterruptData.uint32);

	EBF_REPORT_AND_RETURN(rc);
}
#endif

// Sets high threshold value
uint8_t EBF_TMP102_TemperatureSensor::SetThresholdHigh(float value)
{
	uint8_t rc;

	highThreshold = value;

	rc = chip.SetThresholdHigh(value);

	EBF_REPORT_AND_RETURN(rc);
}

// Sets low threshold value
uint8_t EBF_TMP102_TemperatureSensor::SetThresholdLow(float value)
{
	uint8_t rc;

	lowThreshold = value;

	rc = chip.SetThresholdLow(value);

	EBF_REPORT_AND_RETURN(rc);
}

// Gets high threshold value
float EBF_TMP102_TemperatureSensor::GetThresholdHigh()
{
	uint8_t rc;
	float value;

	rc = chip.GetThresholdHigh(value);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
	}

	return value;
}

// Gets low threshold value
float EBF_TMP102_TemperatureSensor::GetThresholdLow()
{
	uint8_t rc;
	float value;

	rc = chip.GetThresholdLow(value);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
	}

	return value;
}

void EBF_TMP102_TemperatureSensor::ExecuteCallback(volatile PostponedInterruptData& data)
{
	if(data.fields.highThreshold) {
		onThresholdHigh();
	}

	if(data.fields.lowThreshold) {
		onThresholdLow();
	}
}

uint8_t EBF_TMP102_TemperatureSensor::SetConsecutiveFaults(ConsecutiveFaults faults)
{
	uint8_t rc;

	rc = chip.SetConsecutiveFaults((EBF_HAL_TMP102::ConsecutiveFaults)faults);

	EBF_REPORT_AND_RETURN(rc);
}

EBF_TMP102_TemperatureSensor::ConsecutiveFaults EBF_TMP102_TemperatureSensor::GetConsecutiveFaults()
{
	uint8_t rc;
	EBF_HAL_TMP102::ConsecutiveFaults faults;

	rc = chip.GetConsecutiveFaults(faults);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
	}

	return (EBF_TMP102_TemperatureSensor::ConsecutiveFaults)faults;
}
