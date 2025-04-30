#ifndef __EBF_SPARKFUN_QWIIC_SERLCD_H__
#define __EBF_SPARKFUN_QWIIC_SERLCD_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_HalInstance.h"
#include "../Core/EBF_I2CDevice.h"
#include <Wire.h>

class EBF_SparkFun_QWIIC_SerLCD : protected EBF_HalInstance, protected EBF_I2CDevice, public Print {
	protected:
		const uint8_t MAX_ROWS =	4;
		const uint8_t MAX_COLUMNS =	20;

		// Special characters and commands prefix
		const uint8_t SPECIAL_COMMAND =	254; 	// Special command to the LCD
		const uint8_t SETTING_COMMAND =	0x7C; 	// 124 decimal, '|', the pipe character: The command to change settings: baud, lines, width, backlight, splash, etc

		// Setting commands
		const uint8_t LCD_SET_CLEAR =		0x2D;	// 45, '-', the dash character: command to clear and home the display
		const uint8_t CONTRAST_COMMAND =	0x18;	// Command to change the contrast setting
		const uint8_t ADDRESS_COMMAND =		0x19;	// Command to change the i2c address
		const uint8_t SET_RGB_COMMAND =		0x2B;	// 43, '+', the plus character: command to set backlight RGB value
		const uint8_t ENABLE_SYSTEM_MESSAGE_DISPLAY =	0x2E;	// 46, '.', command to enable system messages being displayed
		const uint8_t DISABLE_SYSTEM_MESSAGE_DISPLAY = 	0x2F;	// 47, '/', command to disable system messages being displayed
		const uint8_t ENABLE_SPLASH_DISPLAY =			0x30;	// 48, '0', command to enable splash screen at power on
		const uint8_t DISABLE_SPLASH_DISPLAY =			0x31;	// 49, '1', command to disable splash screen at power on
		const uint8_t SAVE_CURRENT_DISPLAY_AS_SPLASH =	0x0A;	// 10, Ctrl+j, command to save current text on display as splash

		// Special commands
		const uint8_t LCD_CMD_RETURNHOME =		0x02;
		const uint8_t LCD_CMD_ENTRYMODESET =	0x04;
		const uint8_t LCD_CMD_DISPLAYCONTROL =	0x08;
		const uint8_t LCD_CMD_CURSORSHIFT =		0x10;
		const uint8_t LCD_CMD_SETDDRAMADDR =	0x80;

		// Display control flags
		const uint8_t LCD_DISPLAYON =	0x04;
		const uint8_t LCD_DISPLAYOFF = 	0x00;
		const uint8_t LCD_CURSORON =	0x02;
		const uint8_t LCD_CURSOROFF = 	0x00;
		const uint8_t LCD_BLINKON =		0x01;
		const uint8_t LCD_BLINKOFF = 	0x00;

		// flags for display entry mode
		const uint8_t LCD_ENTRYRIGHT = 			0x00;
		const uint8_t LCD_ENTRYLEFT = 			0x02;
		const uint8_t LCD_ENTRYSHIFTINCREMENT =	0x01;
		const uint8_t LCD_ENTRYSHIFTDECREMENT =	0x00;

		// flags for display/cursor shift
		const uint8_t LCD_DISPLAYMOVE =	0x08;
		const uint8_t LCD_CURSORMOVE =	0x00;
		const uint8_t LCD_MOVERIGHT =	0x04;
		const uint8_t LCD_MOVELEFT =	0x00;

	public:
		EBF_SparkFun_QWIIC_SerLCD(EBF_I2C &i2cInterface) : EBF_I2CDevice(&i2cInterface) { }
		EBF_SparkFun_QWIIC_SerLCD(EBF_I2C *pI2cInterface) : EBF_I2CDevice(pI2cInterface) { }

		uint8_t Init(uint8_t i2cAddress = 0x72);
		// Setting polling interval in milli-seconds
		void SetPollInterval(uint32_t ms) { pollIntervalMs = ms; }

		// Print class interface
		size_t write(uint8_t b);
		size_t write(const uint8_t *buffer, size_t size);
		size_t write(const char *str);

		// LCD commands
		uint8_t Clear();
		uint8_t Home();
		uint8_t SetCursor(uint8_t col, uint8_t row);
		uint8_t DisplayOn();
		uint8_t DisplayOff();
		uint8_t CursorOn();
		uint8_t CursorOff();
		uint8_t BlinkOn();
		uint8_t BlinkOff();
		uint8_t ScrollLeft();
		uint8_t ScrollLeft(uint8_t count);
		uint8_t ScrollRight();
		uint8_t ScrollRight(uint8_t count);
		uint8_t CursorLeft();
		uint8_t CursorLeft(uint8_t count);
		uint8_t CursorRight();
		uint8_t CursorRight(uint8_t count);
		uint8_t SetBacklight(uint8_t r, uint8_t g, uint8_t b);
		uint8_t SetBacklight(uint32_t rgb);
		uint8_t SystemMessageOn();
		uint8_t SystemMessageOff();
		uint8_t SplashScreenOn();
		uint8_t SplashScreenOff();
		uint8_t SaveSplashScreen();
		uint8_t DirectionLTR();
		uint8_t DirectionRTL();
		uint8_t AutoScrollOn();
		uint8_t AutoScrollOff();
		uint8_t setContrast(uint8_t contrast);

	protected:
		uint8_t Process();

		uint8_t SendSettingCommand(uint8_t command);
		uint8_t SendSpecialCommand(uint8_t command);
		uint8_t SendSpecialCommand(uint8_t command, uint8_t count);

		uint8_t displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
		uint8_t displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
};

#endif
