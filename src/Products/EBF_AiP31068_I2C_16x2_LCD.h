#ifndef __EBF_AIP31068_I2C_16X2_LCD_H__
#define __EBF_AIP31068_I2C_16X2_LCD_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_HalInstance.h"
#include "../Core/EBF_I2CDevice.h"
#include <Wire.h>

// This class implements 16x2 LCDs based on the AiP31068 I2C driver
class EBF_AiP31068_I2C_16x2_LCD : protected EBF_HalInstance, protected EBF_I2CDevice, public Print {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_AiP31068_I2C_16x2_LCD");

	protected:
		// commands
		const uint8_t LCD_COMMAND_PREFIX =	0x80;
		const uint8_t LCD_DATA_PREFIX =		0x40;

		const uint8_t LCD_CLEARDISPLAY =	0x01;
		const uint8_t LCD_RETURNHOME =		0x02;
		const uint8_t LCD_ENTRYMODESET =	0x04;
		const uint8_t LCD_DISPLAYCONTROL =	0x08;
		const uint8_t LCD_CURSORSHIFT =		0x10;
		const uint8_t LCD_FUNCTIONSET =		0x20;
		const uint8_t LCD_SETCGRAMADDR =	0x40;
		const uint8_t LCD_SETDDRAMADDR =	0x80;

		// flags for function set
		const uint8_t LCD_FUNC_8BITMODE =	0x10;
		const uint8_t LCD_FUNC_4BITMODE =	0x00;
		const uint8_t LCD_FUNC_2LINE =		0x08;
		const uint8_t LCD_FUNC_1LINE =		0x00;
		const uint8_t LCD_FUNC_5x10DOTS =	0x04;
		const uint8_t LCD_FUNC_5x8DOTS =	0x00;

		// flags for display control
		const uint8_t LCD_DISPLAYON =		0x04;
		const uint8_t LCD_DISPLAYOFF =		0x00;
		const uint8_t LCD_CURSORON =		0x02;
		const uint8_t LCD_CURSOROFF =		0x00;
		const uint8_t LCD_BLINKON =			0x01;
		const uint8_t LCD_BLINKOFF =		0x00;

		// flags for display entry mode
		const uint8_t LCD_ENTRYRIGHT =		0x00;
		const uint8_t LCD_ENTRYLEFT =		0x02;
		const uint8_t LCD_ENTRYSHIFTINC =	0x01;
		const uint8_t LCD_ENTRYSHIFTDEC =	0x00;

		// flags for display/cursor shift
		const uint8_t LCD_DISPLAYMOVE =		0x08;
		const uint8_t LCD_CURSORMOVE =		0x00;
		const uint8_t LCD_MOVERIGHT =		0x04;
		const uint8_t LCD_MOVELEFT =		0x00;

	public:
		EBF_AiP31068_I2C_16x2_LCD(EBF_I2C &i2cInterface) : EBF_I2CDevice(&i2cInterface) { }
		EBF_AiP31068_I2C_16x2_LCD(EBF_I2C *pI2cInterface) : EBF_I2CDevice(pI2cInterface) { }

		uint8_t Init(uint8_t i2cAddress = 0x3E);
		// Setting polling interval in milli-seconds
		void SetPollInterval(uint32_t ms) { pollIntervalMs = ms; }

		// Print class interface
		size_t write(uint8_t b);

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
		uint8_t DirectionLTR();
		uint8_t DirectionRTL();
		uint8_t AutoScrollOn();
		uint8_t AutoScrollOff();

	protected:
		uint8_t Process();

		uint8_t SendCommand(uint8_t command);

		uint8_t displayControl;
		uint8_t displayMode;
};

#endif
