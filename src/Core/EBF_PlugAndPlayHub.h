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
		uint8_t SwitchToPort(EBF_I2C* pPnpI2C, uint8_t portNumber);

		// Setting an interrupt line is possible only for device that declared that line as a Digital Output
		uint8_t SetIntLine(EBF_I2C* pPnpI2C, uint8_t portNumber, uint8_t intLineNumber, uint8_t value);
		uint8_t SetIntLinesValue(EBF_I2C* pPnpI2C, uint8_t portNumber, uint8_t value);
		uint8_t GetIntLine(EBF_I2C* pPnpI2C, uint8_t portNumber, uint8_t intLineNumber, uint8_t &value);
		uint8_t GetIntLinesValue(EBF_I2C* pPnpI2C, uint8_t portNumber, uint8_t &value);

		typedef union {
			struct {
				uint32_t interruptNumber : 1;
				uint32_t portNumber : 4;
				uint32_t endpointNumber : 4;
				uint32_t reserved : 23;
			} fields;
			uint32_t uint32;
		} InterruptHint;

		uint8_t AssignInterruptLines(uint8_t portNumber, uint8_t endpointNumber, PnP_DeviceInfo &deviceInfo);
		uint8_t AssignEmbeddedHubLine(uint8_t pinNumber, PnP_InterruptMode intMode, InterruptHint intHint);

		typedef struct {
			EBF_HalInstance** pConnectedInstanes;	// Array of HalInstance pointers assigned to the device connected to that port
			uint8_t numberOfEndpoints;				// Number of endpoints on the device connected to that port
		} PortInfo;

	protected:
		uint8_t Process();
		void ProcessInterrupt();

		uint8_t GetArduinoInterruptMode(PnP_InterruptMode intMode);

		EBF_PlugAndPlayHub* pParentHub;
		uint8_t parentPortNumber;
		uint8_t numberOfPorts;
		PortInfo* pPortInfo;
		uint8_t routingLevel;
		uint8_t switchI2CAddress;
		uint8_t interruptControllerI2CAddress;
		uint8_t interruptMapping[maxPorts * 2];		// For embedded HUBs, 2 interrupts for each port
};

#endif
