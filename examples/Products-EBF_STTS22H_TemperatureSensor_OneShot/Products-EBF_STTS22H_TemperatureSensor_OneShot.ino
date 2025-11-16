#include <Arduino.h>
#include "wiring_private.h" // pinPeripheral() function

#include "EBF_Products.h"

// In that example we will use SERCOM2 on the SparkFun SAMD21 Mini board
TwoWire Wire2(&sercom2, 4, 3);

// EBF objects creation, should be global
EBF_Core EBF;

// Timer creation
EBF_Timer timer;

// We will use the LED as a status indication
EBF_DigitalOutput led;

// EBF_I2C instance will use SERCOM2 assigned to Wire2 instance
EBF_I2C i2c(Wire2);
// If you would like to use the default Wire instance, on Arduino UNO for eaxmple:
//EBF_I2C i2c;

// EBF serial object will use the Arduino's Serial USB for communication
EBF_Serial serial;

// Initialize STTS22H temperature sensor communicating via EBF_I2C instance
EBF_STTS22H_TemperatureSensor tempSensor(i2c);

// The timer will be used to trigger one-shot measurement
void onTimer()
{
	// Initiate one-shot measurement.
	// It takes some time for the device to perform the measurement, so we can't read
	// the temperature right away. We could poll the IsBusy() function know when the
	// measurement is complete, but that task can be left for the EBF, which will call
	// the onMeasureComplete callback when the data will be awailable. During that time
	// the code could do something else.
	// The device will go back to POWER_DOWN state after the measurement
	tempSensor.SetOperationMode(EBF_STTS22H_TemperatureSensor::OperationMode::MODE_ONE_SHOT);

	// EBF timers are one-shot in nature, restart it
	timer.Start();
}

void onMeasureComplete()
{
	float tempC;

	tempC = tempSensor.GetValueC();
	serial.print("temperature (C): ");
	serial.println(tempC);

	if(tempC > 28.0) {
		led.SetValue(1);
		serial.println("Temperature too high!");
	} else {
		led.SetValue(0);
	}
}

void setup()
{
	EBF.Init();

	// Default Init is enough for printouts via Serial on 115200 boud speed
	serial.Init();

	// Initialize the timer for 5sec (5000 mSec), onTimer function will be called
	timer.Init(onTimer, 5000);

	// Start the timer for the first time
	timer.Start();

	// Built-in LED on digital output #13 will be used as an indication for a high temperature
	led.Init(LED_BUILTIN);

	// Using I2C with EBF products requires a stand-alone initialization since several products
	// might use the same I2C interface, hence there is no need to do multiple initializations
	// from within the product's Init
	i2c.Init();
	i2c.SetClock(400000);

	// PIN re-assignment is required since SERCOM2 lines were configured for other usage by the Arduino code
	// You don't need those calls if you're using the defauld Wire instance
	pinPeripheral(4, PIO_SERCOM_ALT);
	pinPeripheral(3, PIO_SERCOM_ALT);

	// Initialize the temperature sensor on I2C address 0x3C, initial state is POWER_DOWN
	tempSensor.Init(0x3C);

	// Register function to be called when on-shot measurement will be complete
	tempSensor.SetOnMeasureComplete(onMeasureComplete);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
