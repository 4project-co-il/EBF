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

	// Assign every color's pattern to order 0 place for each engines
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

uint8_t EBF_Module_1RGB_Led::PreparePoliceAnimation(Time onTime, Time offTime, uint8_t numberOfFlashes, Time restTime, float intensity)
{
	uint8_t rc;
	uint8_t pwm;

	// Check if the animation engine is running
	if (chip.IsEngineBusy()) {
		// We need to stop it before changing the configuration
		rc = chip.StopCommand();
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
	}

	// Limit number of flashes to 14, since 15 means infinite repetition and it will create a problem
	if (numberOfFlashes > 14) {
		EBF_REPORT_AND_RETURN(EBF_INDEX_OUT_OF_BOUNDS);
	}

	// Limit the values to 0%..100%
	intensity = constrain(intensity, 0.0, 100.0);

	// convert specified intensity to pwm value
	pwm = (uint8_t)mapf(intensity, 0.0, 100.0, 0.0, 255.0);

	// We will use pattern 0 for flashing number of times with specified ON/OFF times
	rc = chip.ConfigurePattern(
		0,
		Time::NO_DELAY,
		Time::NO_DELAY,
		numberOfFlashes,
		pwm,
		pwm,
		0,
		0,
		0,
		onTime,
		Time::NO_DELAY,
		offTime,
		Time::NO_DELAY
	);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// We will use pattern 1 for reset time between the colors
	rc = chip.ConfigurePattern(
		1,
		restTime,
		Time::NO_DELAY,
		0,
		0,
		0,
		0,
		0,
		0,
		Time::NO_DELAY,
		Time::NO_DELAY,
		Time::NO_DELAY,
		Time::NO_DELAY
	);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// We will use pattern 2 for the dark period. It's the same as pattern 1, but with PWM=0
	rc = chip.ConfigurePattern(
		2,
		Time::NO_DELAY,
		Time::NO_DELAY,
		numberOfFlashes,
		0,
		0,
		0,
		0,
		0,
		onTime,
		Time::NO_DELAY,
		offTime,
		Time::NO_DELAY
	);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Set RED channel order to pattern0 (blink), pattern1 (restTime), pattern2 (dark), pattern1 (restTime)
	rc = chip.SetEnginePatternsOrder(channelRed, 0, 1, 2, 1);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Set BLUE channel order to pattern2 (dark), pattern1 (restTime), pattern0 (blink), pattern1 (restTime)
	rc = chip.SetEnginePatternsOrder(channelBlue, 2, 1, 0, 1);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Set GREEN channel to have dark patterns indefinetely to have it set to black automatically
	rc = chip.SetEnginePatternsOrder(channelGreen, 2, 1, 2, 1);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Enable orders 0,1,2,3 for all 3 channels
	rc = chip.EnableEngineOrder(0x0F, 0x0F, 0x0F);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Infinite engine level repetitions are needed for that animation
	rc = chip.SetEngineRepetitions(0x03, 0x03 ,0x03);
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

uint8_t EBF_Module_1RGB_Led::PrepareRainbowAnimation(Time transitionTime, float intensity)
{
	uint8_t rc;
	uint8_t pwm;

	// Check if the animation engine is running
	if (chip.IsEngineBusy()) {
		// We need to stop it before changing the configuration
		rc = chip.StopCommand();
		if (rc != EBF_OK) {
			EBF_REPORT_AND_RETURN(rc);
		}
	}

	// Limit the values to 0%..100%
	intensity = constrain(intensity, 0.0, 100.0);

	// convert specified intensity to pwm value
	pwm = (uint8_t)mapf(intensity, 0.0, 100.0, 0.0, 255.0);

	// Fading should be 0 for that animation
	rc = chip.SetFadingTime(Time::NO_DELAY);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Configure the RGB channels with Polyrhythmic Interference
/*
PWM Value
  255 |                     .-.  (PWM3)
      |                    /   \
  180 |       .-. (PWM1)  /     \
      |      /   \       /       \
   60 |     /     `-(PWM2)        \
    0 |-(PWM0)---------------------(PWM4)-
      0s     sloperT0  sloperT1 sloperT2 sloperT3
*/

	rc = chip.ConfigurePattern(
		channelRed,
		Time::NO_DELAY,
		Time::NO_DELAY,
		0x0F,				// Infinite repetition
		0,
		(uint8_t)(0.71 * pwm),
		(uint8_t)(0.25 * pwm),
		pwm,
		0,
		transitionTime,
		(Time)(transitionTime-2),
		(Time)(transitionTime+2),
		transitionTime
	);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	rc = chip.ConfigurePattern(
		channelGreen,
		Time::NO_DELAY,
		Time::NO_DELAY,
		0x0F,				// Infinite repetition
		0,
		(uint8_t)(0.28 * pwm),
		pwm,
		(uint8_t)(0.38 * pwm),
		0,
		(Time)(transitionTime+2),
		transitionTime,
		transitionTime,
		transitionTime
	);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	rc = chip.ConfigurePattern(
		channelBlue,
		Time::NO_DELAY,
		Time::NO_DELAY,
		0x0F,				// Infinite repetition
		0,
		(uint8_t)(0.88 * pwm),
		(uint8_t)(0.19 * pwm),
		(uint8_t)(0.56 * pwm),
		0,
		(Time)(transitionTime+1),
		(Time)(transitionTime+1),
		(Time)(transitionTime+1),
		(Time)(transitionTime-2)
	);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Assign every color's pattern to order 0 place for each engines
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
