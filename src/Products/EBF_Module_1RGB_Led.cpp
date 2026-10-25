#include "EBF_Module_1RGB_Led.h"

// The i2cAddress should specify the device I2C address (0x1D default)
uint8_t EBF_Module_1RGB_Led::Init(uint8_t i2cAddress, float maxCurrentRed, float maxCurrentGreen, float maxCurrentBlue)
{
	uint8_t rc;

	rc = EBF_HalInstance::Init(HAL_Type::I2C_INTERFACE, i2cAddress);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// This is output only device, polling is not needed
	SetPollingInterval(EBF_NO_POLLING);

	// Init the chip. Max current might need to be tweaked to have a perfect white color from the RGB colors
	rc = chip.Init(i2cAddress, 0, maxCurrentRed, maxCurrentGreen, maxCurrentBlue);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Set default saved RGV values to white, in case someone will call On() before SetRGB();
	savedRGB[channelRed] = 0xFF;	// R
	savedRGB[channelGreen] = 0xFF;	// G
	savedRGB[channelBlue] = 0xFF;	// B

	EBF_REPORT_AND_RETURN(EBF_OK);
}

// Called to process the instance after pollInterval
// Nothing to do for output only device
uint8_t EBF_Module_1RGB_Led::Process()
{
	EBF_REPORT_AND_RETURN(EBF_OK);
}

float EBF_Module_1RGB_Led::mapf(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t EBF_Module_1RGB_Led::SetRGB(float red, float green, float blue, uint8_t keepOff)
{
	uint8_t rc = EBF_OK;

	// Limit the values to 0%..100%
	red = constrain(red, 0.0, 100.0);
	green = constrain(green, 0.0, 100.0);
	blue = constrain(blue, 0.0, 100.0);

	savedRGB[channelRed] = (uint8_t)mapf(red, 0.0, 100.0, 0.0, 255.0);
	savedRGB[channelGreen] = (uint8_t)mapf(green, 0.0, 100.0, 0.0, 255.0);
	savedRGB[channelBlue] = (uint8_t)mapf(blue, 0.0, 100.0, 0.0, 255.0);

	if (!keepOff) {
		rc = chip.SetPWM(savedRGB[channelRed], savedRGB[channelGreen], savedRGB[channelBlue]);
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
	}

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Module_1RGB_Led::SetRGB(uint32_t rgb, uint8_t keepOff)
{
	uint8_t rc = EBF_OK;

	savedRGB[channelRed] = (rgb >> 16) & 0xFF;
	savedRGB[channelGreen] = (rgb >> 8) & 0xFF;
	savedRGB[channelBlue] = rgb & 0xFF;

	if (!keepOff) {
		rc = chip.SetPWM(savedRGB[channelRed], savedRGB[channelGreen], savedRGB[channelBlue]);
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
	}

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Module_1RGB_Led::GetRGB(float &red, float &green, float &blue)
{
	uint8_t rc;
	uint8_t rByte;
	uint8_t gByte;
	uint8_t bByte;

	rc = chip.GetPWM(rByte, gByte, bByte);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	red = mapf(rByte, 0.0, 255.0, 0.0, 100.0);
	green = mapf(gByte, 0.0, 255.0, 0.0, 100.0);
	blue = mapf(bByte, 0.0, 255.0, 0.0, 100.0);

	EBF_REPORT_AND_RETURN(rc);
}

uint32_t EBF_Module_1RGB_Led::GetRGB()
{
	uint8_t rc;
	uint8_t rByte;
	uint8_t gByte;
	uint8_t bByte;

	rc = chip.GetPWM(rByte, gByte, bByte);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
	}

	return rByte<<16 | gByte<<8 | bByte;
}

uint8_t EBF_Module_1RGB_Led::Standby(uint8_t standbyMode)
{
	uint8_t rc;

	if (standbyMode) {
		rc = chip.EnableChip(0);
	} else {
		rc = chip.EnableChip(1);
	}

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Module_1RGB_Led::On()
{
	uint8_t rc;

	// Set saved RGB values
	rc = chip.SetPWM(savedRGB[channelRed], savedRGB[channelGreen], savedRGB[channelBlue]);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Module_1RGB_Led::Off()
{
	uint8_t rc;

	// Set black colors
	rc = chip.SetPWM(0, 0, 0);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Module_1RGB_Led::SetFadingTime(Time fading)
{
	uint8_t rc;

	// Save the fading for blink and restore of the settings after power up
	savedFading = fading;

	rc = chip.SetFadingTime(fading);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	rc = chip.UpdateCommand();
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Module_1RGB_Led::PrepareBlinkAnimation(Time timeOn, Time timeOff, uint8_t repeatTimes)
{
	uint8_t rc;
	uint8_t channel;

	// limit the repeat times to max value
	if (repeatTimes > 15) {
		repeatTimes = 15;
	}

	// Check if the animation engine is running
	if (chip.IsEngineBusy()) {
		// We need to stop it before changing the configuration
		rc = chip.StopCommand();
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
	}

	// Configure dedicated patterns for each of the RGB channels, since they have different PWM values

	channel = channelRed;
	rc = chip.ConfigurePattern(
		channel,
		Time::NO_DELAY,
		Time::NO_DELAY,
		repeatTimes,
		0,
		savedRGB[channel],
		savedRGB[channel],
		0,
		0,
		savedFading,
		timeOn,
		savedFading,
		timeOff
	);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	channel = channelGreen;
	rc = chip.ConfigurePattern(
		channel,
		Time::NO_DELAY,
		Time::NO_DELAY,
		repeatTimes,
		0,
		savedRGB[channel],
		savedRGB[channel],
		0,
		0,
		savedFading,
		timeOn,
		savedFading,
		timeOff
	);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	channel = channelBlue;
	rc = chip.ConfigurePattern(
		channel,
		Time::NO_DELAY,
		Time::NO_DELAY,
		repeatTimes,
		0,
		savedRGB[channel],
		savedRGB[channel],
		0,
		0,
		savedFading,
		timeOn,
		savedFading,
		timeOff
	);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Assign every color's patters to order 0 place for each engines
	rc = chip.SetEnginePatternsOrder(0, 0);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	rc = chip.SetEnginePatternsOrder(1, 1);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	rc = chip.SetEnginePatternsOrder(2, 2);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Enable order #0 for the 3 engines we use
	rc = chip.EnableEngineOrder(0x01, 0x01, 0x01);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// No engine level repetitions are needed for that animation
	rc = chip.SetEngineRepetitions(0, 0 ,0);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Enable the animation mode on all 3 channels
	rc = chip.EnableAnimationMode(0x07);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Update the configuration
	rc = chip.UpdateCommand();
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Module_1RGB_Led::StartAnimation()
{
	uint8_t rc;

	// Start the animation
	rc = chip.StartCommand();

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Module_1RGB_Led::StopAnimation()
{
	uint8_t rc;

	rc = chip.StopCommand();
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Switch to manual mode by default
	rc = chip.EnableAnimationMode(0);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Update the configuration
	rc = chip.UpdateCommand();
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(rc);
}
