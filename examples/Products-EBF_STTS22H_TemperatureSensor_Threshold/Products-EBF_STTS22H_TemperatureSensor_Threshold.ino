#include <Arduino.h>
#include "wiring_private.h" // pinPeripheral() function

#include "EBF_Products.h"

// In that example we will use SERCOM2 on the SparkFun SAMD21 Mini board
TwoWire Wire2(&sercom2, 4, 3);

// EBF objects creation, should be global
EBF_Core EBF;

// EBF_I2C instance will use SERCOM2 assigned to Wire2 instance
EBF_I2C i2c(Wire2);
// If you would like to use the default Wire instance, on Arduino UNO for eaxmple:
//EBF_I2C i2c;

// EBF serial object will use the Arduino's Serial USB for communication
EBF_Serial serial;

// Initialize STTS22H temperature sensor communicating via EBF_I2C instance
EBF_STTS22H_TemperatureSensor tempSensor(i2c);

void onTemperatureHigh()
{
	float tempC;

	serial.println("Temperature high alert");

	tempC = tempSensor.GetValueC();
	serial.print("temperature (C): ");
	serial.println(tempC);
}

void onTemperatureLow()
{
	float tempC;

	serial.println("Temperature low alert");

	tempC = tempSensor.GetValueC();
	serial.print("temperature (C): ");
	serial.println(tempC);
}

void setup()
{
	EBF.Init();

	// Default Init is enough for printouts via Serial on 115200 boud speed
	serial.Init();

	// Using I2C with EBF products requires a stand-alone initialization since several products
	// might use the same I2C interface, hence there is no need to do multiple initializations
	// from within the product's Init
	i2c.Init();
	i2c.SetClock(400000);

	// PIN re-assignment is required since SERCOM2 lines were configured for other usage by the Arduino code
	// You don't need those calls if you're using the defauld Wire instance
	pinPeripheral(4, PIO_SERCOM_ALT);
	pinPeripheral(3, PIO_SERCOM_ALT);

	// Initialize the temperature sensor on I2C address 0x3C
	tempSensor.Init(0x3C);

	// Register high and low thresholds callbacks
	tempSensor.SetOnThresholdHigh(onTemperatureHigh);
	tempSensor.SetOnThresholdLow(onTemperatureLow);

	// Set threshold levels
	tempSensor.SetThresholdHigh(28.0);
	tempSensor.SetThresholdLow(26.6);

	// The operation mode can be changed any time during the execurion
	// Polling interval will be set accordingly to the operation mode
	// For example, for 1Hz mode, polling will be 1000mSec
	tempSensor.SetOperationMode(EBF_STTS22H_TemperatureSensor::OperationMode::MODE_1HZ);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
