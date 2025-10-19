#include <Arduino.h>
#include "wiring_private.h" // pinPeripheral() function

#include "EBF_Products.h"

// That example shows how to use STTS22H with an interrupt line connected to
// Digital Input, which will notify when the temperature goes out of configured
// thresholds, so there is no need to poll the sensor every time.

// That example require to enable interrupts for the EBF environment.

// If you're using the PlatformIO environment, you could add the following line to the project platform.ini file:
// build_flags = -D EBF_USE_INTERRUPTS
// Since the Arduino IDE doesn't provide any way to add project wide definitions, there is additional way to add that
// flag to the compilation...
// In the library src directory you will find the "EBF_Config_TEMPLATE.h" file, which include all the configuration
// flags for the EBF library.
// If you're using PlatformIO, you can save the content of that file as "Project_Config.h" file in your project "include"
// directory.
// If you're using Arduino IDE, copy that file and save it's content as "Project_Config.h" file in the library "src" directory
//
// In the "Project_Config.h" file you will find the flag definition commented out:
// #define EBF_USE_INTERRUPTS
// Uncomment that file to have the interrupts code to be compiled into the EBF library


// Timers enumeration
enum {
	TIMER1 = 0,

	NUMBER_OF_TIMERS
};

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

void onTimer()
{
	serial.println(tempSensor.GetValueC());

	EBF.StartTimer(TIMER1);
}

void setup()
{
	EBF.Init(NUMBER_OF_TIMERS, 16);

	// Timer will just print current temperature for debug
	EBF.InitTimer(TIMER1, onTimer, 1000);
	EBF.StartTimer(TIMER1);

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
	// Interrupt line is connected to pin 10 on SparkFun'x SAMD21 Mini board
	tempSensor.AttachInterrupt(10);

	// Register high and low thresholds callbacks
	tempSensor.SetOnThresholdHigh(onTemperatureHigh);
	tempSensor.SetOnThresholdLow(onTemperatureLow);

	// Set threshold levels
	tempSensor.SetThresholdHigh(29.0);
	tempSensor.SetThresholdLow(27.0);

	// The operation mode can be changed any time during the execurion
	// Polling interval will be set accordingly to the operation mode
	// For example, for 1Hz mode, polling will be 1000mSec
	tempSensor.SetOperationMode(EBF_STTS22H_TemperatureSensor::OperationMode::MODE_1HZ);
	// We will use the interrupt line to trigger the threshold level notifications, so no polling is needed
	tempSensor.SetPollInterval(EBF_NO_POLLING);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
