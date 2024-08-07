#include "EBF_PlugAndPlayHub.h"
#include "EBF_Logic.h"

uint8_t EBF_PlugAndPlayHub::Init(EBF_PlugAndPlayHub *pParentHub, uint8_t parentPort, PnP_DeviceInfo &deviceInfo, uint8_t *pParams)
{
	uint8_t rc;

	this->switchI2CAddress = 0;
	this->interruptControllerI2CAddress = 0;
	this->deviceId = deviceInfo.deviceId;

	// This class handles only the HUB devices
	if (deviceInfo.deviceId == PnP_DeviceId::PNP_ID_EMBEDDED_HUB &&
		deviceInfo.deviceId == PnP_DeviceId::PNP_ID_GENERIC_HUB) {
		return EBF_INVALID_STATE;
	}

	this->pParentHub = pParentHub;
	this->parentPortNumber = parentPort;

	if (pParentHub == NULL) {
		routingLevel = 0;
	} else {
		routingLevel = pParentHub->routingLevel + 1;
	}

	rc = EBF_HalInstance::Init(HAL_Type::I2C, parentPort);
	if (rc != EBF_OK) {
		return rc;
	}

	// Allocate pointers to HAL instances. Will be used to pass the interrrupts to connected instances
	pConnectedInstances = (EBF_HalInstance**)malloc(sizeof(EBF_HalInstance*) * numberOfPorts);
	if(pConnectedInstances == NULL) {
		return EBF_NOT_ENOUGH_MEMORY;
	}

	memset(pConnectedInstances, 0, sizeof(EBF_HalInstance*) * numberOfPorts);

	// Endpoints specify switch chip and interrupt controller (if exist)
	// Addresses should be shifted by the routing level to prevent collision between the hubs
	this->numberOfPorts = deviceInfo.numberOfEndpoints;
	for (uint8_t i=0; i<this->numberOfPorts; i++) {
		if (deviceInfo.endpointData[i].endpointId == 0) {
			// 0 is for the HUB switch
			this->switchI2CAddress = deviceInfo.endpointData[i].i2cAddress + this->routingLevel;
		}
		if (deviceInfo.endpointData[i].endpointId == 1) {
			// 1 is for the HUB interrupt controller
			this->interruptControllerI2CAddress = deviceInfo.endpointData[i].i2cAddress + this->routingLevel;
		}
	}

	EBF_Logic *pLogic = EBF_Logic::GetInstance();

	// For embedded HUBs without interrupt controller, attach specified interrupts to the EBF logic
	// Additional parameters will specify ports to interrupt lines mapping for embedded HUBs
	if (deviceId == PnP_DeviceId::PNP_ID_EMBEDDED_HUB && interruptControllerI2CAddress == 0) {
		for (uint8_t i=0; i<this->numberOfPorts; i+=2) {
			// interrupt hint will include port number shifted one bit left and the LSB specifying
			// if it's the first interrupt for the device or the second
			if (pParams[i] != (uint8_t)(-1)) {
				pLogic->AttachInterrupt(pParams[i], this, CHANGE, (i<<1));
			}
			if (pParams[i+1] != (uint8_t)(-1)) {
				pLogic->AttachInterrupt(pParams[i+1], this, CHANGE, (i<<1) + 1);
			}
		}
	}

	return EBF_OK;
}

uint8_t EBF_PlugAndPlayHub::Process()
{
	return EBF_OK;
}

void EBF_PlugAndPlayHub::ProcessInterrupt()
{
	// TODO: Pass the interrupt to the relevant PnP device
	if (interruptControllerI2CAddress != 0) {
		// Read the interrupt controller

		// Call the relevant HAL instance ProcessInterrupt
	} else {
		// Embedded HUB instance without interrupt controller
		EBF_Logic *pLogic = EBF_Logic::GetInstance();
		uint8_t port = pLogic->GetInterruptHint() >> 1;

		pConnectedInstances[port]->ProcessInterrupt();
	}
}

uint8_t EBF_PlugAndPlayHub::SwitchToPort(EBF_I2C &pnpI2C, uint8_t portNumber)
{
	uint8_t rc;

	if (switchI2CAddress == 0) {
		// There is no switch for that HUB, just return OK
		return EBF_OK;
	} else {
		if (pParentHub != NULL) {
			// Switch parent HUBs first (from the main HUB up to this)
			rc = pParentHub->SwitchToPort(pnpI2C, parentPortNumber);
			if (rc != EBF_OK) {
				return rc;
			}
		}

		pnpI2C.beginTransmission(switchI2CAddress + routingLevel);
		pnpI2C.write(portNumber);
		rc = pnpI2C.endTransmission();
		if (rc != 0) {
			return EBF_COMMUNICATION_PROBLEM;
		}
	}

	return EBF_OK;
}
