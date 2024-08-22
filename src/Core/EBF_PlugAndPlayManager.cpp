#include <Arduino.h>
#include <Wire.h>
#include "wiring_private.h" // pinPeripheral() function
#include "EBF_PlugAndPlayManager.h"
#include "EBF_PlugAndPlayHub.h"
#include "EBF_PlugAndPlayI2C.h"

// EBF_PlugAndPlay implementation
EBF_PlugAndPlayManager *EBF_PlugAndPlayManager::pStaticInstance = NULL;

#if defined(PNP_USE_SAMD21_MINI_DIRECT_SERCOM2_INT10) || defined(PNP_USE_SAMD21_MINI_HUB)
// SERCOM2 is used
TwoWire pnpWire(&sercom2, 4, 3);
#endif


EBF_PlugAndPlayManager::EBF_PlugAndPlayManager() : pnpI2C(pnpWire)
{
}

EBF_PlugAndPlayManager *EBF_PlugAndPlayManager::GetInstance()
{
	if (pStaticInstance == NULL) {
		pStaticInstance = new EBF_PlugAndPlayManager();
		pStaticInstance->Init();
	}

	return pStaticInstance;
}

uint8_t EBF_PlugAndPlayManager::Init()
{
	uint8_t rc = EBF_OK;
	PnP_DeviceInfo deviceInfo;
	uint8_t interruptMapping[8*2] = { (uint8_t)(-1) };	// 2 interrupts per port, up to 8 ports

	pnpI2C.Init();
	pnpI2C.SetClock(400000);

#if defined(PNP_USE_SAMD21_MINI_HUB) || defined(PNP_USE_SAMD21_MINI_DIRECT_SERCOM2_INT10)
	// SERCOM2 is used
	// PIN re-assignment is required since SERCOM2 lines were configured for other usage by the Arduino code
	pinPeripheral(4, PIO_SERCOM_ALT);
	pinPeripheral(3, PIO_SERCOM_ALT);
#endif

	pMainHub = new EBF_PlugAndPlayHub();
	if (pMainHub == NULL) {
		return EBF_NOT_ENOUGH_MEMORY;
	}

#if defined(PNP_USE_SAMD21_MINI_DIRECT_SERCOM2_INT10)
	// Prepare device information for direct connection
	interruptMapping[0] = 10;
	interruptMapping[1] = (uint8_t)(-1);
	memset(&deviceInfo, 0, sizeof(PnP_DeviceInfo));

	deviceInfo.version = 1;
	deviceInfo.deviceIDs[0] = PNP_ID_EMBEDDED_HUB;
	deviceInfo.numberOfPorts = 1;
	deviceInfo.numberOfEndpoints = 0;
	deviceInfo.paramsLength = 2;			// 2 interrupts for one port

	rc = pMainHub->Init(NULL, 0, deviceInfo, interruptMapping);
	if (rc != EBF_OK) {
		return rc;
	}
#else

	// Read the configuration of the main hub
	rc = GetDeviceInfo(deviceInfo);
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	// There are parameters, for embedded HUBs those are interrupt mappings
	if (deviceInfo.paramsLength != 0) {
		rc = GetDeviceParameters(interruptMapping, min(deviceInfo.paramsLength, sizeof(interruptMapping));
		if (rc != 0) {
			return EBF_COMMUNICATION_PROBLEM;
		}
	}

	rc = pMainHub->Init(NULL, 0, deviceInfo, interruptMapping);
	if (rc != EBF_OK) {
		return rc;
	}

#endif

	rc = InitHubs(pMainHub);
	if (rc != EBF_OK) {
		return rc;
	}

	return rc;
}

uint8_t EBF_PlugAndPlayManager::InitHubs(EBF_PlugAndPlayHub *pHub)
{
	uint8_t rc;
	PnP_DeviceInfo deviceInfo;
	uint8_t parameters[32];

	// Loop over all the ports of that HUB
	for (uint8_t i=0; i<pHub->numberOfPorts; i++) {
		rc = pHub->SwitchToPort(pnpI2C, i);
		if (rc != EBF_OK) {
			// Something is wrong...
			return rc;
		}

		// Read a regular PnP device info
		rc = GetDeviceInfo(deviceInfo, 0);

		if (rc != EBF_OK) {
			// There is no PnP device connected to that port, check maybe there's another HUB set up for the next routing level
			if (pHub->routingLevel + 1 >= maxRoutingLevels) {
				// We're reached max routing levels
				continue;
			}

			// Try to get device for the next routing level
			rc = GetDeviceInfo(deviceInfo, pHub->routingLevel + 1);
			if (rc != EBF_OK) {
				// No HUB either, mark it with (-1) pointer, so it will be skipped in searches
				pHub->pConnectedInstances[i] = (EBF_HalInstance*)(-1);
				continue;
			}
		}

		// In case it's a HUB, create its new instance and connect all the pointers
		if (deviceInfo.deviceIDs[0] == PnP_DeviceId::PNP_ID_GENERIC_HUB) {
			// Read the parameters in case the configuration says so
			if (deviceInfo.paramsLength > 0) {
				rc = GetDeviceParameters(pHub->routingLevel + 1, &parameters[0], min(deviceInfo.paramsLength, sizeof(parameters)));
				if (rc != EBF_OK) {
					// Something is wrong... should not happen
					return rc;
				}
			}

			EBF_PlugAndPlayHub *pNewHub = new EBF_PlugAndPlayHub();

			rc = pNewHub->Init(pHub, i, deviceInfo, &parameters[0]);
			if (rc != EBF_OK) {
				return rc;
			}

			// Save the pointer
			pHub->pConnectedInstances[i] = pNewHub;

			// Initialize the new HUB connections
			InitHubs(pNewHub);
		}
	}

	return EBF_OK;
}

uint8_t EBF_PlugAndPlayManager::IsHeaderValid(PnP_DeviceInfo &deviceInfo)
{
	if (deviceInfo.headerId[0] != 'P' ||
		deviceInfo.headerId[1] != 'n' ||
		deviceInfo.headerId[2] != 'P' ||
		deviceInfo.headerId[3] != '*') {
			return 0;
		} else {
			return 1;
		}
}

uint8_t EBF_PlugAndPlayManager::GetDeviceInfo(PnP_DeviceInfo &deviceInfo, uint8_t routingLevel)
{
	uint8_t rc;

	// Read the device info
	pnpI2C.beginTransmission(eepromI2cAddress + routingLevel);
	// Device info resides at the beginning of the EEPROM
	pnpI2C.write(0);
	rc = pnpI2C.endTransmission(false);
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	pnpI2C.requestFrom(eepromI2cAddress + routingLevel, sizeof(PnP_DeviceInfo));

	rc = pnpI2C.readBytes((uint8_t*)&deviceInfo, sizeof(PnP_DeviceInfo));
	// Strange, shuuld not happen with PnP device, skip it
	if (rc != sizeof(PnP_DeviceInfo)) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	// This is not a PnP device, skip it
	if (!IsHeaderValid(deviceInfo)) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

uint8_t EBF_PlugAndPlayManager::GetDeviceParameters(uint8_t routingLevel, uint8_t *pParams, uint8_t maxSize)
{
	uint8_t rc;

	// No parameters should be read
	if (maxSize == 0) {
		return EBF_OK;
	}

	// Read the device parameters
	pnpI2C.beginTransmission(eepromI2cAddress + routingLevel);
	// Parameters are right after the device info structure
	pnpI2C.write(sizeof(PnP_DeviceInfo));
	rc = pnpI2C.endTransmission(false);
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	pnpI2C.requestFrom(eepromI2cAddress + routingLevel, sizeof(PnP_DeviceInfo));

	rc = pnpI2C.readBytes(pParams, maxSize);
	// Strange, should not happen with PnP device, skip it
	if (rc != maxSize) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

uint8_t EBF_PlugAndPlayManager::AssignDevice(
	EBF_HalInstance* pHalInstance,
	PnP_DeviceInfo& deviceInfo,
	uint8_t& endpointIndex,
	EBF_PlugAndPlayI2C** pI2CRouter,
	EBF_PlugAndPlayHub** pAssignedHub,
	EBF_PlugAndPlayHub* pHub)
{
	uint8_t rc;
	EBF_HalInstance *pConnectedInstance;

	if (pHub == NULL) {
		pHub = pMainHub;
	}

	// Check all the ports
	for (uint8_t i=0; i<pHub->numberOfPorts; i++) {
		pConnectedInstance = pHub->pConnectedInstances[i];

		// Nothing is connected to that port
		if (pConnectedInstance == (EBF_HalInstance*)(-1)) {
			continue;
		}

		rc = pHub->SwitchToPort(pnpI2C, i);
		if (rc != EBF_OK) {
			// Something is wrong
			return rc;
		}

		// If the connected device is a HUB, search there
		if (pConnectedInstance != NULL) {
			if (pConnectedInstance->GetId() == PnP_DeviceId::PNP_ID_GENERIC_HUB) {
				rc = AssignDevice(pHalInstance, deviceInfo, endpointIndex, pI2CRouter, pAssignedHub, (EBF_PlugAndPlayHub*)(pConnectedInstance));
				if (rc == EBF_OK) {
					// Device was found by inner HUB instance
					return EBF_OK;
				}
			}

			// That port was already assigned to a HAL instance
			continue;
		}

		// Check if current port is connected to the needed device
		rc = GetDeviceInfo(deviceInfo);

		if (rc != EBF_OK) {
			// Should not happen since we already communicated with that device before...
			continue;
		}

		// Check all endpoints
		for (endpointIndex=0; endpointIndex<maxEndpoints; endpointIndex++) {
			if (deviceInfo.deviceIDs[endpointIndex] == pHalInstance->GetId()) {
				// Found the matching endpoint
				break;
			} else {
				// Not the needed device
				continue;
			}
		}

		// No mathcing endpoints found in that device
		if (endpointIndex == maxEndpoints) {
			continue;
		}

		// We found the needed device
		// Store it's pointer in the HUB and create PnP I2C linkage that can route to the needed port
		pHub->pConnectedInstances[i] = pHalInstance;
		*pI2CRouter = new EBF_PlugAndPlayI2C(pnpI2C, pHub, i);
		*pAssignedHub = pHub;

		return EBF_OK;
	}

	return EBF_NOT_INITIALIZED;
}

uint8_t EBF_PlugAndPlayManager::WriteDeviceEEPROM(PnP_DeviceInfo &deviceInfo, uint8_t* pParams, uint8_t paramsSize)
{
	uint8_t rc;

	EBF_PlugAndPlayManager *pInstance = EBF_PlugAndPlayManager::GetInstance();
	uint8_t* pData = (uint8_t*)&deviceInfo;

	// Write page-by-page
	for (uint8_t i=0; i<sizeof(PnP_DeviceInfo)/eepromPageSize; i++) {
		uint8_t pageOffset = i*eepromPageSize;
		uint8_t size = sizeof(PnP_DeviceInfo) - pageOffset;

		if (size > eepromPageSize) {
			size = eepromPageSize;
		}

		rc = pInstance->WriteDeviceEepromPage(pageOffset, pData+pageOffset, size);

		if (rc != EBF_OK) {
			return rc;
		}
	}

	if(paramsSize > 0 && pParams != NULL) {
		for (uint8_t i=0; i<paramsSize/eepromPageSize; i++) {
			uint8_t pageOffset = i*eepromPageSize + sizeof(PnP_DeviceInfo);
			uint8_t size = paramsSize - pageOffset;

			if (size > eepromPageSize) {
				size = eepromPageSize;
			}

			rc = pInstance->WriteDeviceEepromPage(pageOffset, pParams+pageOffset, size);
			if (rc != EBF_OK) {
				return rc;
			}
		}
	}

	return EBF_OK;
}

uint8_t EBF_PlugAndPlayManager::WriteDeviceEepromPage(uint8_t address, uint8_t* pData, uint8_t size)
{
	uint8_t rc;

	pnpI2C.beginTransmission(eepromI2cAddress);
	pnpI2C.write(address);
	pnpI2C.write(pData, size);

	rc = pnpI2C.endTransmission(true);
	if (rc != 0) {
		return EBF_COMMUNICATION_PROBLEM;
	}

	// Add delay after each page. max write time is 5ms
	delay(5);

	return rc;
}