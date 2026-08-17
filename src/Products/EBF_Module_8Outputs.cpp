#include "EBF_Module_8Outputs.h"

// Initializing EBF_Module_8Outputs class instance.
// The pwmFrequency should specify the desired frequency the channels should work (24..1526)
// Default 50Hz frequency should be ok for analog servos, leds and motor controllers
// The i2cAddress should specify the device I2C address
uint8_t EBF_Module_8Outputs::Init(uint16_t pwmFrequency, uint8_t i2cAddress)
{
	uint8_t rc;

	chip.i2cAddress = i2cAddress;

	rc = EBF_HalInstance::Init(HAL_Type::I2C_INTERFACE, i2cAddress);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// Init the chip with specified frequency
	rc = chip.Init(pwmFrequency);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// Turn off all the outputs
	for (uint8_t i=0; i<numberOfOutputs; i++) {
		rc = chip.SetChannelValue(i, 0);
		if (rc != EBF_OK) {
			EBF_REPORT_ERROR(rc);
			return rc;
		}
	}

	return EBF_OK;
}

// Sets boolean value on the specified output channel (1=HIGH, 0=LOW)
uint8_t EBF_Module_8Outputs::SetValue(uint8_t index, uint8_t value)
{
	uint8_t rc;

	if (index > numberOfOutputs) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	rc = chip.SetChannelValue(index, value);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Sets PWM timing value on the specified output channel
// float percent paramenter: 0=Complete OFF, 50=50% width ON/OFF, 100=Complete ON
uint8_t EBF_Module_8Outputs::SetPWM(uint8_t index, float pwmPercent)
{
	uint8_t rc;

	if (index > numberOfOutputs) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return 0;
	}

	if (pwmPercent < 0.0) pwmPercent = 0.0;
	if (pwmPercent > 100.0) pwmPercent = 100.0;

	// Special case for 0% and 100% values
	if (pwmPercent == 0.0) {
		rc = SetValue(index, 0);
	} else if (pwmPercent == 100.0) {
		rc = SetValue(index, 1);
	} else {
		// We will always start ON period on 0's step of the chip, so pwmPercent will specify
		// after how many steps 0..4096 should the line go back to OFF
		rc = chip.SetChannelPWM(index, 0, (uint16_t)4095 * (pwmPercent / 100.0));
	}

	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Returns current value of the specified output line
// float percent paramenter: 0=Complete OFF, 50=50% width ON/OFF, 100=Complete ON
float EBF_Module_8Outputs::GetValue(uint8_t index)
{
	uint8_t rc;
	uint16_t stepON;
	uint16_t stepOFF;

	if (index > numberOfOutputs) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return 0;
	}

	rc = chip.GetChannelPWM(index, stepON, stepOFF);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// Check constant values
	if (stepON == chip.NUMBER_OF_STEPS && stepOFF == 0) {
		return 100.0;
	}

	if (stepON == 0 && stepOFF == chip.NUMBER_OF_STEPS) {
		return 0.0;
	}

	// Do the conversion
	// Step OFF is always with higher value in current implementation, so:
	return ((stepOFF - stepON) * 100) / (float)chip.NUMBER_OF_STEPS;
}