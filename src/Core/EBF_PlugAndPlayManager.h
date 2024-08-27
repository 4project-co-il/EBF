#ifndef __EBF_PLUGANDPLAY_MANAGER_H__
#define __EBF_PLUGANDPLAY_MANAGER_H__

#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_PlugAndPlayDevice.h"
#include "EBF_I2C.h"
#include "EBF_PlugAndPlayHub.h"

class EBF_PlugAndPlayHub;
class EBF_PlugAndPlayI2C;

class EBF_PlugAndPlayManager {
	public:
		static const uint8_t maxRoutingLevels = 3;
		static const uint8_t maxEndpoints = 8;

		EBF_PlugAndPlayManager();

		uint8_t Init();

		static EBF_PlugAndPlayManager *GetInstance();

		EBF_I2C &GetI2CInterface() { return pnpI2C; }

		uint8_t AssignDevice(
			EBF_HalInstance* pHalInstance,
			PnP_DeviceInfo& deviceInfo,
			uint8_t& endpointIndex,
			EBF_PlugAndPlayI2C** pI2CRouter,
			EBF_PlugAndPlayHub** pAssignedHub,
			EBF_PlugAndPlayHub* pHub = NULL);

		static uint8_t WriteDeviceEEPROM(uint8_t i2cAddress, PnP_DeviceInfo &deviceInfo, uint8_t* pParams = NULL, uint8_t paramsSize = 0);

	private:
		EBF_PlugAndPlayHub* pMainHub;
		uint8_t IsHeaderValid(PnP_DeviceInfo &deviceInfo);
		uint8_t GetDeviceInfo(PnP_DeviceInfo &deviceInfo, uint8_t routingLevel = 0);
		uint8_t GetDeviceParameters(uint8_t routingLevel, uint8_t *pParams, uint8_t maxSize);

		uint8_t InitHubs(EBF_PlugAndPlayHub *pHub);

		uint8_t WriteDeviceEepromPage(uint8_t i2cAddress, uint8_t eepromAddress, uint8_t* pData, uint8_t size);



	private:
		static EBF_PlugAndPlayManager* pStaticInstance;
		// I2C interface
		EBF_I2C pnpI2C;
		static const uint8_t eepromI2cAddress = 0x50;
		static const uint8_t eepromPageSize = 16;

};

#endif
