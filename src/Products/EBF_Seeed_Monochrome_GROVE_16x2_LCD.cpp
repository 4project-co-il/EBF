#include "EBF_Seeed_Monochrome_GROVE_16x2_LCD.h"

// The i2cAddress should specify the device I2C address (0x3E default)
uint8_t EBF_Seeed_Monochrome_GROVE_16x2_LCD::Init(uint8_t i2cAddress)
{
	uint8_t rc;

	rc = EBF_HalInstance::Init(HAL_Type::I2C_INTERFACE, i2cAddress);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// This is output only device, polling is not needed
	SetPollingInterval(EBF_NO_POLLING);

	// Init the LCD to it's default settings
	rc = chip.Init(i2cAddress);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(EBF_OK);
}

// Called to process the instance after pollInterval
// Nothing to do for output only device
uint8_t EBF_Seeed_Monochrome_GROVE_16x2_LCD::Process()
{
	EBF_REPORT_AND_RETURN(EBF_OK);
}
