#include "EBF_HAL_STTS22H.h"
#include "../Core/EBF_Core.h"

EBF_HAL_STTS22H::EBF_HAL_STTS22H(EBF_I2C *i2cInterface) : EBF_I2CDevice(i2cInterface)
{
	EBF_I2CDevice::i2cAddress = defaultI2CAddress;
}

uint8_t EBF_HAL_STTS22H::Init(uint8_t i2cAddress)
{
	uint8_t rc;
	StatusRegister_t status;

	EBF_I2CDevice::i2cAddress = i2cAddress;

	// Reading status register to reset the interrupt line
	rc = GetStatusRegister(status);

	EBF_REPORT_AND_RETURN(rc);
}

// Returns Status register content
uint8_t EBF_HAL_STTS22H::GetStatusRegister(StatusRegister_t &status)
{
	uint8_t rc;

	rc = Read8bitRegister(regStatus, status.reg);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_STTS22H::GetControlRegister(ControlRegister_t &ctrl)
{
	uint8_t rc;

	rc = Read8bitRegister(regControl, ctrl.reg);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_STTS22H::SetControlRegister(ControlRegister_t ctrl)
{
	uint8_t rc;

	// Address increment should be turned ON for all operations
	ctrl.fields.addrInc = 1;

	rc = Write8bitRegister(regControl, ctrl.reg);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_STTS22H::GetValueRaw(int16_t &value)
{
	uint8_t rc;
	uint16_t readVal = 0;

	rc = Read16bitRegister(regTempOutput, readVal);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	value = (int16_t)readVal;

	EBF_REPORT_AND_RETURN(EBF_OK);
}

uint8_t EBF_HAL_STTS22H::PowerDown()
{
	uint8_t rc;
	ControlRegister_t ctrl = {};

	// Sending zero control register powers down the chip
	rc = SetControlRegister(ctrl);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_STTS22H::SetOneShotMode()
{
	uint8_t rc;
	ControlRegister_t ctrl = {};

	ctrl.fields.oneShot = 1;

	rc = SetControlRegister(ctrl);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_STTS22H::Set1HzMode()
{
	uint8_t rc;
	ControlRegister_t ctrl = {};

	ctrl.fields.mode_1Hz = 1;

	rc = SetControlRegister(ctrl);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_STTS22H::SetFreeRunMode(AveragingFrequency freq)
{
	uint8_t rc;
	ControlRegister_t ctrl = {};

	ctrl.fields.freeRun = 1;
	ctrl.fields.avg = (uint8_t)freq;

	rc = SetControlRegister(ctrl);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_STTS22H::IsBusy()
{
	uint8_t rc;
	StatusRegister_t status = {};

	rc = GetStatusRegister(status);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
	}

	return status.fields.busy;
}

uint8_t EBF_HAL_STTS22H::SetThresholdHigh(float temp)
{
	uint8_t rc;

	// Threshold = (temp_limit_reg - 63) * 0.64°C
	rc = Write8bitRegister(regTempHighLimit, (uint8_t)floor((temp / 0.64 + 63)));

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_STTS22H::SetThresholdLow(float temp)
{
	uint8_t rc;

	// Threshold = (temp_limit_reg - 63) * 0.64°C
	rc = Write8bitRegister(regTempLowLimit, (uint8_t)floor((temp / 0.64 + 63)));

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_STTS22H::GetThresholdHigh(float &temp)
{
	uint8_t rc;
	uint8_t readVal = 0;

	rc = Read8bitRegister(regTempHighLimit, readVal);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Threshold = (temp_limit_reg - 63) * 0.64°C
	temp = (readVal - 63.0) * 0.64;

	EBF_REPORT_AND_RETURN(EBF_OK);
}

uint8_t EBF_HAL_STTS22H::GetThresholdLow(float &temp)
{
	uint8_t rc;
	uint8_t readVal = 0;

	rc = Read8bitRegister(regTempLowLimit, readVal);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Threshold = (temp_limit_reg - 63) * 0.64°C
	temp = (readVal - 63.0) * 0.64;

	EBF_REPORT_AND_RETURN(EBF_OK);
}

uint8_t EBF_HAL_STTS22H::DisableThresholdHigh()
{
	uint8_t rc;

	// Value 0 disables the threshold
	rc = Write8bitRegister(regTempHighLimit, 0);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_STTS22H::DisableThresholdLow()
{
	uint8_t rc;

	// Value 0 disables the threshold
	rc = Write8bitRegister(regTempLowLimit, 0);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_STTS22H::GetIntFlags(uint8_t &highThreshold, uint8_t &lowThreshold)
{
	uint8_t rc;
	StatusRegister_t status = {};

	rc = GetStatusRegister(status);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	highThreshold = status.fields.overThreshold;
	lowThreshold = status.fields.underThreshold;

	EBF_REPORT_AND_RETURN(EBF_OK);
}
