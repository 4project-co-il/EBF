#ifndef __EBF_MODULE_8OUTPUTS_H__
#define __EBF_MODULE_8OUTPUTS_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_HalInstance.h"
#include "../HAL/EBF_HAL_PCA9685.h"

class EBF_Module_8Outputs : public EBF_HalInstance {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_Module_8Outputs");

	public:
		EBF_Module_8Outputs(EBF_I2C &i2cInterface) : chip(i2cInterface) {}
		EBF_Module_8Outputs(EBF_I2C* pI2cInterface) : chip(pI2cInterface) {}

		static const uint8_t numberOfOutputs = 8;

		uint8_t Init(uint16_t pwmFrequency = 50, uint8_t i2cAddress = EBF_HAL_PCA9685::defaultI2CAddress);

		// Sets boolean value on the specified output channel (1=HIGH, 0=LOW)
		uint8_t SetValue(uint8_t index, uint8_t value);
		// Sets PWM timing value on the specified output channel
		// float percent paramenter: 0=Complete OFF, 50=50% width ON/OFF, 100=Complete ON
		uint8_t SetPWM(uint8_t index, float pwmPercent);

		// Returns current value of the specified output line
		// float percent paramenter: 0=Complete OFF, 50=50% width ON/OFF, 100=Complete ON
		float GetValue(uint8_t index);

	protected:
		// I2C 16-channel PWM controller chip
		EBF_HAL_PCA9685 chip;

		// No processing is needed
		virtual uint8_t Process() { return EBF_OK; }
};

#endif
