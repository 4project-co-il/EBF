#include "EBF_PlugAndPlayHub.h"
#include "EBF_Logic.h"

EBF_PlugAndPlayHub::EBF_PlugAndPlayHub()
{
	this->type = HAL_Type::PnP;
}

uint8_t EBF_PlugAndPlayHub::Init(EBF_PlugAndPlayHub *pParentHub, uint8_t parentPort, PnP_DeviceInfo &deviceInfo, uint8_t *pParams)
{
	uint8_t rc;

	this->switchI2CAddress = 0;
	this->interruptControllerI2CAddress = 0;
	this->numberOfPorts = deviceInfo.numberOfPorts;

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

	// Fix type and ID after the EBF_Instance init
	this->type = HAL_Type::PnP;
	this->id = deviceInfo.deviceId;
	this->pollIntervalMs = EBF_NO_POLLING;	// No polling is needed for HUBs

	// Allocate pointers to HAL instances. Will be used to pass the interrrupts to connected instances
	pConnectedInstances = (EBF_HalInstance**)malloc(sizeof(EBF_HalInstance*) * numberOfPorts);
	if(pConnectedInstances == NULL) {
		return EBF_NOT_ENOUGH_MEMORY;
	}

	memset(pConnectedInstances, 0, sizeof(EBF_HalInstance*) * numberOfPorts);

	// Endpoints specify switch chip and interrupt controller (if exist)
	// Addresses should be shifted by the routing level to prevent collision between the hubs
	for (uint8_t i=0; i<deviceInfo.numberOfEndpoints; i++) {
		if (deviceInfo.endpointData[i].endpointId == 0) {
			// 0 is for the HUB switch
			this->switchI2CAddress = deviceInfo.endpointData[i].i2cAddress + this->routingLevel;
		}
		if (deviceInfo.endpointData[i].endpointId == 1) {
			// 1 is for the HUB interrupt controller
			this->interruptControllerI2CAddress = deviceInfo.endpointData[i].i2cAddress + this->routingLevel;
		}
	}

	// Copy interrupt mapping from the params, if specified
	// It will be used later by the embedded HUBs to connect needed interrrupt lines
	memset(interruptMapping, 0, sizeof(interruptMapping));
	if (deviceInfo.paramsLength > 0) {
		memcpy(interruptMapping, pParams, deviceInfo.paramsLength);
	}

	return EBF_OK;
}

uint8_t EBF_PlugAndPlayHub::AttachInterrupt(uint8_t portNumber, PnP_InterruptMode int1Mode, PnP_InterruptMode int2Mode)
{
	uint8_t rc;

	// For embedded HUBs without interrupt controller, attach specified interrupts to the EBF logic
	// Additional parameters will specify ports to interrupt lines mapping for embedded HUBs
	if (this->GetId() == PnP_DeviceId::PNP_ID_EMBEDDED_HUB && interruptControllerI2CAddress == 0) {
		EBF_Logic *pLogic = EBF_Logic::GetInstance();

		// interrupt hint will include port number shifted one bit left and the LSB specifying
		// if it's the first interrupt for the device or the second
		if (interruptMapping[portNumber*2 + 0] != (uint8_t)(-1) &&
			int1Mode != PNP_NO_INTERRUPT) {
			rc = pLogic->AttachInterrupt(interruptMapping[portNumber*2 + 0], this, GetArduinoInterruptMode(int1Mode), (portNumber<<1));

			if (rc != EBF_OK) {
				return rc;
			}
		}

		if (interruptMapping[portNumber*2 + 1] != (uint8_t)(-1) &&
			int2Mode != PNP_NO_INTERRUPT) {
			rc = pLogic->AttachInterrupt(interruptMapping[portNumber*2 + 1], this, GetArduinoInterruptMode(int2Mode), (portNumber<<1) + 1);

			if (rc != EBF_OK) {
				return rc;
			}
		}
	} else {
		// This HUB have interrupt controller
		// Connect parent HUBs first, then open the interrupt controller port
		rc = pParentHub->AttachInterrupt(parentPortNumber, int1Mode, int2Mode);
		if (rc != EBF_OK) {
			return rc;
		}
		// TODO: Configure portNumber lines for specified modes
	}

	return EBF_OK;
}

// We can't rely that Arduino's enumeration will not change some day
// So we'll have our own enumaration in the EEPROM and convert it to Arduino's equivalent when needed
uint8_t EBF_PlugAndPlayHub::GetArduinoInterruptMode(PnP_InterruptMode intMode)
{
	switch (intMode)
	{
	case PNP_NO_INTERRUPT:
		// should not happen
		return (uint8_t)(-1);

	case PNP_INTERRUPT_ON_CHANGE:
		return CHANGE;

	case PNP_INTERRUPT_LOW:
		return LOW;

	case PNP_INTERRUPT_HIGH:
		return HIGH;

	case PNP_INTERRUPT_RISING:
		return RISING;

	case PNP_INTERRUPT_FALLING:
		return FALLING;

	default:
		// Should not happen
		return (uint8_t)(-1);
		break;
	}
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

		if (pConnectedInstances[port] != NULL) {
			pConnectedInstances[port]->ProcessInterrupt();
		}
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
