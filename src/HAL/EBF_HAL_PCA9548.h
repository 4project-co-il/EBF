#ifndef __EBF_HAL_PCA9548_H__
#define __EBF_HAL_PCA9548_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Core.h"
#include "../Core/EBF_I2CDevice.h"

// This class implements access to PCA9548 chip, 8 channel I2C switch
class EBF_HAL_PCA9548 : public EBF_I2CDevice {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_HAL_PCA9548");

	public:
		static const uint8_t defaultI2CAddress = 0x70;

		EBF_HAL_PCA9548(EBF_I2C *i2cInterface);
		EBF_HAL_PCA9548(EBF_I2C &i2cInterface) : EBF_HAL_PCA9548(&i2cInterface) { }

	private:
		// Registers

	public:
		// APIs

		// Switch to specified port
		uint8_t Switch(uint8_t port);

		// Reset the switching
		uint8_t Reset() { return Switch(0); }
};

#endif
