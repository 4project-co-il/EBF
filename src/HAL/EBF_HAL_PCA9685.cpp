#include "EBF_HAL_PCA9685.h"

EBF_HAL_PCA9685::EBF_HAL_PCA9685(EBF_I2C *i2cInterface) : EBF_I2CDevice(i2cInterface)
{
	i2cAddress = defaultI2CAddress;
}

uint8_t EBF_HAL_PCA9685::Init(uint16_t freq)
{
	uint8_t rc;
	uint8_t prescale;

	// freq can be between 24Hz up to 1526Hz
	if (freq < 24 || freq >= 1526) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	// In order to change the PRE_SCALE to set the required frequency, the chip have to be in sleep mode
	rc = EnterSleepMode();
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// Prescale is calculated as clock / (4096 * freq) - 1
	prescale = INT_OSCILATOR_FREQ / (NUMBER_OF_STEPS * freq) - 1;
	if (prescale < 3) prescale = 3;

	rc = Write8bitRegister(regPrescale, prescale);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// Exit sleep mode
	rc = ExitSleepMode(0);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

uint8_t EBF_HAL_PCA9685::EnterSleepMode()
{
	uint8_t rc;
	uint8_t mode = 0;

	// Entering sleep mode by writing 1 to bit4
	// And enable auto-increment for faster updates

	// Bit 0 = 1, PCA9685 responds to LED All Call I2C-bus address
	// We will need it for syncronization of multiple output modules
	mode |= 1<<0;

	// Bit 4 = 0, Normal mode
	mode |= 1<<4;

	// Bit 5 = 1, Register Auto-Increment enabled
	mode |= 1<<5;

	rc = Write8bitRegister(regMode1, mode);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

uint8_t EBF_HAL_PCA9685::ExitSleepMode(uint8_t restoreOutputs)
{
	uint8_t rc;
	uint8_t mode = 0;

	// Exiting the sleep mode by writing 0 to bit4
	// And enable auto-increment for faster updates

	// Bit 0 = 1, PCA9685 responds to LED All Call I2C-bus address
	// We will need it for syncronization of multiple output modules
	mode |= 1<<0;

	// Bit 4 = 0, Normal mode

	// Bit 5 = 1, Register Auto-Increment enabled
	mode |= 1<<5;

	rc = Write8bitRegister(regMode1, mode);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// Have to wait 500uSec for the clock to stabilize before anything can be done with the chip
	delayMicroseconds(500);

	// Restore outputs is needed
	if (restoreOutputs) {
		// Read mode1 register to check bit7 (the restart bit) if something can be restored
		mode = 0;
		rc = Read8bitRegister(regMode1, mode);
		if (rc != EBF_OK) {
			EBF_REPORT_ERROR(rc);
			return rc;
		}

		// We cab restore the PWM outputs
		if (mode & 1<<7) {
			// Writing 1 to bit7 will restore the PWM outputs
			// We just write the mode register content again
			rc = Write8bitRegister(regMode1, mode);
			if (rc != EBF_OK) {
				EBF_REPORT_ERROR(rc);
				return rc;
			}
		}
	}

	return EBF_OK;
}

// Sets PWM output on specified channel
// stepON specifies the step number (out of 4096) when the output should raise to logical HIGH
// stepOFF specifies the steo numberstep (out of 4096) when the output should lower to logical LOW
uint8_t EBF_HAL_PCA9685::SetChannelPWM(uint8_t channel, uint16_t stepON, uint16_t stepOFF)
{
	uint8_t rc;
	uint8_t buffer[5];	// 5 bytes are needed to set a channel (channel register, ON_L, ON_H, OFF_L, OFF_H)

	// PCA9685 chip have 16 channels 0..15
	if (channel > 15) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	// Prepare the buffer
	buffer[0] = regLED0_ON_L + channel*4;
	buffer[1] = stepON & 0xFF;
	buffer[2] = (stepON >> 8) & 0x0F;
	buffer[3] = stepOFF & 0xFF;
	buffer[4] = (stepOFF >> 8) & 0x0F;

	rc = WriteBuffer(buffer, sizeof(buffer));
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

uint8_t EBF_HAL_PCA9685::GetChannelPWM(uint8_t channel, uint16_t &stepON, uint16_t &stepOFF)
{
	uint8_t rc;
	uint32_t pwmData;

	// PCA9685 chip have 16 channels 0..15
	if (channel > 15) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	rc = Read32bitRegister(regLED0_ON_L + channel*4, pwmData);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// The data is read in wrong order
	stepON = (pwmData >> 24) & 0xFF;
	stepON |= ((pwmData >> 16) & 0xFF) << 8;

	stepOFF = (pwmData >> 8) & 0xFF;
	stepOFF |= ((pwmData) & 0xFF) << 8;

	// Constant ON setting
	if (stepON & 1<<12) {
		stepON = NUMBER_OF_STEPS;
		stepOFF = 0;
	}

	// Constant OFF setting
	if (stepOFF & 1<<12) {
		stepON = 0;
		stepOFF = NUMBER_OF_STEPS;
	}

	return EBF_OK;
}

// Sets the channel to specified value (0 or 1)
uint8_t EBF_HAL_PCA9685::SetChannelValue(uint8_t channel, uint8_t value)
{
	uint8_t rc;

	// PCA9685 chip have 16 channels 0..15
	if (channel > 15) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	// 5 bytes are needed to set a channel (channel register, ON_L, ON_H, OFF_L, OFF_H)
	uint8_t buffer[5];

	// Prepare the buffer
	buffer[0] = regLED0_ON_L + channel*4;

	if (value == 0) {
		// Turning OFF using bit4 in LEDx_OFF_H register
		buffer[1] = 0;
		buffer[2] = 0;
		buffer[3] = 0;
		buffer[4] = 1<<4;
	} else {
		// Turning ON using bit4 in LEDx_ON_H register
		buffer[1] = 0;
		buffer[2] = 1<<4;
		buffer[3] = 0;
		buffer[4] = 0;
	}

	rc = WriteBuffer(buffer, sizeof(buffer));
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Sets multiple channels using one communication sequeince in order to syncronize all outputs
// The OnOff buffer should containg 2 16bit values for each channel, similar to SetChannelPWM parameters
// First stepON value, second stepOFF value for each channel
uint8_t EBF_HAL_PCA9685::SetMultipleChannelsPWM(uint8_t startChannel, uint8_t numberOfChannels, uint16_t *pOnOffBuffer)
{
	uint8_t rc;

	// PCA9685 chip have 16 channels 0..15
	if (numberOfChannels > 15) {
		EBF_REPORT_ERROR(EBF_INDEX_OUT_OF_BOUNDS);
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	// 5 bytes are needed to set a channel (channel register, ON_L, ON_H, OFF_L, OFF_H)
	uint8_t buffer[5*numberOfChannels];
	uint8_t bufIdx = 0;

	// prepare the buffer
	for (uint8_t i=0; i<numberOfChannels; i++) {
		buffer[bufIdx++] = regLED0_ON_L + (startChannel+i)*4;

		buffer[bufIdx++] = pOnOffBuffer[i*2] & 0xFF;
		buffer[bufIdx++] = (pOnOffBuffer[i*2] >> 8) & 0x0F;
		buffer[bufIdx++] = pOnOffBuffer[i*2 + 1] & 0xFF;
		buffer[bufIdx++] = (pOnOffBuffer[i*2 + 1] >> 8) & 0x0F;
	}

	rc = WriteBuffer(buffer, sizeof(buffer));
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}
