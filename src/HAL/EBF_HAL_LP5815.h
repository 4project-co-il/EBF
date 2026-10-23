#ifndef __EBF_HAL_LP5815_H__
#define __EBF_HAL_LP5815_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Core.h"
#include "../Core/EBF_I2CDevice.h"

// This class implements access to LP5815 chip, I2C 3-channel led driver
class EBF_HAL_LP5815 : public EBF_I2CDevice {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_HAL_LP5815");

	public:
		static const uint8_t defaultI2CAddress = 0x2D;

		EBF_HAL_LP5815(EBF_I2C *i2cInterface);
		EBF_HAL_LP5815(EBF_I2C &i2cInterface) : EBF_HAL_LP5815(&i2cInterface) { }

		uint8_t Init(
			uint8_t i2cAddress = defaultI2CAddress,
			uint8_t highCurrentEnable = 0,		// 1=51mA, 0=25.5mA cap
			float ch0CurrentLimit = 10.0,		// In mA
			float ch1CurrentLimit = 10.0,		// In mA
			float ch2CurrentLimit = 10.0);		// In mA

	private:
		// Registers
		const uint8_t regChipEn					= 0x00;
		const uint8_t regDevConfig0				= 0x01;
		const uint8_t regDevConfig1				= 0x02;
		const uint8_t regDevConfig2				= 0x03;
		const uint8_t regDevConfig3				= 0x04;
		const uint8_t regDevConfig4				= 0x05;
		const uint8_t regEngineConfig0			= 0x06;
		const uint8_t regEngineConfig1			= 0x07;
		const uint8_t regEngineConfig2			= 0x08;
		const uint8_t regEngineConfig3			= 0x09;
		const uint8_t regEngineConfig4			= 0x0A;
		const uint8_t regEngineConfig5			= 0x0B;
		const uint8_t regEngineConfig6			= 0x0C;
		const uint8_t regShutdownCmd			= 0x0D;
		const uint8_t regResetCmd				= 0x0E;
		const uint8_t regUpdateCmd				= 0x0F;
		const uint8_t regStartCmd				= 0x10;
		const uint8_t regStopCmd				= 0x11;
		const uint8_t regPauseContinue			= 0x12;
		const uint8_t regFlagClr				= 0x13;
		const uint8_t regOut0_DC				= 0x14;
		const uint8_t regOut1_DC				= 0x15;
		const uint8_t regOut2_DC				= 0x16;
		const uint8_t regOut0_Manual_PWM		= 0x18;
		const uint8_t regOut1_Manual_PWM		= 0x19;
		const uint8_t regOut2_Manual_PWM		= 0x1A;

		const uint8_t regPattern0_Pause_Time	= 0x1C;
		const uint8_t regPattern0_Repeat_Time	= 0x1D;
		const uint8_t regPattern0_PWM0			= 0x1E;
		const uint8_t regPattern0_PWM1			= 0x1F;
		const uint8_t regPattern0_PWM2			= 0x20;
		const uint8_t regPattern0_PWM3			= 0x21;
		const uint8_t regPattern0_PWM4			= 0x22;
		const uint8_t regPattern0_Sloper_Time1	= 0x23;
		const uint8_t regPattern0_Sloper_Time2	= 0x24;

		const uint8_t regPattern1_Pause_Time	= 0x25;
		const uint8_t regPattern1_Repeat_Time	= 0x26;
		const uint8_t regPattern1_PWM0			= 0x27;
		const uint8_t regPattern1_PWM1			= 0x28;
		const uint8_t regPattern1_PWM2			= 0x29;
		const uint8_t regPattern1_PWM3			= 0x2A;
		const uint8_t regPattern1_PWM4			= 0x2B;
		const uint8_t regPattern1_Sloper_Time1	= 0x2C;
		const uint8_t regPattern1_Sloper_Time2	= 0x2D;

