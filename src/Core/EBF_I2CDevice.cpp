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
