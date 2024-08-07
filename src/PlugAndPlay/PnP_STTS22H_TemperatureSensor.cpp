#include "PnP_STTS22H_TemperatureSensor.h"
#include "../Core/EBF_PlugAndPlayManager.h"

uint8_t PnP_STTS22H_TemperatureSensor::Init()
{
	uint8_t rc = EBF_OK;
	PnP_DeviceInfo deviceInfo;

	EBF_PlugAndPlayManager *pPnpManager = EBF_PlugAndPlayManager::GetInstance();

	// Assign the current instance to physical PnP device and get all needed information
	rc = pPnpManager->AssignDevice(this, deviceInfo, &pI2C);
	if(rc != EBF_OK) {
		return rc;
	}

	// Initialize the device
	rc = EBF_STTS22H_TemperatureSensor::Init(deviceInfo.endpointData[0].i2cAddress);
	if (rc != EBF_OK) {
		return rc;
	}

	return rc;
}
