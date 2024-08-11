#include "PnP_STTS22H_TemperatureSensor.h"
#include "../Core/EBF_PlugAndPlayManager.h"

PnP_STTS22H_TemperatureSensor::PnP_STTS22H_TemperatureSensor() : EBF_STTS22H_TemperatureSensor(NULL)
{
	this->type = HAL_Type::PnP;
	this->id = PnP_DeviceId::PNP_ID_STTS22H_TEMPERATURE_SENSOR;
}

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

	// Fix type and ID after the EBF_Instance init
	this->type = HAL_Type::PnP;
	this->id = PnP_DeviceId::PNP_ID_STTS22H_TEMPERATURE_SENSOR;

	return rc;
}
