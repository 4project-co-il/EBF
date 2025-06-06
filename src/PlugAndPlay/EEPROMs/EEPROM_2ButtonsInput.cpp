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

	// Device EEPROM programming code
	PnP_DeviceInfo deviceInfo;
	memset(&deviceInfo, 0, sizeof(PnP_DeviceInfo));

	deviceInfo.headerId = 0x506E502A;	// "PnP*"
	deviceInfo.version = 1;
	deviceInfo.deviceIDs[0] = PNP_ID_2BUTTONS_INPUT;
	deviceInfo.numberOfEndpoints = 1;
	deviceInfo.endpointData[0].endpointId = 1;
	deviceInfo.endpointData[0].i2cAddress = 0;
	deviceInfo.numberOfInterrupts = 2;
	deviceInfo.interrupt1Mode = PnP_InterruptMode::PNP_INTERRUPT_ON_CHANGE;
	deviceInfo.interrupt2Mode = PnP_InterruptMode::PNP_INTERRUPT_ON_CHANGE;

	rc = EBF_PlugAndPlayManager::WriteDeviceEEPROM(0x50 + EBF_PlugAndPlayManager::PNP_EEPROM_DEVICE, deviceInfo);
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