#include "EBF_HAL_TCAL9539.h"

EBF_HAL_TCAL9539::EBF_HAL_TCAL9539(EBF_I2C *i2cInterface) : EBF_I2CDevice(i2cInterface)
{
	i2cAddress = defaultI2CAddress;
}

// Returns input register
uint8_t EBF_HAL_TCAL9539::GetInput(uint16_t &input)
{
	uint8_t rc;

	rc = Read16bitRegister(regInputPort0, input);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Sets output register
uint8_t EBF_HAL_TCAL9539::SetOutput(uint16_t output)
{
	uint8_t rc;

	rc = Write16bitRegister(regOutputPort0, output);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Returns current output flip-flops state
uint8_t EBF_HAL_TCAL9539::GetOuput(uint16_t &output)
{
	uint8_t rc;

	rc = Read16bitRegister(regOutputPort0, output);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Sets polarity inversion (1=inverted, 0=non-inverted)
uint8_t EBF_HAL_TCAL9539::SetPolarityInversion(uint16_t inversion)
{
	uint8_t rc;

	rc = Write16bitRegister(regPolarityInversion0, inversion);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Returns current polarity inversion setting
uint8_t EBF_HAL_TCAL9539::GetPolarityInversion(uint16_t &inversion)
{
	uint8_t rc;

	rc = Read16bitRegister(regPolarityInversion0, inversion);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Sets ports configuration (1=input, 0=output)
uint8_t EBF_HAL_TCAL9539::SetConfiguration(uint16_t config)
{
	uint8_t rc;

	rc = Write16bitRegister(regConfiguration0, config);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Gets current port configuration
uint8_t EBF_HAL_TCAL9539::GetConfiguration(uint16_t &config)
{
	uint8_t rc;

	rc = Read16bitRegister(regConfiguration0, config);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Sets output drivers strength. 2bits for every port (00=0.25x, 01=0.50x, 10=0.75x, 11=1.0x)
uint8_t EBF_HAL_TCAL9539::SetOuputStrength(uint32_t strength)
{
	uint8_t rc;

	rc = Write32bitRegister(regOutputDriveStrength0_0, strength);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Gets current output drivers strength
uint8_t EBF_HAL_TCAL9539::GetOutputStrngth(uint32_t &strength)
{
	uint8_t rc;

	rc = Read32bitRegister(regOutputDriveStrength0_0, strength);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Sets input latch register (1=latch enabled, 0=no latch)
uint8_t EBF_HAL_TCAL9539::SetLatching(uint16_t latch)
{
	uint8_t rc;

	rc = Write16bitRegister(regInputLatch0, latch);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Gets current latch register setting
uint8_t EBF_HAL_TCAL9539::GetLatching(uint16_t &latch)
{
	uint8_t rc;

	rc = Read16bitRegister(regInputLatch0, latch);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Sets pull-up/pull-down resistor connection (0=no connection, 1=connected)
uint8_t EBF_HAL_TCAL9539::SetPullUpPullDownEnable(uint16_t resistorEnable)
{
	uint8_t rc;

	rc = Write16bitRegister(regPullUpPullDownEnable0, resistorEnable);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Gets current pull-up/pull-down resistor connection
uint8_t EBF_HAL_TCAL9539::GetPullUpPullDownEnable(uint16_t &resistorEnable)
{
	uint8_t rc;

	rc = Read16bitRegister(regPullUpPullDownEnable0, resistorEnable);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Sets pull-up/pull-down resistor selection (0=pull-down, 1=pull-up)
uint8_t EBF_HAL_TCAL9539::SetPullUpPullDownSelection(uint16_t resistorSelection)
{
	uint8_t rc;

	rc = Write16bitRegister(regPullUpPullDownSelection0, resistorSelection);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Gets current pull-up/pull-down resistor selection
uint8_t EBF_HAL_TCAL9539::GetPullUpPullDownSelection(uint16_t &resistorSelection)
{
	uint8_t rc;

	rc = Read16bitRegister(regPullUpPullDownSelection0, resistorSelection);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Sets interrups mask (1=masked, 0=interrupt passed)
uint8_t EBF_HAL_TCAL9539::SetInterruptMask(uint16_t intMask)
{
	uint8_t rc;

	rc = Write16bitRegister(regInterruptMask0, intMask);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Get current interrupt mask setting
uint8_t EBF_HAL_TCAL9539::GetInterruptMask(uint16_t &intMask)
{
	uint8_t rc;

	rc = Read16bitRegister(regInterruptMask0, intMask);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Gets interrupt status register (1=port fired the interrupt, 0=No interrupt on that port)
uint8_t EBF_HAL_TCAL9539::GetInterruptStatus(uint16_t &intStatus)
{
	uint8_t rc;

	rc = Read16bitRegister(regInterruptStatus0, intStatus);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Sets output port configuration (0=push-pull, 1=open-drain)
// Should be called before configuring the ports as outputs
uint8_t EBF_HAL_TCAL9539::SetOutputPortConfiguration(uint8_t outPortConfig)
{
	uint8_t rc;

	rc = Write8bitRegister(regOutputPortConfiguration, outPortConfig);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Gets current output port configuration
uint8_t EBF_HAL_TCAL9539::GetOutputPortConfiguration(uint8_t &outPortConfig)
{
	uint8_t rc;

	rc = Read8bitRegister(regOutputPortConfiguration, outPortConfig);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}
