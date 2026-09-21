#ifndef __EBF_HAL_STTS22H_H__
#define __EBF_HAL_STTS22H_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Core.h"
#include "../Core/EBF_I2CDevice.h"

// This class implements access to STTS22H chip, I2C temperature sensor chip with interrupts
class EBF_HAL_STTS22H : public EBF_I2CDevice {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_HAL_STTS22H");

	public:
		static const uint8_t defaultI2CAddress = 0x3F;

		EBF_HAL_STTS22H(EBF_I2C *i2cInterface);
		EBF_HAL_STTS22H(EBF_I2C &i2cInterface) : EBF_HAL_STTS22H(&i2cInterface) { }

		uint8_t Init(uint8_t i2cAddress = defaultI2CAddress);

	private:
		// Registers
		const uint8_t regTempHighLimit 	= 0x02;
		const uint8_t regTempLowLimit 	= 0x03;
		const uint8_t regControl 		= 0x04;
		const uint8_t regStatus			= 0x05;
		const uint8_t regTempOutput		= 0x06;

		// Control register
		typedef union {
			struct {
				uint8_t oneShot		: 1;
				uint8_t timeOutDis	: 1;
				uint8_t freeRun		: 1;
				uint8_t addrInc		: 1;
				uint8_t avg			: 2;
				uint8_t bdu			: 1;
				uint8_t mode_1Hz	: 1;
			} fields;
			uint8_t reg;
		} ControlRegister_t;

		// Status register
		typedef union {
			struct {
				uint8_t busy			: 1;
				uint8_t overThreshold	: 1;
				uint8_t underThreshold	: 1;
				uint8_t notUsed			: 5;
			} fields;
			uint8_t reg;
		} StatusRegister_t;

		uint8_t GetControlRegister(ControlRegister_t &ctrl);
		uint8_t SetControlRegister(ControlRegister_t ctrl);
		uint8_t GetStatusRegister(StatusRegister_t &status);

	public:
		// APIs

		typedef enum : uint8_t {
			AVERAGING_25HZ =	0x00,
			AVERAGING_50HZ =	0x01,
			AVERAGING_100HZ =	0x02,
			AVERAGING_200HZ =	0x03,
		} AveragingFrequency;

		uint8_t PowerDown();
		uint8_t SetOneShotMode();
		uint8_t Set1HzMode();
		uint8_t SetFreeRunMode(AveragingFrequency freq);
		uint8_t IsBusy();
		uint8_t GetValueRaw(int16_t &value);

		uint8_t SetThresholdHigh(float temp);
		uint8_t SetThresholdLow(float temp);
		uint8_t DisableThresholdHigh();
		uint8_t DisableThresholdLow();

		uint8_t GetIntFlags(uint8_t &highThreshold, uint8_t &lowThreshold);
};

#endif
