#include "EBF_HAL_PCA9548.h"

EBF_HAL_PCA9548::EBF_HAL_PCA9548(EBF_I2C *i2cInterface) : EBF_I2CDevice(i2cInterface)
{
	i2cAddress = defaultI2CAddress;
}

// Switch to specified port
uint8_t EBF_HAL_PCA9548::Switch(uint8_t port)
{
	uint8_t rc = EBF_OK;

	do {
		// Casting to EBF_I2C to explicitly have that class implementation and not a derrived class
		// Overwise additional switch might be called by the derrived class

		((EBF_I2C*)pI2C)->EBF_I2C::beginTransmission(i2cAddress);
		((EBF_I2C*)pI2C)->EBF_I2C::write(1<<port);
		rc = ((EBF_I2C*)pI2C)->EBF_I2C::endTransmission();
		if (rc != 0) {
			rc = EBF_COMMUNICATION_PROBLEM;
			EBF_REPORT_ERROR(rc);
			break;
		}
	} while (0);

	return rc;
}
