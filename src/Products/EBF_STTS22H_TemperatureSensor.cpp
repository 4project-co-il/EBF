#include "EBF_STTS22H_TemperatureSensor.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_DigitalInput.h"

// Part of the code is based on the SparkFun_STTS22H_Arduino_Library
// https://github.com/sparkfun/SparkFun_STTS22H_Arduino_Library/tree/main

// Initializing EBF_STTS22H_TemperatureSensor class instance.
// The i2cAddress should specify the device I2C address
// The mode should specify the operation mode of the device

extern void EBF_EmptyCallback();

uint8_t EBF_STTS22H_TemperatureSensor::Init(uint8_t i2cAddress, OperationMode mode)
{
	uint8_t rc;
	StatusRegister_t status;

	this->state = InstanceState::STATE_IDLE;
	this->interruptAttached = 0;
	this->postProcessingFlags = 0;
	this->highThresholdSet = 0;
	this->lowThresholdSet = 0;
	this->i2cAddress = i2cAddress;

	onChangeCallback = EBF_EmptyCallback;
	onMeasureComplete = EBF_EmptyCallback;
	onThresholdHigh = EBF_EmptyCallback;
	onThresholdLow = EBF_EmptyCallback;

	rc = EBF_HalInstance::Init(HAL_Type::I2C, i2cAddress);
	if (rc != EBF_OK) {
		return rc;
	}

	// Reading status register to reset the interrupt line
	rc = GetStatusRegister(status);
	if (rc != EBF_OK) {
		return rc;
	}

	rc = SetOperationMode(mode);
	if (rc != EBF_OK) {
		return rc;
	}

	return EBF_OK;
}

#ifdef EBF_USE_INTERRUPTS
uint8_t EBF_STTS22H_TemperatureSensor::AttachInterrupt(uint8_t interruptPin)
{
	uint8_t rc;
	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	rc = pLogic->AttachInterrupt(interruptPin, this, EBF_DigitalInput::InterruptMode::MODE_LOW);
	if (rc != EBF_OK) {
		return rc;
	}

	interruptAttached = 1;

	UpdatePollInterval();

	return EBF_OK;
}
#endif

void EBF_STTS22H_TemperatureSensor::UpdatePollInterval()
{
	uint8_t needPolling = 0;

	// No need to poll, unless some callbacks are needed and there is no interrupt attached
	pollIntervalMs = EBF_NO_POLLING;

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
			pollIntervalMs = EBF_NO_POLLING;
			break;

		case OperationMode::MODE_ONE_SHOT:
			pollIntervalMs = 0;
			break;

		case OperationMode::MODE_1HZ:
			pollIntervalMs = 1000;
			break;

		case OperationMode::MODE_25HZ:
			pollIntervalMs = 1000 / 25;
			break;

		case OperationMode::MODE_50HZ:
			pollIntervalMs = 1000 / 50;
			break;

		case OperationMode::MODE_100HZ:
			pollIntervalMs = 1000 / 100;
			break;

		case OperationMode::MODE_200HZ:
			pollIntervalMs = 1000 / 200;
			break;
		}
	}
}

uint8_t EBF_STTS22H_TemperatureSensor::GetControlRegister(ControlRegister_t &ctrl)
{
	uint8_t rc;

	i2c.beginTransmission(i2cAddress);
	i2c.write(regControl);
	rc = i2c.endTransmission(false);
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	i2c.requestFrom(i2cAddress, 1);

	ctrl.reg = i2c.read();

	return EBF_OK;
}

