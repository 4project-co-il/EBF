#include <Arduino.h>
#include "EBF.h"
#include "EBF_PlugAndPlay.h"
#include "EBF_PlugAndPlayManager.h"

// EBF objects creation, should be global
EBF_Core EBF;
PnP_StatusLed led;

void setup()
{
	uint8_t rc;

	// EBF is the first thing that should be initialized, with the maximum timers to be used
	EBF.Init();

	// Status LED initialization
	led.Init();

	// Embedded HUB EEPROM programming code for SAMD21 Development board
	PnP_DeviceInfo deviceInfo;
	uint8_t interruptMapping[8*2];		// 8 ports, 2 interrupts per port
	memset(&deviceInfo, 0, sizeof(PnP_DeviceInfo));

	deviceInfo.headerId = 0x506E502A;	// "PnP*"
	deviceInfo.version = 1;
	deviceInfo.deviceIDs[0] = PNP_ID_EMBEDDED_HUB;
	deviceInfo.numberOfPorts = 8;
	deviceInfo.numberOfEndpoints = 1;
	deviceInfo.endpointData[0].endpointId = 0;
	deviceInfo.endpointData[0].i2cAddress = 0x70;		// TCA9548A I2C mux address
	deviceInfo.paramsLength = 16;

	interruptMapping[0*2 + 0] = 11;
	interruptMapping[0*2 + 1] = 12;
	interruptMapping[1*2 + 0] = 6;
	interruptMapping[1*2 + 1] = 7;
	interruptMapping[2*2 + 0] = A5;
	interruptMapping[2*2 + 1] = 31;
	interruptMapping[3*2 + 0] = A1;
	interruptMapping[3*2 + 1] = A2;
	interruptMapping[4*2 + 0] = A3;
	interruptMapping[4*2 + 1] = A4;
	interruptMapping[5*2 + 0] = 8;
	interruptMapping[5*2 + 1] = 9;
	interruptMapping[6*2 + 0] = 13;
	interruptMapping[6*2 + 1] = 3;
	interruptMapping[7*2 + 0] = 5;
	interruptMapping[7*2 + 1] = 2;

	rc = EBF_PlugAndPlayManager::WriteDeviceEEPROM(0x50 + EBF_PlugAndPlayManager::PNP_EEPROM_MAIN_HUB, deviceInfo, interruptMapping, sizeof(interruptMapping));
	if (rc == EBF_OK) {
		led.Blink(100, 900);
	} else {
		led.Blink(100, 100);
	}
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}