#ifndef __EBF_HAL_TMP102_H__
#define __EBF_HAL_TMP102_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Core.h"
#include "../Core/EBF_I2CDevice.h"

// This class implements access to TMP102 chip, I2C temperature sensor chip with interrupts
class EBF_HAL_TMP102 : public EBF_I2CDevice {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_HAL_TMP102");

	public:
		static const uint8_t defaultI2CAddress = 0x48;

		EBF_HAL_TMP102(EBF_I2C *i2cInterface);
		EBF_HAL_TMP102(EBF_I2C &i2cInterface) : EBF_HAL_TMP102(&i2cInterface) { }

		uint8_t Init(uint8_t i2cAddress = defaultI2CAddress);

	private:
		// Registers
		const uint8_t regTemperature	= 0x00;
		const uint8_t regConfiguration	= 0x01;
		const uint8_t regTLow			= 0x02;
		const uint8_t regTHigh			= 0x03;

		// Configuration register
		typedef union {
			struct {
				uint16_t shutDownMode			: 1;
				uint16_t thermostatMode			: 1;
				uint16_t alertPolarity			: 1;
				uint16_t faultQueue				: 2;
				uint16_t converterResolution	: 2;
				uint16_t oneShotMode			: 1;
				uint16_t reserved				: 4;
				uint16_t extendedMode			: 1;
				uint16_t alertBit				: 1;
				uint16_t conversionRate			: 2;
			} fields;
			uint16_t reg;
		} ConfigurationRegister_t;

		uint8_t GetConfigurationRegister(ConfigurationRegister_t &config);
		uint8_t SetConfigurationRegister(ConfigurationRegister_t config);

		typedef union {
			struct {
				uint8_t MSB;
				uint8_t LSB;
			} fields;
			uint16_t reg;
		} TemperatureRegister_t;

	public:
		// APIs

		typedef enum : uint8_t {
			CONVERSION_RATE_0_25HZ =	0x00,
			CONVERSION_RATE_1HZ =		0x01,
			CONVERSION_RATE_4HZ =		0x02,
			CONVERSION_RATE_8HZ =		0x03,
		} ConversionRate;

		typedef enum : uint8_t {
			FAULTS_1	= 0,
			FAULTS_2	= 1,
			FAULTS_4	= 2,
			FAULTS_6	= 3,
		} ConsecutiveFaults;

		uint8_t PowerDown();
		uint8_t SetOneShotMode();
		uint8_t SetConversionRate(ConversionRate rate);
		uint8_t IsBusy();
		uint8_t GetValue(float &value);

		uint8_t SetConsecutiveFaults(ConsecutiveFaults faults);
		uint8_t GetConsecutiveFaults(ConsecutiveFaults &faults);

		uint8_t SetThresholdHigh(float value);
		uint8_t SetThresholdLow(float value);
		uint8_t GetThresholdHigh(float &value);
		uint8_t GetThresholdLow(float &value);
};

#endif
