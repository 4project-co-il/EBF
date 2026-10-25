#include "EBF_HAL_LP5815.h"
#include "../Core/EBF_Core.h"

EBF_HAL_LP5815::EBF_HAL_LP5815(EBF_I2C *i2cInterface) : EBF_I2CDevice(i2cInterface)
{
	EBF_I2CDevice::i2cAddress = defaultI2CAddress;
}

uint8_t EBF_HAL_LP5815::Init(uint8_t i2cAddress,
	uint8_t highCurrentEnable,
	float ch0CurrentLimit,
	float ch1CurrentLimit,
	float ch2CurrentLimit)
{
	uint8_t rc;
	float currentCap;

	EBF_I2CDevice::i2cAddress = i2cAddress;

	// Reset the chip to default settings
	rc = ResetCommand();
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Enable the chip and disable the insta-blink on initialization
	rc = EnableChip(1);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	if (highCurrentEnable) {
		currentCap = 51; //mA
		rc = Write8bitRegister(regDevConfig0, 0x01);

	} else {
		currentCap = 25.5; // mA
		rc = Write8bitRegister(regDevConfig0, 0x00);
	}

	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// check that requested limits are in maximal limits
	if (ch0CurrentLimit > currentCap) {
		ch0CurrentLimit = currentCap;
	}
	if (ch1CurrentLimit > currentCap) {
		ch1CurrentLimit = currentCap;
	}
	if(ch2CurrentLimit > currentCap) {
		ch2CurrentLimit = currentCap;
	}

	// Set channel0 to ratio of requested limit bound into 0..255 range
	rc = Write8bitRegister(regOut0_DC, (uint8_t)(ch0CurrentLimit/currentCap * 255.0));
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Set channel1 to ratio of requested limit bound into 0..255 range
	rc = Write8bitRegister(regOut1_DC, (uint8_t)(ch1CurrentLimit/currentCap * 255.0));
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Set channel2 to ratio of requested limit bound into 0..255 range
	rc = Write8bitRegister(regOut2_DC, (uint8_t)(ch2CurrentLimit/currentCap * 255.0));
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Use exponential dimming on all 3 channels, to have more realistic effects
	// Autonomous animation disabled by default
	rc = EnableAnimationMode(0, 0x07);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Assign engine for each channel for the animation execution
	rc = SetChannelEngine(0, 1, 2);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Enable all 3 channels output
	rc = SetChannelOutput(0x07);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	rc = UpdateCommand();
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::SetFadingTime(LP5815_Time fading)
{
	uint8_t rc;
	uint8_t val = fading;

	if (fading != NO_DELAY) {
		// Fading time should be shifted to bits 4-7, 0x07 turns on all 3 channels
		val = (fading << 4) | 0x07;
	}

	rc = Write8bitRegister(regDevConfig2, val);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::ShutdownCommand()
{
	uint8_t rc;

	// 0x33 = Enter shutdown mode
	rc = Write8bitRegister(regShutdownCmd, 0x33);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::ResetCommand()
{
	uint8_t rc;

	// 0xCC = Reset all the registers to default value
	rc = Write8bitRegister(regResetCmd, 0xCC);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::UpdateCommand()
{
	uint8_t rc;

	// 0x55 = Update all device configuration registers value
	rc = Write8bitRegister(regUpdateCmd, 0x55);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::StartCommand()
{
	uint8_t rc;

	// 0xFF = Start autonomous animation
	rc = Write8bitRegister(regStartCmd, 0xFF);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::StopCommand()
{
	uint8_t rc;

	// 0xAA = Stop autonomous animation
	rc = Write8bitRegister(regStopCmd, 0xAA);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::PauseAnimation()
{
	uint8_t rc;

	// Pause or continue autonomous animation. 0x1 = Pause
	rc = Write8bitRegister(regPauseContinue, 0x01);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::ContinueAnimation()
{
	uint8_t rc;

	// Pause or continue autonomous animation. 0x0 = Continue
	rc = Write8bitRegister(regPauseContinue, 0x00);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::SetChannelPWM(uint8_t channel, uint8_t pwm)
{
	uint8_t rc;

	// The chip has only 3 channels 0..2
	if (channel > 2) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	// Set the specific channel PWM value
	rc = Write8bitRegister(regOut0_Manual_PWM + channel, pwm);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::GetChannelPWM(uint8_t channel, uint8_t &pwm)
{
	uint8_t rc;

	// The chip has only 3 channels 0..2
	if (channel > 2) {
		return EBF_INDEX_OUT_OF_BOUNDS;
	}

	// Get the specific channel PWM value
	rc = Read8bitRegister(regOut0_Manual_PWM + channel, pwm);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::SetPWM(uint8_t ch0PWM, uint8_t ch1PWM, uint8_t ch2PWM)
{
	uint8_t rc;

	// 3 channels
	uint8_t buffer[3];

	// prepare the buffer
	buffer[0] = ch0PWM;
	buffer[1] = ch1PWM;
	buffer[2] = ch2PWM;

	// Set 3 channels at once
	rc = WriteBuffer(regOut0_Manual_PWM, buffer, sizeof(buffer));

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::GetPWM(uint8_t &ch0PWM, uint8_t &ch1PWM, uint8_t &ch2PWM)
{
	uint8_t rc;

	// 3 channels
	uint8_t buffer[3] = {0};

	// Set 3 channels at once
	rc = ReadBuffer(regOut0_Manual_PWM, buffer, sizeof(buffer));
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	ch0PWM = buffer[0];
	ch1PWM = buffer[1];
	ch2PWM = buffer[2];

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::EnableChip(uint8_t enable)
{
	uint8_t rc;

	if (enable) {
		enable = 0x03;	// Chip enable (bit0) and insta-blink disable (bit1)
	} else {
		enable = 0x02;	// Chip disable (bit0) and insta-blink disable (bit1)
	}

	// Enable the chip and disable the insta-blink on initialization
	rc = Write8bitRegister(regChipEn, enable);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::SetChannelOutput(uint8_t channels)
{
	uint8_t rc;

	// Limit the input to 3 channels
	channels = channels & 0x07;

	// Enable the specified channels
	rc = Write8bitRegister(regDevConfig1, channels);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::GetChannelOutput(uint8_t &channels)
{
	uint8_t rc;

	// Enable the chip and disable the insta-blink on initialization
	rc = Read8bitRegister(regDevConfig1, channels);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::ConfigurePattern(
	uint8_t num,
	LP5815_Time pauseT0,
	LP5815_Time pauseT1,
	uint8_t repeatTimes,
	uint8_t pwm0,
	uint8_t pwm1,
	uint8_t pwm2,
	uint8_t pwm3,
	uint8_t pwm4,
	LP5815_Time sloperT0,
	LP5815_Time sloperT1,
	LP5815_Time sloperT2,
	LP5815_Time sloperT3
)
{
	uint8_t rc;
	uint8_t buffer[9];
	uint8_t index = 0;

	// 4 patterns in LP5815, 0..3
	if (num > 3) {
		EBF_REPORT_AND_RETURN(EBF_INDEX_OUT_OF_BOUNDS);
	}

	buffer[index++] = pauseT0 << 4 | pauseT1;
	buffer[index++] = repeatTimes;
	buffer[index++] = pwm0;
	buffer[index++] = pwm1;
	buffer[index++] = pwm2;
	buffer[index++] = pwm3;
	buffer[index++] = pwm4;
	buffer[index++] = sloperT1 << 4 | sloperT0;
	buffer[index++] = sloperT3 << 4 | sloperT2;

	rc = WriteBuffer(regPattern0_Pause_Time + num*9, buffer, sizeof(buffer));
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::SetChannelEngine(uint8_t ch0EngineNumber, uint8_t ch1EngineNumber, uint8_t ch2EngineNumber)
{
	uint8_t rc;

	if (ch0EngineNumber > 3) {
		EBF_REPORT_AND_RETURN(EBF_INDEX_OUT_OF_BOUNDS);
	}
	if (ch1EngineNumber > 3) {
		EBF_REPORT_AND_RETURN(EBF_INDEX_OUT_OF_BOUNDS);
	}
	if (ch2EngineNumber > 3) {
		EBF_REPORT_AND_RETURN(EBF_INDEX_OUT_OF_BOUNDS);
	}

	rc = Write8bitRegister(regDevConfig4, ch2EngineNumber << 4 | ch1EngineNumber << 2 | ch0EngineNumber);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::EnableAnimationMode(uint8_t enable, uint8_t exponential)
{
	uint8_t rc;

	// Limit to 3 channels
	enable = enable & 0x07;
	exponential = exponential & 0x07;

	rc = Write8bitRegister(regDevConfig3, exponential << 4 | enable);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::GetFlags(uint8_t &flags)
{
	uint8_t rc;

	rc = Read8bitRegister(regFlag, flags);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::IsEngineBusy()
{
	uint8_t rc;
	uint8_t flags;

	rc = GetFlags(flags);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	// Engine busy flag is bit3
	if (flags & 1<<2) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t EBF_HAL_LP5815::SetEnginePatternsOrder(uint8_t engineNumber, uint8_t pattern0, uint8_t pattern1, uint8_t pattern2, uint8_t pattern3)
{
	uint8_t rc;

	if (engineNumber > 3) {
		EBF_REPORT_AND_RETURN(EBF_INDEX_OUT_OF_BOUNDS);
	}
	if (pattern0 > 3) {
		EBF_REPORT_AND_RETURN(EBF_INDEX_OUT_OF_BOUNDS);
	}
	if (pattern1 > 3) {
		EBF_REPORT_AND_RETURN(EBF_INDEX_OUT_OF_BOUNDS);
	}
	if (pattern2 > 3) {
		EBF_REPORT_AND_RETURN(EBF_INDEX_OUT_OF_BOUNDS);
	}
	if (pattern3 > 3) {
		EBF_REPORT_AND_RETURN(EBF_INDEX_OUT_OF_BOUNDS);
	}

	rc = Write8bitRegister(regEngineConfig0 + engineNumber, pattern3 << 6 | pattern2 << 4 | pattern1 << 2 | pattern0);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::EnableEngineOrder(uint8_t engine0Orders, uint8_t engine1Orders, uint8_t engine2Orders, uint8_t engine3Orders)
{
	uint8_t rc;

	// Limit to 4 bits for each engine
	engine0Orders = engine0Orders & 0x0F;
	engine1Orders = engine1Orders & 0x0F;
	engine2Orders = engine2Orders & 0x0F;
	engine3Orders = engine3Orders & 0x0F;

	rc = Write8bitRegister(regEngineConfig4, engine1Orders << 4 | engine0Orders);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	rc = Write8bitRegister(regEngineConfig5, engine3Orders << 4 | engine2Orders);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_HAL_LP5815::SetEngineRepetitions(uint8_t engine0Repetitions, uint8_t engine1Repetitions, uint8_t engine2Repetitions, uint8_t engine3Repetitions)
{
	uint8_t rc;

	// Limit to 3 bits for each engine
	engine0Repetitions = engine0Repetitions & 0x07;
	engine1Repetitions = engine1Repetitions & 0x07;
	engine2Repetitions = engine2Repetitions & 0x07;
	engine3Repetitions = engine3Repetitions & 0x07;

	rc = Write8bitRegister(regEngineConfig6, engine3Repetitions << 6 | engine2Repetitions << 4 | engine2Repetitions << 2 | engine0Repetitions);

	EBF_REPORT_AND_RETURN(rc);
}
