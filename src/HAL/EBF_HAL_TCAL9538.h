#ifndef __EBF_HAL_TCAL9538_H__
#define __EBF_HAL_TCAL9538_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Core.h"
#include "../Core/EBF_I2CDevice.h"

// This class implements access to TCAL9538 chip, I2C 8bit I/O expander with interrupts
class EBF_HAL_TCAL9538 : public EBF_I2CDevice {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_HAL_TCAL9538");

	public:
		static const uint8_t defaultI2CAddress = 0x70;

		EBF_HAL_TCAL9538(EBF_I2C *i2cInterface);
		EBF_HAL_TCAL9538(EBF_I2C &i2cInterface) : EBF_HAL_TCAL9538(&i2cInterface) { }

	private:
		// Registers
		const uint8_t regInputPort = 				0x00;
		const uint8_t regOutputPort =				0x01;
		const uint8_t regPolarityInversion =		0x02;
		const uint8_t regConfiguration =			0x03;
		const uint8_t regOutputDriveStrength0 =		0x40;
		const uint8_t regOutputDriveStrength1 =		0x41;
		const uint8_t regInputLatch =				0x42;
		const uint8_t regPullUpPullDownEnable =		0x43;
		const uint8_t regPullUpPullDownSelection =	0x44;
		const uint8_t regInterruptMask =			0x45;
		const uint8_t regInterruptStatus =			0x46;
		const uint8_t regOutputPortConfiguration =	0x4F;

	public:
		// APIs

		// Returns input register
		uint8_t GetInput(uint8_t &input);

		// Sets output register
		uint8_t SetOutput(uint8_t output);
		// Returns current output flip-flops state
		uint8_t GetOuput(uint8_t &output);

		// Sets polarity inversion (1=inverted, 0=non-inverted)
		uint8_t SetPolarityInversion(uint8_t inversion);
		// Returns current polarity inversion setting
		uint8_t GetPolarityInversion(uint8_t &inversion);

		// Sets ports configuration (1=input, 0=output)
		uint8_t SetConfiguration(uint8_t config);
		// Gets current port configuration
		uint8_t GetConfiguration(uint8_t &config);

		// Sets output drivers strength. 2bits for every port (00=0.25x, 01=0.50x, 10=0.75x, 11=1.0x)
		uint8_t SetOuputStrength(uint16_t strength);
		// Gets current output drivers strength
		uint8_t GetOutputStrngth(uint16_t &strength);

		// Sets input latch register (1=latch enabled, 0=no latch)
		uint8_t SetLatching(uint8_t latch);
		// Gets current latch register setting
		uint8_t GetLatching(uint8_t &latch);

		// Sets pull-up/pull-down resistor connection (0=no connection, 1=connected)
		uint8_t SetPullUpPullDownEnable(uint8_t resistorEnable);
		// Gets current pull-up/pull-down resistor connection
		uint8_t GetPullUpPullDownEnable(uint8_t &resistorEnable);

		// Sets pull-up/pull-down resistor selection (0=pull-down, 1=pull-up)
		uint8_t SetPullUpPullDownSelection(uint8_t resistorSelection);
		// Gets current pull-up/pull-down resistor selection
		uint8_t GetPullUpPullDownSelection(uint8_t &resistorSelection);

		// Sets interrups mask (1=masked, 0=interrupt passed)
		uint8_t SetInterruptMask(uint8_t intMask);
		// Get current interrupt mask setting
		uint8_t GetInterruptMask(uint8_t &intMask);

		// Gets interrupt status register (1=port fired the interrupt, 0=No interrupt on that port)
		uint8_t GetInterruptStatus(uint8_t &intStatus);

		// Sets output port configuration (0=push-pull, 1=open-drain)
		// Should be called before configuring the ports as outputs
		uint8_t SetOutputPortConfiguration(uint8_t outPortConfig);
		// Gets current output port configuration
		uint8_t GetOutputPortConfiguration(uint8_t &outPortConfig);
};

#endif
