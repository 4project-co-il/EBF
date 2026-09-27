#include "EBF_HAL_TMP102.h"
#include "../Core/EBF_Core.h"

EBF_HAL_TMP102::EBF_HAL_TMP102(EBF_I2C *i2cInterface) : EBF_I2CDevice(i2cInterface)
{
	EBF_I2CDevice::i2cAddress = defaultI2CAddress;
}

uint8_t EBF_HAL_TMP102::Init(uint8_t i2cAddress)
{
	uint8_t rc;
	ConfigurationRegister_t config;

	EBF_I2CDevice::i2cAddress = i2cAddress;

	// Reading the configuration register to reset the interrupt line
	rc = GetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Setting default values:
	// TM (Thermostat Mode) to Interrupt mode (1)
	config.fields.thermostatMode = 1;

	// POL (Polarity) to 0
	config.fields.alertPolarity = 0;

	// Fault Queue (how many times the measurement should pass the threshold in order to alert) = 0 (1 time)
	config.fields.faultQueue = 0;

	// Extended mode = 0, we're working in 12bit mode
	config.fields.extendedMode = 0;

	// Conversion rate to the default 4Hz value
	config.fields.conversionRate = ConversionRate::CONVERSION_RATE_4HZ;

	// write the configuration
	rc = SetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(EBF_OK);
}

// Returns Status register content
uint8_t EBF_HAL_TMP102::GetConfigurationRegister(ConfigurationRegister_t &config)
{
	uint8_t rc;

	rc = Read16bitRegister(regConfiguration, config.reg);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_TMP102::SetConfigurationRegister(ConfigurationRegister_t config)
{
	uint8_t rc;

	rc = Write16bitRegister(regConfiguration, config.reg);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_TMP102::GetValue(float &value)
{
	uint8_t rc;
	TemperatureRegister_t regValue = {};
	int16_t rawValue;

	rc = Read16bitRegister(regTemperature, regValue.reg);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Assemble temperature value for 12bit mode
	rawValue = regValue.fields.MSB << 4 | regValue.fields.LSB >> 4;

	// fill the MSB bits
	if (rawValue & 1<<11) {
		rawValue |= 0xF000;
	}

	value = rawValue * 0.0625;

	EBF_REPORT_AND_RETURN(EBF_OK);
}

uint8_t EBF_HAL_TMP102::PowerDown()
{
	uint8_t rc;
	ConfigurationRegister_t config = {};

	// Get current config values
	rc = GetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Set SD bit to 1
	config.fields.shutDownMode = 1;

	// Write the new config value back to the chip
	rc = SetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(EBF_OK);
}

uint8_t EBF_HAL_TMP102::SetOneShotMode()
{
	uint8_t rc;
	ConfigurationRegister_t config = {};

	// Get current config values
	rc = GetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Set OS bit to 1
	config.fields.oneShotMode = 1;

	// Write the new config value back to the chip
	rc = SetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(EBF_OK);
}

uint8_t EBF_HAL_TMP102::SetConversionRate(ConversionRate rate)
{
	uint8_t rc;
	ConfigurationRegister_t config = {};

	// Get current config values
	rc = GetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Set specified conversion rate
	config.fields.conversionRate = rate;

	// Clear the SD (ShutDown) bit
	config.fields.shutDownMode = 0;

	// Write the new config value back to the chip
	rc = SetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(EBF_OK);
}

uint8_t EBF_HAL_TMP102::IsBusy()
{
	uint8_t rc;
	ConfigurationRegister_t config = {};

	// Get current config values
	rc = GetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// From the datasheet:
	// When the device is in Shutdown Mode, writing a 1 to the OS bit starts a single temperature conversion.
	// During the conversion, the OS bit reads '0'. The device returns to the shutdown state at the completion
	// of the single conversion. After the conversion, the OS bit reads 1.
	return !config.fields.oneShotMode;
}

uint8_t EBF_HAL_TMP102::SetThresholdHigh(float value)
{
	uint8_t rc;
	TemperatureRegister_t regValue = {};
	int16_t rawValue;

	// 12bits are used to represent the temperature value in 2's complement format in 0.0625 degC units
	rawValue = (int16_t)(value / 0.0625);
	rawValue &= 0x0FFF;

	regValue.fields.LSB = (rawValue << 4) & 0xF0;
	regValue.fields.MSB = (rawValue >> 4) & 0xFF;

	rc = Write16bitRegister(regTHigh, regValue.reg);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_TMP102::SetThresholdLow(float value)
{
	uint8_t rc;
	TemperatureRegister_t regValue = {};
	int16_t rawValue;

	// 12bits are used to represent the temperature value in 2's complement format in 0.0625 degC units
	rawValue = (int16_t)(value / 0.0625);
	rawValue &= 0x0FFF;

	regValue.fields.LSB = (rawValue << 4) & 0xF0;
	regValue.fields.MSB = (rawValue >> 4) & 0xFF;

	rc = Write16bitRegister(regTLow, regValue.reg);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_TMP102::GetThresholdHigh(float &value)
{
	uint8_t rc;
	TemperatureRegister_t regValue = {};
	int16_t rawValue;

	rc = Read16bitRegister(regTHigh, regValue.reg);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Assemble temperature value for 12bit mode
	rawValue = regValue.fields.MSB << 4 | regValue.fields.LSB >> 4;

	// fill the MSB bits
	if (rawValue & 1<<11) {
		rawValue |= 0xF000;
	}

	value = rawValue * 0.0625;

	EBF_REPORT_AND_RETURN(EBF_OK);
}

uint8_t EBF_HAL_TMP102::GetThresholdLow(float &value)
{
	uint8_t rc;
	TemperatureRegister_t regValue = {};
	int16_t rawValue;

	rc = Read16bitRegister(regTLow, regValue.reg);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Assemble temperature value for 12bit mode
	rawValue = regValue.fields.MSB << 4 | regValue.fields.LSB >> 4;

	// fill the MSB bits
	if (rawValue & 1<<11) {
		rawValue |= 0xF000;
	}

	value = rawValue * 0.0625;

	EBF_REPORT_AND_RETURN(EBF_OK);
}

uint8_t EBF_HAL_TMP102::SetConsecutiveFaults(ConsecutiveFaults faults)
{
	uint8_t rc;
	ConfigurationRegister_t config = {};

	// Get current config values
	rc = GetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Set the consecutive faults number
	config.fields.faultQueue = faults;

	// Write the new config value back to the chip
	rc = SetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(EBF_OK);
}

uint8_t EBF_HAL_TMP102::GetConsecutiveFaults(ConsecutiveFaults &faults)
{
	uint8_t rc;
	ConfigurationRegister_t config = {};

	// Get current config values
	rc = GetConfigurationRegister(config);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	faults = (ConsecutiveFaults)config.fields.faultQueue;

	EBF_REPORT_AND_RETURN(EBF_OK);
}
