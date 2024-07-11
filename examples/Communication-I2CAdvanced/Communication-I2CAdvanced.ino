#include <Arduino.h>
#include "wiring_private.h" // pinPeripheral() function

#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;

// We will be using SERCOM2 communication module in SparkFun SAMD21 Mini board for that example
TwoWire Wire2(&sercom2, 4, 3);

// Other Wire instances can be specified in the constructor:
EBF_I2C i2c(Wire2);


void setup()
{
	EBF.Init();

	// EBF_I2C initialization allows providing callback function that will be called when a data
	// on I2C bus will be available. Usage of such callback might be helpfull when there is a
	// very slow device that you are communicating with.
	// Generally the I2C devices are very fast with response, so there is no much usage for that
	// callback while using the EBF_I2C class directly. But it might be used for other EBF products
	// implementations.

	// Althought the EBF_I2C implementation doesn't add anything special on top of the default Wire
	// implementation, it is needed for additional EBF classes and can serve as a wrapper for the I2C
	// connection that cab be changed to different communication module, as shown in that example,
	// without the need to change the rest of the code

	// EBF_I2C initialization in master mode (no address should be specified):
	i2c.Init();

	// PIN re-assignment is required since SERCOM2 lines were configured for other usage by the Arduino code
	pinPeripheral(4, PIO_SERCOM_ALT);
	pinPeripheral(3, PIO_SERCOM_ALT);

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
