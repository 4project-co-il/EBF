#include <Arduino.h>

#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;

// Default Wire instance will be used for the following declaration:
EBF_I2C i2c;

// Other Wire instances can be specified in the constructor:
//EBF_I2C i2c(Wire2);


void setup()
{
	EBF.Init();

	// EBF_I2C initialization allows providing callback function that will be called when a data
	// on I2C bus will be available. Usage of such callback might be helpfull when there is a
	// very slow device that you are communicating with.
	// Generally the I2C devices are very fast with response, so there is no much usage for that
	// callback while using the EBF_I2C class directly. But it might be used for other EBF products
	// implementations.

	// EBF_I2C initialization in master mode (no address should be specified):
	i2c.Init();

	// EBF_I2C initialization in slave mode (address is specified, callback function cab be skipped with NULL pointer)
	//i2c.Init(NULL, 0x55);

	// I2C bus clock can be changed by calling:
	i2c.SetClock(400000);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
