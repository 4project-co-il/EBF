#include "EBF_STTS22H_TemperatureSensor.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_DigitalInput.h"
#include "../Core/EBF_Core.h"

extern void EBF_EmptyCallback();

// Part of the code is based on the SparkFun_STTS22H_Arduino_Library
// https://github.com/sparkfun/SparkFun_STTS22H_Arduino_Library/tree/main

// Initializing EBF_STTS22H_TemperatureSensor class instance.
// The i2cAddress should specify the device I2C address
// The mode should specify the operation mode of the device
uint8_t EBF_STTS22H_TemperatureSensor::Init(uint8_t i2cAddress, OperationMode mode)
{
	uint8_t rc;

	this->state = InstanceState::STATE_IDLE;
	this->interruptAttached = 0;
	this->highThresholdSet = 0;
	this->lowThresholdSet = 0;

	onChangeCallback = EBF_EmptyCallback;
	onMeasureComplete = EBF_EmptyCallback;
	onThresholdHigh = EBF_EmptyCallback;
	onThresholdLow = EBF_EmptyCallback;

	rc = EBF_HalInstance::Init(HAL_Type::I2C_INTERFACE, i2cAddress);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Init the STTS22H chip
	rc = chip.Init(i2cAddress);
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
uint8_t EBF_STTS22H_TemperatureSensor::AttachInterrupt(uint8_t interruptPin)
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

void EBF_STTS22H_TemperatureSensor::UpdatePollInterval()
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
			SetPollingInterval(EBF_NO_POLLING);
			break;

		case OperationMode::MODE_ONE_SHOT:
			SetPollingInterval(0);
			break;

		case OperationMode::MODE_1HZ:
			SetPollingInterval(1000);
			break;

		case OperationMode::MODE_25HZ:
			SetPollingInterval(1000 / 25);
			break;

		case OperationMode::MODE_50HZ:
			SetPollingInterval(1000 / 50);
			break;

		case OperationMode::MODE_100HZ:
			SetPollingInterval(1000 / 100);
			break;

		case OperationMode::MODE_200HZ:
			SetPollingInterval(1000 / 200);
			break;
		}
	}
}

// Changes device operation mode
uint8_t EBF_STTS22H_TemperatureSensor::SetOperationMode(OperationMode mode)
{
	uint8_t rc;

	operationMode = mode;

	// Any change should be done after the device is moved to power down mode
	rc = chip.PowerDown();
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	switch (operationMode)
	{
	case OperationMode::POWER_DOWN:
		state = InstanceState::STATE_IDLE;

		// Power down was already sent to the chip
		break;

	case OperationMode::MODE_ONE_SHOT:
		state = InstanceState::STATE_ONE_SHOT;

		rc = chip.SetOneShotMode();
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;

	case OperationMode::MODE_1HZ:
		state = InstanceState::STATE_MEASURING;

		rc = chip.Set1HzMode();
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;

	case OperationMode::MODE_25HZ:
		state = InstanceState::STATE_MEASURING;

		rc = chip.SetFreeRunMode(EBF_HAL_STTS22H::AVERAGING_25HZ);
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;

	case OperationMode::MODE_50HZ:
		state = InstanceState::STATE_MEASURING;

		rc = chip.SetFreeRunMode(EBF_HAL_STTS22H::AVERAGING_50HZ);
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;

	case OperationMode::MODE_100HZ:
		state = InstanceState::STATE_MEASURING;

		rc = chip.SetFreeRunMode(EBF_HAL_STTS22H::AVERAGING_100HZ);
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;

	case OperationMode::MODE_200HZ:
		state = InstanceState::STATE_MEASURING;

		rc = chip.SetFreeRunMode(EBF_HAL_STTS22H::AVERAGING_200HZ);
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
		break;
	}

	UpdatePollInterval();

	EBF_REPORT_AND_RETURN(EBF_OK);
}

// Returns the measured temperature in Celsius
float EBF_STTS22H_TemperatureSensor::GetValueC()
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
float EBF_STTS22H_TemperatureSensor::GetValueF()
{
	return (GetValueC() + 1.8) + 32.0;
}

// Returns the measured temperature in Kelvin
float EBF_STTS22H_TemperatureSensor::GetValueK()
{
	return (GetValueC() + 273.15);
}

