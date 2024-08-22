#ifndef __EBF_PLUGANDPLAY_DEVICE_H__
#define __EBF_PLUGANDPLAY_DEVICE_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

// NVRAM structure:
// The used NVRAM chip have page sizes of 16 bytes, so sligning the data structures to 16 bytes is a good practice.
//
// First 16 bytes contain the DeviceInfo structure
// The headerId of the DeviceInfo should contain "PnP*" characters to indicate that's the PnP header
// The device info structure specifies the device ID, which is used to connect between the
// physical device connected to the PnP port to the relevant PnP class instance
// The DeviceInfo structure contains number of ports that the device provide, number of interrupts
// needed for the device's full functionality and number of endpoints the device have.
// Endpoint is basically a "chip" with it's own I2C address
// The DeviceInfo also specifies the length of the parameters that will follow after the initial structure
//
// Right after the 16 bytes of the initial structure there is an array of 8 possible enpoint data structures
// Every endpoint have it's own ID, which can be used by the device itself (for example, HUB switch chip have ID=0,
// HUB interrupt controller ID=1).
// Every endpoint might have a range of addresses, starting from the base I2C address up to the specified range.
// That allows having the same configuration for devices such as HUBs that might have multiple addresses.
//
// After the DeviceInfo structure there might be additional device specific parameters (HUB ports to interrupt lines mapping for example).
// The length of the parameters structure is defined by the paramsLength field in the DeviceInfo structure.

// 4 bytes
typedef enum : uint32_t {
	PNP_NO_DEVICE = 0,
	PNP_ID_EMBEDDED_HUB = 101,				// HUB with interrupt lines connected directly to the microcontroller
	PNP_ID_GENERIC_HUB,						// HUB with interrupt controller
	PNP_ID_STTS22H_TEMPERATURE_SENSOR,
} PnP_DeviceId;

// 2 bytes
typedef struct {
	uint8_t i2cAddress;			// Base of the I2C address
	uint8_t endpointId : 3;		// Specific to every device (up to 8 endpoints)
	uint8_t reserved1 : 5;
} PnP_EndpointData;

typedef enum : uint8_t {
	PNP_NO_INTERRUPT = 0,
	PNP_INTERRUPT_ON_CHANGE,
	PNP_INTERRUPT_LOW,
	PNP_INTERRUPT_HIGH,
	PNP_INTERRUPT_RISING,
	PNP_INTERRUPT_FALLING
} PnP_InterruptMode;

// 2kBit EEPROM is used for the device (256 bytes)

// 32 bytes
typedef struct {
	// 16 bytes for header data
	char headerId[4];					// 4 bytes
	uint8_t version;					// 1 byte - version of the data structure
	uint8_t numberOfPorts : 3;			// For HUBs (up to 8)
	uint8_t numberOfInterrupts : 2;		// Number of interrupts needed for the device (up to 2)
	uint8_t numberOfEndpoints : 3;		// Number of endpoints on the device (up to 8)
	uint8_t paramsLength;				// 1 byte - number of bytes for the parameters data after the endpoints
	uint8_t interrupt1Mode : 3;			// Mode of the first interrupt line (PnP_InterruptMode enumeration)
	uint8_t interrupt2Mode : 3;			// Mode of the second interrupt line (PnP_InterruptMode enumeration)
	uint8_t reserved1 : 2;
	uint32_t reserved2;
	uint32_t reserved3;
	// 32 bytes for deviceIDs
	PnP_DeviceId deviceIDs[8];			// up to 8 endpoints, 4 bytes per endpoint, 32 bytes
	// 16 bytes for endpointData
	PnP_EndpointData endpointData[8];	// up to 8 endpoints info, 2 bytes per endpoint, 16 bytes
	// Device additional parameters will be right after endpointData
} PnP_DeviceInfo;


#endif
