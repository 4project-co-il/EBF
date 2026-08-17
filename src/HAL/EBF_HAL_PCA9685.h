#ifndef __EBF_HAL_PCA9685_H__
#define __EBF_HAL_PCA9685_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Core.h"
#include "../Core/EBF_I2CDevice.h"

// This class implements access to PCA9685 chip, 16-channel 12bit PWM controller with I2C interface
class EBF_HAL_PCA9685 : public EBF_I2CDevice {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_HAL_PCA9685");

	public:
		static const uint8_t defaultI2CAddress = 0x40;
		static const uint8_t defaultAllCallAddress = 0x70;

		// Constants
		// TODO: Might need to allow changing the oscilator frequency, as Adafruit does in their library,
		// due to the fact that the oscilator is not accurate and there might be a big difference between
		// the modules
		static const uint32_t INT_OSCILATOR_FREQ = 		25000000;	// 25MHz oscilator
		static const uint16_t NUMBER_OF_STEPS =			4096;

		EBF_HAL_PCA9685(EBF_I2C *i2cInterface);
		EBF_HAL_PCA9685(EBF_I2C &i2cInterface) : EBF_HAL_PCA9685(&i2cInterface) { }

		// Initialize the chip to specified frequency of PWM outputs
		// freq can be between 24Hz up to 1526Hz
		uint8_t Init(uint16_t freq);

	private:
		// Registers
		const uint8_t regMode1 =					0x00;
		const uint8_t regMode2 =					0x01;
		const uint8_t regSubAddr1 =					0x02;
		const uint8_t regSubAddr2 =					0x03;
		const uint8_t regSubAddr3 =					0x04;
		const uint8_t regAllCallAddr =				0x05;
		const uint8_t regLED0_ON_L =				0x06;
		const uint8_t regLED0_ON_H =				0x07;
		const uint8_t regLED0_OFF_L =				0x08;
		const uint8_t regLED0_OFF_H =				0x09;
		const uint8_t regLED1_ON_L =				0x0A;
		const uint8_t regLED1_ON_H =				0x0B;
		const uint8_t regLED1_OFF_L =				0x0C;
		const uint8_t regLED1_OFF_H =				0x0D;
		const uint8_t regLED2_ON_L =				0x0E;
		const uint8_t regLED2_ON_H =				0x0F;
		const uint8_t regLED2_OFF_L =				0x10;
		const uint8_t regLED2_OFF_H =				0x11;
		const uint8_t regLED3_ON_L =				0x12;
		const uint8_t regLED3_ON_H =				0x13;
		const uint8_t regLED3_OFF_L =				0x14;
		const uint8_t regLED3_OFF_H =				0x15;
		const uint8_t regLED4_ON_L =				0x16;
		const uint8_t regLED4_ON_H =				0x17;
		const uint8_t regLED4_OFF_L =				0x18;
		const uint8_t regLED4_OFF_H =				0x19;
		const uint8_t regLED5_ON_L =				0x1A;
		const uint8_t regLED5_ON_H =				0x1B;
		const uint8_t regLED5_OFF_L =				0x1C;
		const uint8_t regLED5_OFF_H =				0x1D;
		const uint8_t regLED6_ON_L =				0x1E;
		const uint8_t regLED6_ON_H =				0x1F;
		const uint8_t regLED6_OFF_L =				0x20;
		const uint8_t regLED6_OFF_H =				0x21;
		const uint8_t regLED7_ON_L =				0x22;
		const uint8_t regLED7_ON_H =				0x23;
		const uint8_t regLED7_OFF_L =				0x24;
		const uint8_t regLED7_OFF_H =				0x25;
		const uint8_t regLED8_ON_L =				0x26;
		const uint8_t regLED8_ON_H =				0x27;
		const uint8_t regLED8_OFF_L =				0x28;
		const uint8_t regLED8_OFF_H =				0x29;
		const uint8_t regLED9_ON_L =				0x2A;
		const uint8_t regLED9_ON_H =				0x2B;
		const uint8_t regLED9_OFF_L =				0x2C;
		const uint8_t regLED9_OFF_H =				0x2D;
		const uint8_t regLED10_ON_L =				0x2E;
		const uint8_t regLED10_ON_H =				0x2F;
		const uint8_t regLED10_OFF_L =				0x30;
		const uint8_t regLED10_OFF_H =				0x31;
		const uint8_t regLED11_ON_L =				0x32;
		const uint8_t regLED11_ON_H =				0x33;
		const uint8_t regLED11_OFF_L =				0x34;
		const uint8_t regLED11_OFF_H =				0x35;
		const uint8_t regLED12_ON_L =				0x36;
		const uint8_t regLED12_ON_H =				0x37;
		const uint8_t regLED12_OFF_L =				0x38;
		const uint8_t regLED12_OFF_H =				0x39;
		const uint8_t regLED13_ON_L =				0x3A;
		const uint8_t regLED13_ON_H =				0x3B;
		const uint8_t regLED13_OFF_L =				0x3C;
		const uint8_t regLED13_OFF_H =				0x3D;
		const uint8_t regLED14_ON_L =				0x3E;
		const uint8_t regLED14_ON_H =				0x3F;
		const uint8_t regLED14_OFF_L =				0x40;
		const uint8_t regLED14_OFF_H =				0x41;
		const uint8_t regLED15_ON_L =				0x42;
		const uint8_t regLED15_ON_H =				0x43;
		const uint8_t regLED15_OFF_L =				0x44;
		const uint8_t regLED15_OFF_H =				0x45;
		const uint8_t regAllLED_ON_L =				0xFA;
		const uint8_t regAllLED_ON_H =				0xFB;
		const uint8_t regAllLED_OFF_L =				0xFC;
		const uint8_t regAllLED_OFF_H =				0xFD;
		const uint8_t regPrescale =					0xFE;
		const uint8_t regTestMode =					0xFF;

	public:
		// APIs

		// Enter the sleep mode
		uint8_t EnterSleepMode();
		// Exit the sleep mode, with option to restore all channels PWM if available
		uint8_t ExitSleepMode(uint8_t restoreOutputs = 0);

		// Sets PWM output on specified channel
		// stepON specifies the step number (out of 4096) when the output should raise to logical HIGH
		// stepOFF specifies the steo numberstep (out of 4096) when the output should lower to logical LOW
		uint8_t SetChannelPWM(uint8_t channel, uint16_t stepON, uint16_t stepOFF);

		// Returnds specified channel PWM settings
		// stepON specifies the step number (out of 4096) when the output should raise to logical HIGH
		// stepOFF specifies the steo numberstep (out of 4096) when the output should lower to logical LOW
		// When the channel is set to constant ON, stepON will be 4096, stepOFF will be 0.
		// When the channel is set to constant OFF, stepON will be 0, stepOFF will be 4096.
		uint8_t GetChannelPWM(uint8_t channel, uint16_t &stepON, uint16_t &stepOFF);

		// Sets the channel to specified value (0 or 1)
		uint8_t SetChannelValue(uint8_t channel, uint8_t value);

		// Sets multiple channels using one communication sequeince in order to syncronize all outputs
		// The OnOff buffer should containg 2 16bit values for each channel, similar to SetChannelPWM parameters
		// First stepON value, second stepOFF value for each channel
		uint8_t SetMultipleChannelsPWM(uint8_t startChannel, uint8_t numberOfChannels, uint16_t *pOnOffBuffer);
};

#endif