uint8_t EBF_STTS22H_TemperatureSensor::SetControlRegister(ControlRegister_t ctrl)
{
	uint8_t rc;

	// Address increment should be turned ON for all operations
	ctrl.fields.addrInc = 1;

	i2c.beginTransmission(i2cAddress);
	i2c.write(regControl);
	i2c.write((uint8_t)ctrl.reg);
	rc = i2c.endTransmission();
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

uint8_t EBF_STTS22H_TemperatureSensor::GetValueRaw(int16_t &value)
{
	uint8_t rc;

	i2c.beginTransmission(i2cAddress);
	i2c.write(regTempOutput);
	rc = i2c.endTransmission(false);
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	i2c.requestFrom(i2cAddress, 2);

	rc = i2c.read();
	value = rc;

	rc = i2c.read();
	value |= rc << 8;

	return EBF_OK;
}

// Changes device operation mode
uint8_t EBF_STTS22H_TemperatureSensor::SetOperationMode(OperationMode mode)
{
	uint8_t rc;
	ControlRegister_t ctrl = {};

	operationMode = mode;

	// Any change should be done after the device is moved to power down mode
	ctrl.fields.freeRun = 0;
	ctrl.fields.mode_1Hz = 0;
	rc = SetControlRegister(ctrl);
	if (rc != EBF_OK) {
		return rc;
	}

	switch (operationMode)
	{
	case OperationMode::POWER_DOWN:
		state = InstanceState::STATE_IDLE;
		break;

	case OperationMode::MODE_ONE_SHOT:
		state = InstanceState::STATE_ONE_SHOT;

		ctrl.fields.oneShot = 1;
		break;

	case OperationMode::MODE_1HZ:
		state = InstanceState::STATE_MEASURING;

		ctrl.fields.mode_1Hz = 1;
		ctrl.fields.freeRun = 0;
		break;

	case OperationMode::MODE_25HZ:
		state = InstanceState::STATE_MEASURING;

		ctrl.fields.mode_1Hz = 0;
		ctrl.fields.freeRun = 1;
		ctrl.fields.avg = avgMode_25Hz;
		break;

	case OperationMode::MODE_50HZ:
		state = InstanceState::STATE_MEASURING;

		ctrl.fields.mode_1Hz = 0;
		ctrl.fields.freeRun = 1;
		ctrl.fields.avg = avgMode_50Hz;
		break;

	case OperationMode::MODE_100HZ:
		state = InstanceState::STATE_MEASURING;

		ctrl.fields.mode_1Hz = 0;
		ctrl.fields.freeRun = 1;
		ctrl.fields.avg = avgMode_100Hz;
		break;

	case OperationMode::MODE_200HZ:
		state = InstanceState::STATE_MEASURING;

		ctrl.fields.mode_1Hz = 0;
		ctrl.fields.freeRun = 1;
		ctrl.fields.avg = avgMode_200Hz;
		break;
	}

	// Power down was already sent in order to change the operation mode
	if (mode != OperationMode::POWER_DOWN) {
		SetControlRegister(ctrl);
	}

	UpdatePollInterval();

	return EBF_OK;
}

// Returns TRUE (1) while the device is busy performing the one-shot measurement
uint8_t EBF_STTS22H_TemperatureSensor::IsBusy()
{
	StatusRegister_t status = {};

	GetStatusRegister(status);

	return status.fields.busy;
}

// Returns Status register content
uint8_t EBF_STTS22H_TemperatureSensor::GetStatusRegister(StatusRegister_t &status)
{
	uint8_t rc;

	i2c.beginTransmission(i2cAddress);
	i2c.write(regStatus);
	rc = i2c.endTransmission(false);
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	i2c.requestFrom(i2cAddress, 1);

	status.reg = i2c.read();

	return EBF_OK;
}

// Returns the measured temperature in Celsius
float EBF_STTS22H_TemperatureSensor::GetValueC()
{
	int16_t rawTemp;

	GetValueRaw(rawTemp);

	return (float)rawTemp / 100.0f;
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
	float value;
	float change;
	StatusRegister_t status;

	// Process interrupt detected logic
	if(postProcessingFlags) {
		if(postProcessingFlags & postProcessHighThreshold) {
			onThresholdHigh();
		}

		if(postProcessingFlags & postProcessLowThreshold) {
			onThresholdLow();
		}

		postProcessingFlags = 0;
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
			pollIntervalMs = EBF_NO_POLLING;

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
			GetStatusRegister(status);

			if(highThresholdSet && status.fields.overThreshold) {
				onThresholdHigh();
			}

			if(lowThresholdSet && status.fields.underThreshold) {
				onThresholdLow();
			}
		}
		break;
	}

	return EBF_OK;
}

#ifdef EBF_USE_INTERRUPTS
void EBF_STTS22H_TemperatureSensor::ProcessInterrupt()
{
	StatusRegister_t status;

	// Status register is cleared on read
	GetStatusRegister(status);

	// Set the relevant post-processing flags
	if(status.fields.overThreshold) {
		postProcessingFlags |= postProcessHighThreshold;
	}

	if(status.fields.underThreshold) {
		postProcessingFlags |= postProcessLowThreshold;
	}

	if (postProcessingFlags) {
		// Pass the control back to EBF, so it will call the Process() function from normal run
		EBF_Logic *pLogic = EBF_Logic::GetInstance();

		pLogic->ProcessInterrupt(this);
	}
}
#endif

// Sets high threshold value
uint8_t EBF_STTS22H_TemperatureSensor::SetThresholdHigh(float temp)
{
	uint8_t rc;

	highThresholdSet = 1;

	i2c.beginTransmission(i2cAddress);
	i2c.write(regTempHighLimit);
	// Threshold = (temp_limit_reg -63) *0.64°C
	i2c.write((uint8_t)floor((temp / 0.64 + 63 + 0.5)));
	rc = i2c.endTransmission();
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

// Sets low threshold value
uint8_t EBF_STTS22H_TemperatureSensor::SetThresholdLow(float temp)
{
	uint8_t rc;

	lowThresholdSet = 1;

	i2c.beginTransmission(i2cAddress);
	i2c.write(regTempLowLimit);
	// Threshold = (temp_limit_reg -63) *0.64°C
	i2c.write((uint8_t)floor((temp / 0.64 + 63 + 0.5)));
	rc = i2c.endTransmission();
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

// Disable high threshold triggering
uint8_t EBF_STTS22H_TemperatureSensor::DisableThresholdHigh()
{
	uint8_t rc;

	highThresholdSet = 0;

	i2c.beginTransmission(i2cAddress);
	i2c.write(regTempHighLimit);
	// Value 0 disables the threshold
	i2c.write(0);
	rc = i2c.endTransmission();
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

// Disable low threshold triggering
uint8_t EBF_STTS22H_TemperatureSensor::DisableThresholdLow()
{
	uint8_t rc;

	lowThresholdSet = 0;

	i2c.beginTransmission(i2cAddress);
	i2c.write(regTempLowLimit);
	// Value 0 disables the threshold
	i2c.write(0);
	rc = i2c.endTransmission();
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}
