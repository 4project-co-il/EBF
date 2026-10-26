#ifndef __EBF_MODULE_1RGB_LED_H__
#define __EBF_MODULE_1RGB_LED_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_HalInstance.h"
#include "../HAL/EBF_HAL_LP5815.h"

class EBF_Module_1RGB_Led : protected EBF_HalInstance {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_Module_1RGB_Led");

		const uint8_t channelRed = 0;
		const uint8_t channelGreen = 1;
		const uint8_t channelBlue = 2;

	public:
		EBF_Module_1RGB_Led(EBF_I2C *pI2cInterface) : chip(pI2cInterface) {}
		EBF_Module_1RGB_Led(EBF_I2C &i2cInterface) : EBF_Module_1RGB_Led(&i2cInterface) { }

		uint8_t Init(
			uint8_t i2cAddress = EBF_HAL_LP5815::defaultI2CAddress,
			float maxCurrentRed = 13.5,			// current limit in mA calculated from LITEON LTST-C19HE1WT RGB LED mcd values to get a white color
			float maxCurrentGreen = 10,
			float maxCurrentBlue = 4.8);

		using Time = EBF_HAL_LP5815::LP5815_Time;

		// Commands

		// Enter and exit standby mode
		uint8_t Standby(uint8_t standbyMode);

		// Sets transition fading time
		uint8_t SetFadingTime(Time fading);

		// Sets the intensity values for the RGB colors, in percentage from 0% to 100%
		uint8_t SetRGB(float red, float green, float blue, uint8_t keepOff = 0);

		// Sets the intensity values for the RGB colors, in HEX format of 0x00RRGGBB
		uint8_t SetRGB(uint32_t rgb, uint8_t keepOff = 0);

		// Gets current intensity values for the RGB colors, in percentage from 0% to 100%
		uint8_t GetRGB(float &red, float &green, float &blue);

		// Gets current intensity values for the RGB colors, in HEX format of 0x00RRGGBB
		uint32_t GetRGB();

		// Turns ON the LED based on current RGB settings
		uint8_t On();

		// Turns OFF the LED
		uint8_t Off();

		// Prepares Blink/Breath transition for specified ON/OFF/Repetition settings.
		// Infinite repeat times = 0x0F (15)
		uint8_t PrepareBlinkAnimation(Time timeOn, Time timeOff, uint8_t repeatTimes = 15);

		// Prepares police lights animation using red and blue colors
		// TitalColorTime should be (on+off)*numberOfFlashes + 100mSec rest
		uint8_t PreparePoliceAnimation(
			Time onTime = Time::TIME_50MS,
			Time offTime = Time::TIME_50MS,
			uint8_t numberOfFlashes = 5,
			Time restTime = Time::TIME_100MS,
			float intensity = 100.0
		);

		// Prepares animation of rainbow colors transition
		uint8_t PrepareRainbowAnimation(Time transitionTime = Time::TIME_500MS, float intensity = 100.0);

		// Starts the prepared animation
		uint8_t StartAnimation();

		// Stops the animation and goes back to manual RGB settings mode
		uint8_t StopAnimation();

		// Returns 1 when the animation is running, 0 otherwise.
		uint8_t IsAnimationRunning() { return chip. IsEngineBusy(); }

	protected:
		// LCD Driver AiP31068 chip
		EBF_HAL_LP5815 chip;

		// Saved color settings in order to implement On/Off functionality with
		uint8_t savedRGB[3];
		// Saved fading for blink and restore of the settings after power up
		Time savedFading;

		uint8_t Process();

		float mapf(float x, float in_min, float in_max, float out_min, float out_max);
};

#endif
