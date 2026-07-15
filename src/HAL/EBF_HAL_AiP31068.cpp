#include "EBF_HAL_AiP31068.h"
#include "../Core/EBF_Core.h"

// Part of the code is based on the Grove_LCD_RGB_Backlight
// https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight


EBF_HAL_AiP31068::EBF_HAL_AiP31068(EBF_I2C *i2cInterface) : EBF_I2CDevice(i2cInterface)
{
	i2cAddress = defaultI2CAddress;

	displayControl = 0;
	displayMode = 0;

	// According to datasheet, the LCD needs more than 15mSec after power up. Have to delay
	delay(20);
}

// Inits the LCD to 2-line mode, Display ON, Cursor OFF, Blink OFF, LTR direction
uint8_t EBF_HAL_AiP31068::Init()
{
	uint8_t rc;

	// Initialize the LCD for 2 line mode
	rc = SendCommand(LCD_FUNCTIONSET | LCD_FUNC_2LINE);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

    // Turn the display on with no cursor and no blinking by default
    displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	rc = SendCommand(LCD_DISPLAYCONTROL | displayControl);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// Clear the display
	rc = Clear();
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// LTR direction by default
    displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDEC;
    rc = SendCommand(LCD_ENTRYMODESET | displayMode);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

uint8_t EBF_HAL_AiP31068::SendCommand(uint8_t command)
{
	uint8_t rc;
	pI2C->beginTransmission(i2cAddress);
	pI2C->write(LCD_COMMAND_PREFIX);
	pI2C->write(command);
	rc = pI2C->endTransmission();

	if (rc != 0) {
		EBF_REPORT_ERROR(EBF_COMMUNICATION_PROBLEM);
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

uint8_t EBF_HAL_AiP31068::WriteChar(uint8_t b)
{
	uint8_t rc;

	pI2C->beginTransmission(i2cAddress);
	pI2C->write(LCD_DATA_PREFIX);
	pI2C->write(b);
	rc = pI2C->endTransmission();

	if (rc == 0) {
		return 1;
	} else {
		return 0;
	}
}

// Clears the display and moves to the first row/col position
uint8_t EBF_HAL_AiP31068::Clear()
{
	uint8_t rc;

	rc = SendCommand(LCD_CLEARDISPLAY);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// TODO: delay() is bad, but looks like there is no other way to do it
	delayMicroseconds(1500);	// According to datasheet, the HD44780 needs 1.52mSec after clear command

	return EBF_OK;
}

// Moves to the first row/col position without clearing the display
uint8_t EBF_HAL_AiP31068::Home()
{
	uint8_t rc;

	rc = SendCommand(LCD_RETURNHOME);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// TODO: delay() is bad, but looks like there is no other way to do it
	delayMicroseconds(1500);	// According to datasheet, the HD44780 needs 1.52mSec after home command

	return EBF_OK;
}

// Moves the cursor to specified row and column
uint8_t EBF_HAL_AiP31068::SetCursor(uint8_t col, uint8_t row)
{
	if (row == 0) {
		col |= 0x80;
	} else {
		col |= 0xC0;
	}

	return SendCommand(col);
}

// Turns ON the displayed text
uint8_t EBF_HAL_AiP31068::DisplayOn()
{
	displayControl |= LCD_DISPLAYON;
	return SendCommand(LCD_DISPLAYCONTROL | displayControl);
}

// Turns OFF the displayed text
uint8_t EBF_HAL_AiP31068::DisplayOff()
{
	displayControl &= ~LCD_DISPLAYON;
	return SendCommand(LCD_DISPLAYCONTROL | displayControl);
}

// Turns ON the underline cursor
uint8_t EBF_HAL_AiP31068::CursorOn()
{
	displayControl |= LCD_CURSORON;
	return SendCommand(LCD_DISPLAYCONTROL | displayControl);
}

// Turns OFF the underline cursor
uint8_t EBF_HAL_AiP31068::CursorOff()
{
	displayControl &= ~LCD_CURSORON;
	return SendCommand(LCD_DISPLAYCONTROL | displayControl);
}

// Enable the blinking cursor
uint8_t EBF_HAL_AiP31068::BlinkOn()
{
	displayControl |= LCD_BLINKON;
	return SendCommand(LCD_DISPLAYCONTROL | displayControl);
}

// Disable the blinking cursor
uint8_t EBF_HAL_AiP31068::BlinkOff()
{
	displayControl &= ~LCD_BLINKON;
	return SendCommand(LCD_DISPLAYCONTROL | displayControl);
}

// Scroll the display left by 1 character
uint8_t EBF_HAL_AiP31068::ScrollLeft()
{
	return SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

// Scroll the display left by specified number of characters
uint8_t EBF_HAL_AiP31068::ScrollLeft(uint8_t count)
{
	uint8_t rc;

	for (uint8_t i=0; i<count; i++) {
		rc = ScrollLeft();
		if (rc != EBF_OK) {
			EBF_REPORT_ERROR(rc);
			return rc;
		}

		// Have to wait between the commands
		delayMicroseconds(50);
	}

	return EBF_OK;
}

// Scroll the display right by 1 character
uint8_t EBF_HAL_AiP31068::ScrollRight()
{
	return SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// Scroll the display right by specified number of characters
uint8_t EBF_HAL_AiP31068::ScrollRight(uint8_t count)
{
	uint8_t rc;

	for (uint8_t i=0; i<count; i++) {
		rc = ScrollRight();
		if (rc != EBF_OK) {
			EBF_REPORT_ERROR(rc);
			return rc;
		}

		// Have to wait between the commands
		delayMicroseconds(50);
	}

	return EBF_OK;
}

// Changes the direction of text flow to LTR - Left-To-Right
uint8_t EBF_HAL_AiP31068::DirectionLTR()
{
	displayMode |= LCD_ENTRYLEFT;
	return SendCommand(LCD_ENTRYMODESET | displayMode);
}

// Changes the direction of text flow to RTL - Right-To-Left
uint8_t EBF_HAL_AiP31068::DirectionRTL()
{
	displayMode &= ~LCD_ENTRYLEFT;
	return SendCommand(LCD_ENTRYMODESET | displayMode);
}

// Enable the autoscroll
uint8_t EBF_HAL_AiP31068::AutoScrollOn()
{
	displayMode |= LCD_ENTRYSHIFTINC;
	return SendCommand(LCD_ENTRYMODESET | displayMode);
}

// Disable the autoscroll
uint8_t EBF_HAL_AiP31068::AutoScrollOff()
{
	displayMode &= ~LCD_ENTRYSHIFTINC;
	return SendCommand(LCD_ENTRYMODESET | displayMode);
}
