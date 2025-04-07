#include "PnP_Module_STTS22H_TemperatureSensor.h"
#include "../Core/EBF_PlugAndPlayManager.h"
#include "../Core/EBF_PlugAndPlayI2C.h"

PnP_Module_STTS22H_TemperatureSensor::PnP_Module_STTS22H_TemperatureSensor() : EBF_STTS22H_TemperatureSensor(NULL)
{
	this->type = HAL_Type::PnP_DEVICE;
	this->id = PnP_DeviceId::PNP_ID_STTS22H_TEMPERATURE_SENSOR;
}

uint8_t PnP_Module_STTS22H_TemperatureSensor::Init()
{
	uint8_t rc = EBF_OK;
	PnP_DeviceInfo deviceInfo;
	uint8_t endpointIndex;
	EBF_PlugAndPlayI2C *pPnPI2C;
	EBF_PlugAndPlayHub *pAssignedHub;

	EBF_PlugAndPlayManager *pPnpManager = EBF_PlugAndPlayManager::GetInstance();

	// Assign the current instance to physical PnP device and get all needed information
	rc = pPnpManager->AssignDevice(this, deviceInfo, endpointIndex, &pPnPI2C, &pAssignedHub);
	if(rc != EBF_OK) {
		return rc;
	}

	pI2C = pPnPI2C;

	// Initialize the device
	rc = EBF_STTS22H_TemperatureSensor::Init(deviceInfo.endpointData[endpointIndex].i2cAddress);
	if (rc != EBF_OK) {
		return rc;
	}

	// Fix type and ID after the EBF_Instance init
	this->type = HAL_Type::PnP_DEVICE;
	this->id = PnP_DeviceId::PNP_ID_STTS22H_TEMPERATURE_SENSOR;

	// Attach interrupt lines for that device
	rc = pAssignedHub->AssignInterruptLines(pPnPI2C->GetPortNumber(), endpointIndex, deviceInfo);
	if (rc != EBF_OK) {
		return rc;
	}

	return rc;
}
