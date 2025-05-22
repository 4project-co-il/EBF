#include "EBF_PlugAndPlayHub.h"
#include "EBF_Logic.h"

EBF_PlugAndPlayHub::EBF_PlugAndPlayHub()
{
	this->type = HAL_Type::PnP_DEVICE;
}

uint8_t EBF_PlugAndPlayHub::Init(EBF_PlugAndPlayHub *pParentHub, uint8_t parentPort, PnP_DeviceInfo &deviceInfo, uint8_t *pParams)
{
	uint8_t rc;

	this->switchI2CAddress = 0;
	this->interruptControllerI2CAddress = 0;
	this->numberOfPorts = deviceInfo.numberOfPorts;

	// This class handles only the HUB devices
	if (deviceInfo.deviceIDs[0] != PnP_DeviceId::PNP_ID_EMBEDDED_HUB &&
		deviceInfo.deviceIDs[0] != PnP_DeviceId::PNP_ID_EXTENDER_HUB) {
		return EBF_INVALID_STATE;
	}

	this->pParentHub = pParentHub;
	this->parentPortNumber = parentPort;

	if (pParentHub == NULL) {
		// Main HUBs will be level 3
		routingLevel = 3;
	} else {
		// Generic HUBs will be levels 4,5,6,7
		routingLevel = pParentHub->routingLevel + 1;
	}

	rc = EBF_HalInstance::Init(HAL_Type::I2C_INTERFACE, parentPort);
	if (rc != EBF_OK) {
		return rc;
	}

	// Fix type and ID after the EBF_Instance init
	this->type = HAL_Type::PnP_DEVICE;
	this->id = deviceInfo.deviceIDs[0];
	this->pollIntervalMs = EBF_NO_POLLING;	// No polling is needed for HUBs

	// Allocate pointers to HAL instances. Will be used to pass the interrrupts to connected instances
	// Allocate port info structure. HAL pointer will be used to pass the interrrupts to connected instances
	pPortInfo = (PortInfo*)malloc(sizeof(PortInfo) * numberOfPorts);
	if(pPortInfo == NULL) {
		return EBF_NOT_ENOUGH_MEMORY;
	}

	memset(pPortInfo, 0, sizeof(PortInfo) * numberOfPorts);

	// We do not initialize port info data since we don't know what is connected to those ports yet

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

uint8_t EBF_PlugAndPlayHub::AssignEmbeddedHubLine(uint8_t pinNumber, PnP_InterruptMode intMode, InterruptHint intHint)
{
	switch (intMode) {
		case PNP_NOT_CONECTED:
			// The line is not connected, nothing to do
			break;

		case PNP_DIGITAL_OUTPUT:
			// Initialize the line as digital output
			pinMode(pinNumber, OUTPUT);
			break;

		case PNP_DIGITAL_INPUT:
			// Initialize the line as digital input, but without an interrupt
			pinMode(pinNumber, INPUT);
			break;

		default:
			// For the rest of the cases attach the line as an interrupt

			// interrupt hint will include port number shifted one bit left and the LSB specifying
			// if it's the first interrupt for the device or the second
			if (pinNumber != (uint8_t)(-1)) {
				pinMode(pinNumber, INPUT);

				uint8_t rc;
				EBF_Logic *pLogic = EBF_Logic::GetInstance();

				rc = pLogic->AttachInterrupt(pinNumber, this, GetArduinoInterruptMode(intMode), intHint.uint32);

				if (rc != EBF_OK) {
					return rc;
				}
			}
	}

	return EBF_OK;

}

uint8_t EBF_PlugAndPlayHub::AssignInterruptLines(uint8_t portNumber, uint8_t endpointNumber, PnP_DeviceInfo &deviceInfo)
{
	uint8_t rc;
	InterruptHint hint;
	PnP_InterruptMode int1Mode = PnP_InterruptMode::PNP_NOT_CONECTED;
	PnP_InterruptMode int2Mode = PnP_InterruptMode::PNP_NOT_CONECTED;

	if (deviceInfo.interrupt1Endpoint == endpointNumber) {
		int1Mode = (PnP_InterruptMode)deviceInfo.interrupt1Mode;
	}

	if (deviceInfo.interrupt2Endpoint == endpointNumber) {
		int2Mode = (PnP_InterruptMode)deviceInfo.interrupt2Mode;
	}

	// For embedded HUBs without interrupt controller, attach specified interrupts to the EBF logic
	// Additional parameters will specify ports to interrupt lines mapping for embedded HUBs
	if (this->GetId() == PnP_DeviceId::PNP_ID_EMBEDDED_HUB && interruptControllerI2CAddress == 0) {
		// interrupt hint will include port number shifted one bit left and the LSB specifying
		// if it's the first interrupt for the device or the second
		// it helps passing the interrupt call to the correct HAL instance
		hint.uint32 = 0;
		hint.fields.interruptNumber = 0;
		hint.fields.portNumber = portNumber;
		hint.fields.endpointNumber = endpointNumber;

		rc = this->AssignEmbeddedHubLine(interruptMapping[portNumber*2 + 0], int1Mode, hint);
		if (rc != EBF_OK) {
			return rc;
		}

		hint.fields.interruptNumber = 1;
		rc = this->AssignEmbeddedHubLine(interruptMapping[portNumber*2 + 1], int2Mode, hint);
		if (rc != EBF_OK) {
			return rc;
		}
	} else {
		// This HUB have interrupt controller
		// Connect parent HUBs first, then open the interrupt controller port
		// HUBs are always on the first endpoint
		rc = pParentHub->AssignInterruptLines(parentPortNumber, 0, deviceInfo);
		if (rc != EBF_OK) {
			return rc;
		}

		// TODO: Configure portNumber lines for specified modes (input, output, interrupt)
	}

	return EBF_OK;
}

// We can't rely that Arduino's enumeration will not change some day
// So we'll have our own enumaration in the EEPROM and convert it to Arduino's equivalent when needed
uint8_t EBF_PlugAndPlayHub::GetArduinoInterruptMode(PnP_InterruptMode intMode)
{
	switch (intMode)
	{
	case PNP_NOT_CONECTED:
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
	uint8_t rc;

	// Pass the Process call to all the devices. Connected HUBs will pass it even further.
	for (uint8_t i=0; i<numberOfPorts; i++) {
		if (pPortInfo[i].numberOfEndpoints > 0) {
			for (uint8_t j=0; j<pPortInfo[i].numberOfEndpoints; j++) {
				if (pPortInfo[i].pConnectedInstanes[j] != 0) {
					rc = pPortInfo[i].pConnectedInstanes[j]->Process();

					if (rc != EBF_OK) {
						return rc;
					}
				}
			}
		}
	}

	return EBF_OK;
}

void EBF_PlugAndPlayHub::ProcessInterrupt()
{
	InterruptHint hint;

	// TODO: Pass the interrupt to the relevant PnP device
	if (interruptControllerI2CAddress != 0) {
		// Read the interrupt controller

		// Call the relevant HAL instance ProcessInterrupt
	} else {
		// Embedded HUB instance without interrupt controller
		EBF_Logic *pLogic = EBF_Logic::GetInstance();
		hint.uint32 = pLogic->GetInterruptHint();

		if (pPortInfo[hint.fields.portNumber].numberOfEndpoints > 0 &&
			pPortInfo[hint.fields.portNumber].pConnectedInstanes[hint.fields.endpointNumber] != 0) {
			pPortInfo[hint.fields.portNumber].pConnectedInstanes[hint.fields.endpointNumber]->ProcessInterrupt();
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

		// Casting to EBF_I2C to explicitly have that class implementation and not a derrived class
		((EBF_I2C)pnpI2C).beginTransmission(switchI2CAddress);
		((EBF_I2C)pnpI2C).write(1 << portNumber);
		rc = ((EBF_I2C)pnpI2C).endTransmission();
		if (rc != 0) {
			return EBF_COMMUNICATION_PROBLEM;
		}
	}

	return EBF_OK;
}

// Setting an interrupt line is possible only for device that declared that line as a Digital Output
uint8_t EBF_PlugAndPlayHub::SetIntLine(EBF_I2C &pnpI2C, uint8_t portNumber, uint8_t intLineNumber, uint8_t value)
{
	//uint8_t rc;

	// This is the main HUB without interrupt controller, the interrupt lines are directly connected to the MCU
	if (interruptControllerI2CAddress == 0) {
		// Set the corresponding line
		if (interruptMapping[portNumber*2 + intLineNumber] != (uint8_t)(-1)) {
			digitalWrite(interruptMapping[portNumber*2 + intLineNumber], value & 0x01);
		} else {
			return EBF_NOT_INITIALIZED;
		}
	} else {
/*
		if (pParentHub != NULL) {
			// Switch parent HUBs first (from the main HUB up to this)
			rc = pParentHub->SwitchToPort(pnpI2C, parentPortNumber);
			if (rc != EBF_OK) {
				return rc;
			}
		}
*/

		// TODO: Configure the interrupt controller to set the relevant line to the needed value
	}

	return EBF_OK;
}

// Setting both interrupt lines is possible only for device that declared those line as a Digital Outputs
uint8_t EBF_PlugAndPlayHub::SetIntLinesValue(EBF_I2C &pnpI2C, uint8_t portNumber, uint8_t value)
{
	//uint8_t rc;

	// This is the main HUB without interrupt controller, the interrupt lines are directly connected to the MCU
	if (interruptControllerI2CAddress == 0) {
		// Set the corresponding lines
		if (interruptMapping[portNumber*2 + 0] != (uint8_t)(-1)) {
			digitalWrite(interruptMapping[portNumber*2 + 0], value & 0x01);
		}

		if (interruptMapping[portNumber*2 + 1] != (uint8_t)(-1)) {
			digitalWrite(interruptMapping[portNumber*2 + 1], value & 0x02);
		}
	} else {
		// TODO: Configure the interrupt controller to set the relevant line to the needed value
/*
		if (pParentHub != NULL) {
			// Switch parent HUBs first (from the main HUB up to this)
			rc = pParentHub->SwitchToPort(pnpI2C, parentPortNumber);
			if (rc != EBF_OK) {
				return rc;
			}
		}

		// interruptControllerI2CAddress

		// set to value & 0x03
*/
	}

	return EBF_OK;
}

// Getting both interrupt lines values
uint8_t EBF_PlugAndPlayHub::GetIntLinesValue(EBF_I2C &pnpI2C, uint8_t portNumber, uint8_t &value)
{
//	uint8_t rc;
	value = 0;

	// This is the main HUB without interrupt controller, the interrupt lines are directly connected to the MCU
	if (interruptControllerI2CAddress == 0) {
		// Get the corresponding lines values
		if (interruptMapping[portNumber*2 + 0] != (uint8_t)(-1)) {
			value |= digitalRead(interruptMapping[portNumber*2 + 0]) & 0x01;
		}

		if (interruptMapping[portNumber*2 + 1] != (uint8_t)(-1)) {
			value |= ((digitalRead(interruptMapping[portNumber*2 + 1]) & 0x01) << 1);
		}
	} else {
		// TODO: Configure the interrupt controller to set the relevant line to the needed value
/*
		if (pParentHub != NULL) {
			// Switch parent HUBs first (from the main HUB up to this)
			rc = pParentHub->SwitchToPort(pnpI2C, parentPortNumber);
			if (rc != EBF_OK) {
				return rc;
			}
		}

		// interruptControllerI2CAddress
*/
	}

	return EBF_OK;
}

uint8_t EBF_PlugAndPlayHub::GetIntLine(EBF_I2C &pnpI2C, uint8_t portNumber, uint8_t intLineNumber, uint8_t &value)
{
//	uint8_t rc;
	value = 0;

	// Line can be only 0 or 1 (the interrupt line number)
	if (intLineNumber > 1) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	// This is the main HUB without interrupt controller, the interrupt lines are directly connected to the MCU
	if (interruptControllerI2CAddress == 0) {
		// Get the corresponding line
		if (interruptMapping[portNumber*2 + intLineNumber] != (uint8_t)(-1)) {
			value |= digitalRead(interruptMapping[portNumber*2 + intLineNumber]);
		} else {
			return EBF_NOT_INITIALIZED;
		}
	} else {
		// TODO: Configure the interrupt controller to set the relevant line to the needed value
/*
		if (pParentHub != NULL) {
			// Switch parent HUBs first (from the main HUB up to this)
			rc = pParentHub->SwitchToPort(pnpI2C, parentPortNumber);
			if (rc != EBF_OK) {
				return rc;
			}
		}

		// interruptControllerI2CAddress
*/
	}

	return EBF_OK;
}