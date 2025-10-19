#ifndef __EBF_I2CDEVICE_H__
#define __EBF_I2CDEVICE_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_I2C.h"

class EBF_I2CDevice {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_I2CDevice");

	public:
		EBF_I2CDevice(EBF_I2C *pI2C) : pI2C(pI2C) {};

		EBF_I2C *pI2C;
		uint8_t i2cAddress;

		uint8_t Read8bitRegister(uint8_t regAddress, uint8_t& value);
		uint8_t Write8bitRegister(uint8_t regAddress, uint8_t value);
};

#endif
