#include "EBF_I2CDevice.h"
#include "EBF_Core.h"

uint8_t EBF_I2CDevice::Read8bitRegister(uint8_t regAddress, uint8_t& value)
{
	uint8_t rc = EBF_OK;

	noInterrupts();
	do {
		pI2C->beginTransmission(i2cAddress);
		pI2C->write(regAddress);
		rc = pI2C->endTransmission(false);
		if (rc != 0) {
			rc = EBF_COMMUNICATION_PROBLEM;
			EBF_REPORT_ERROR(rc);
			break;
		}

		pI2C->requestFrom(i2cAddress, 1);

		value = pI2C->read();
	} while (0);
	interrupts();

	return EBF_OK;
}

uint8_t EBF_I2CDevice::Write8bitRegister(uint8_t regAddress, uint8_t value)
{
	uint8_t rc = EBF_OK;

	noInterrupts();
	do {
		pI2C->beginTransmission(i2cAddress);
		pI2C->write(regAddress);
		pI2C->write(value);
		rc = pI2C->endTransmission();
		if (rc != 0) {
			rc = EBF_COMMUNICATION_PROBLEM;
			EBF_REPORT_ERROR(rc);
			break;
		}
	} while (0);
	interrupts();

	return rc;
}

uint8_t EBF_I2CDevice::Read16bitRegister(uint8_t regAddress, uint16_t& value)
{
	uint8_t rc = EBF_OK;
	uint8_t readVal = 0;

	noInterrupts();
	do {
		pI2C->beginTransmission(i2cAddress);
		pI2C->write(regAddress);
		rc = pI2C->endTransmission(false);
		if (rc != 0) {
			rc = EBF_COMMUNICATION_PROBLEM;
			break;
		}

		pI2C->requestFrom(i2cAddress, 2);

		readVal = pI2C->read();
		value = readVal;

		readVal = pI2C->read();
		value |= readVal << 8;
	} while (0);
	interrupts();

	return EBF_OK;
}

uint8_t EBF_I2CDevice::Write16bitRegister(uint8_t regAddress, uint16_t value)
{
	uint8_t rc = EBF_OK;

	noInterrupts();
	do {
		pI2C->beginTransmission(i2cAddress);
		pI2C->write(regAddress);
		pI2C->write(value & 0xFF);
		pI2C->write((value>>8) & 0xFF);
		rc = pI2C->endTransmission();
		if (rc != 0) {
			rc = EBF_COMMUNICATION_PROBLEM;
			EBF_REPORT_ERROR(rc);
			break;
		}
	} while (0);
	interrupts();

	return rc;
}


uint8_t EBF_I2CDevice::Read32bitRegister(uint8_t regAddress, uint32_t& value)
{
	uint8_t rc = EBF_OK;
	uint8_t readVal = 0;

	noInterrupts();
	do {
		pI2C->beginTransmission(i2cAddress);
		pI2C->write(regAddress);
		rc = pI2C->endTransmission(false);
		if (rc != 0) {
			rc = EBF_COMMUNICATION_PROBLEM;
			break;
		}

		pI2C->requestFrom(i2cAddress, 4);

		readVal = pI2C->read();
		value = readVal;

		readVal = pI2C->read();
		value |= readVal << 8;

		readVal = pI2C->read();
		value |= readVal << 16;

		readVal = pI2C->read();
		value |= readVal << 24;
	} while (0);
	interrupts();

	return EBF_OK;
}

uint8_t EBF_I2CDevice::Write32bitRegister(uint8_t regAddress, uint32_t value)
{
	uint8_t rc = EBF_OK;

	noInterrupts();
	do {
		pI2C->beginTransmission(i2cAddress);
		pI2C->write(regAddress);
		pI2C->write(value & 0xFF);
		pI2C->write((value>>8) & 0xFF);
		pI2C->write((value>>16) & 0xFF);
		pI2C->write((value>>24) & 0xFF);
		rc = pI2C->endTransmission();
		if (rc != 0) {
			rc = EBF_COMMUNICATION_PROBLEM;
			EBF_REPORT_ERROR(rc);
			break;
		}
	} while (0);
	interrupts();

	return rc;
}
