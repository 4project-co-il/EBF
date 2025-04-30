#include "EBF_PlugAndPlayI2C.h"

EBF_PlugAndPlayI2C::EBF_PlugAndPlayI2C(EBF_I2C &i2c, EBF_PlugAndPlayHub* pHub, uint8_t port) : EBF_I2C(i2c)
{
	this->pHub = pHub;
	this->portNumber = port;
}

void EBF_PlugAndPlayI2C::beginTransmission(uint8_t address)
{
	// Switch all the hubs first
	pHub->SwitchToPort(*this, portNumber);

	i2c.beginTransmission(address);
}