		const uint8_t regPattern2_Pause_Time	= 0x2E;
		const uint8_t regPattern2_Repeat_Time	= 0x2F;
		const uint8_t regPattern2_PWM0			= 0x30;
		const uint8_t regPattern2_PWM1			= 0x31;
		const uint8_t regPattern2_PWM2			= 0x32;
		const uint8_t regPattern2_PWM3			= 0x33;
		const uint8_t regPattern2_PWM4			= 0x34;
		const uint8_t regPattern2_Sloper_Time1	= 0x35;
		const uint8_t regPattern2_Sloper_Time2	= 0x36;

		const uint8_t regPattern3_Pause_Time	= 0x37;
		const uint8_t regPattern3_Repeat_Time	= 0x38;
		const uint8_t regPattern3_PWM0			= 0x39;
		const uint8_t regPattern3_PWM1			= 0x3A;
		const uint8_t regPattern3_PWM2			= 0x3B;
		const uint8_t regPattern3_PWM3			= 0x3C;
		const uint8_t regPattern3_PWM4			= 0x3D;
		const uint8_t regPattern3_Sloper_Time1	= 0x3E;
		const uint8_t regPattern3_Sloper_Time2	= 0x3F;

		const uint8_t regFlag					= 0x40;

	public:
		// APIs
		typedef enum : uint8_t {
			NO_DELAY	= 0x00,
			TIME_50MS	= 0x01,
			TIME_100MS	= 0x02,
			TIME_150MS	= 0x03,
			TIME_200MS	= 0x04,
			TIME_250MS	= 0x05,
			TIME_300MS	= 0x06,
			TIME_350MS	= 0x07,
			TIME_400MS	= 0x08,
			TIME_450MS	= 0x09,
			TIME_500MS	= 0x0A,
			TIME_1000MS	= 0x0B,
			TIME_2000MS	= 0x0C,
			TIME_4000MS	= 0x0D,
			TIME_6000MS	= 0x0E,
			TIME_8000MS	= 0x0F,
		} LP5815_Time;

		uint8_t EnableChip(uint8_t enable);
		uint8_t ShutdownCommand();
		uint8_t ResetCommand();
		uint8_t UpdateCommand();
		uint8_t StartCommand();
		uint8_t StopCommand();
		uint8_t PauseAnimation();
		uint8_t ContinueAnimation();

		uint8_t SetFadingTime(LP5815_Time fading);

		uint8_t SetChannelPWM(uint8_t channel, uint8_t pwm);
		uint8_t GetChannelPWM(uint8_t channel, uint8_t &pwm);
		uint8_t SetPWM(uint8_t ch0PWM, uint8_t ch1PWM, uint8_t ch2PWM);
		uint8_t GetPWM(uint8_t &ch0PWM, uint8_t &ch1PWM, uint8_t &ch2PWM);

		// Enable or disable specific output channels. Bit per channel. 0x07 to enable all 3 channels at once.
		uint8_t SetChannelOutput(uint8_t channels);
		// Returns enable flags for the channels. Bit per channel. 0x07 when all 3 channels are enabled.
		uint8_t GetChannelOutput(uint8_t &channels);

		// Configure animation pattern
		uint8_t ConfigurePattern(
			uint8_t num,
			LP5815_Time pauseT0,
			LP5815_Time pauseT1,
			uint8_t repeatTimes,		// 7 = Infinite repeats
			uint8_t pwm0,
			uint8_t pwm1,
			uint8_t pwm2,
			uint8_t pwm3,
			uint8_t pwm4,
			LP5815_Time sloperT0,
			LP5815_Time sloperT1,
			LP5815_Time sloperT2,
			LP5815_Time sloperT3
		);

		uint8_t SetChannelEngine(uint8_t ch0EngineNumber, uint8_t ch1EngineNumber, uint8_t ch2EngineNumber);

		// Enables animation mode, or disables to work in manual PWM mode. Bit per channel, 0x07 for all 3 channels at once
		// Exponential dimming enables with bit per channel, 0x07 for all 3 channels at once
		uint8_t EnableAnimationMode(uint8_t enable, uint8_t exponential = 0x07);

		uint8_t GetFlags(uint8_t &flags);
		uint8_t IsEngineBusy();
};

#endif
