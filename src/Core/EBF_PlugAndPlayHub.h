#ifndef __EBF_PLUGANDPLAY_HUB_H__
#define __EBF_PLUGANDPLAY_HUB_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_PlugAndPlayDevice.h"
#include "EBF_HalInstance.h"
#include "EBF_PlugAndPlayManager.h"

class EBF_PlugAndPlayManager;

class EBF_PlugAndPlayHub : protected EBF_HalInstance {
	public:
		friend class EBF_PlugAndPlayManager;

		static const uint8_t maxPorts = 8;

		EBF_PlugAndPlayHub();

		uint8_t Init(EBF_PlugAndPlayHub *pParentHub, uint8_t parentPort, PnP_DeviceInfo &deviceInfo, uint8_t *pParams);
		uint8_t SwitchToPort(EBF_I2C &pnpI2C, uint8_t portNumber);

	protected:
		uint8_t Process();
		void ProcessInterrupt();

		EBF_PlugAndPlayHub* pParentHub;
		uint8_t parentPortNumber;
		EBF_HalInstance** pConnectedInstances;
		uint8_t routingLevel;
		uint8_t numberOfPorts;
		uint8_t switchI2CAddress;
		uint8_t interruptControllerI2CAddress;
};

#endif