// Called to process the instance after pollInterval
// We can't use ProcessCallbacks in that case since we need to detect the changes
// in distance and not for every change of the digital input line
uint8_t EBF_STTS22H_TemperatureSensor::Process()
{
	uint8_t rc;
	float value;
	float change;
	PostponedInterruptData interruptData;
	uint8_t highThreshold;
	uint8_t lowThreshold;

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

	case InstanceState::STATE_ONE_SHOT:
		if (!IsBusy()) {
			// state is changed to IDLE after one-shot is complete and there is no need to poll it again
			state = InstanceState::STATE_IDLE;
			SetPollingInterval(EBF_NO_POLLING);

			onMeasureComplete();
		}
		break;

	case InstanceState::STATE_MEASURING:
		if (onChangeCallback != EBF_EmptyCallback) {
			value = GetValueC();

			change = (value - lastValue) * 100.0 / value;

			if (abs(change) > changePercent) {
				lastValue = value;
				onChangeCallback();
			}
		}

		if (lowThresholdSet || highThresholdSet) {
			rc = chip.GetIntFlags(highThreshold, lowThreshold);
			if (rc != EBF_OK) {
				EBF_REPORT_AND_RETURN(rc);
			}

			if(highThresholdSet && highThreshold) {
				onThresholdHigh();
			}

			if(lowThresholdSet && lowThreshold) {
				onThresholdLow();
			}
		}
		break;
	}

	EBF_REPORT_AND_RETURN(EBF_OK);
}

#ifdef EBF_USE_INTERRUPTS
void EBF_STTS22H_TemperatureSensor::ProcessInterrupt()
{
	uint8_t rc;
	uint8_t highThreshold;
	uint8_t lowThreshold;

	// Get current interrupts. status register is cleared on read
	rc = chip.GetIntFlags(highThreshold, lowThreshold);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return;
	}

	// Save the interrupt flags
	currentInterruptData.uint32 = 0;
	currentInterruptData.fields.highThreshold = highThreshold;
	currentInterruptData.fields.lowThreshold = lowThreshold;

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
uint8_t EBF_STTS22H_TemperatureSensor::PostponeProcessing()
{
	uint8_t rc;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	// Pass the control back to EBF, so it will call the Process() function from normal run
	rc = pLogic->PostponeInterrupt(this, currentInterruptData.uint32);

	EBF_REPORT_AND_RETURN(rc);
}
#endif

// Sets high threshold value
uint8_t EBF_STTS22H_TemperatureSensor::SetThresholdHigh(float temp)
{
	uint8_t rc;

	highThresholdSet = 1;

	rc = chip.SetThresholdHigh(temp);

	EBF_REPORT_AND_RETURN(rc);
}

// Sets low threshold value
uint8_t EBF_STTS22H_TemperatureSensor::SetThresholdLow(float temp)
{
	uint8_t rc;

	lowThresholdSet = 1;

	rc = chip.SetThresholdLow(temp);

	EBF_REPORT_AND_RETURN(rc);
}

// Gets high threshold value
float EBF_STTS22H_TemperatureSensor::GetThresholdHigh()
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
float EBF_STTS22H_TemperatureSensor::GetThresholdLow()
{
	uint8_t rc;
	float value;

	rc = chip.GetThresholdLow(value);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
	}

	return value;
}

// Disable high threshold triggering
uint8_t EBF_STTS22H_TemperatureSensor::DisableThresholdHigh()
{
	uint8_t rc;

	highThresholdSet = 0;

	rc = chip.DisableThresholdHigh();

	EBF_REPORT_AND_RETURN(rc);
}

// Disable low threshold triggering
uint8_t EBF_STTS22H_TemperatureSensor::DisableThresholdLow()
{
	uint8_t rc;

	lowThresholdSet = 0;

	rc = chip.DisableThresholdLow();

	EBF_REPORT_AND_RETURN(rc);
}

void EBF_STTS22H_TemperatureSensor::ExecuteCallback(volatile PostponedInterruptData& data)
{
	if(data.fields.highThreshold) {
		onThresholdHigh();
	}

	if(data.fields.lowThreshold) {
		onThresholdLow();
	}
}
