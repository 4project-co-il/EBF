#include "EBF_SparkFun_QWIIC_SerLCD.h"
#include "../Core/EBF_Core.h"

extern void EBF_EmptyCallback();

// Part of the code is based on the SparkFun_SerLCD_Arduino_Library
// https://github.com/sparkfun/SparkFun_SerLCD_Arduino_Library

// Initializing EBF_SparkFun_QWIIC_SerLCD class instance.
// The i2cAddress should specify the device I2C address (0x72 default)
uint8_t EBF_SparkFun_QWIIC_SerLCD::Init(uint8_t i2cAddress)
{
	uint8_t rc;

	this->i2cAddress = i2cAddress;

	rc = EBF_HalInstance::Init(HAL_Type::I2C_INTERFACE, i2cAddress);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// This is output only device, polling is not needed
	SetPollingInterval(EBF_NO_POLLING);

	// Initialize the LCD
	pI2C->beginTransmission(i2cAddress);
	pI2C->write(SPECIAL_COMMAND);                      //Send special command character
	pI2C->write(LCD_CMD_DISPLAYCONTROL | displayControl); //Send the display command
	pI2C->write(SPECIAL_COMMAND);                      //Send special command character
	pI2C->write(LCD_CMD_ENTRYMODESET | displayMode);      //Send the entry mode command
	pI2C->write(SETTING_COMMAND);                      //Put LCD into setting mode
	pI2C->write(LCD_SET_CLEAR);                        //Send clear display command
	rc = pI2C->endTransmission();

	if (rc != 0) {
		EBF_REPORT_ERROR(EBF_COMMUNICATION_PROBLEM);
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

// Called to process the instance after pollInterval
// Nothing to do for output only device
uint8_t EBF_SparkFun_QWIIC_SerLCD::Process()
{
	return EBF_OK;
}

size_t EBF_SparkFun_QWIIC_SerLCD::write(uint8_t b)
{
	uint8_t rc;

	pI2C->beginTransmission(i2cAddress);
	pI2C->write(b);
	rc = pI2C->endTransmission();

	if (rc == 0) {
		return 1;
	} else {
		return 0;
	}
}

size_t EBF_SparkFun_QWIIC_SerLCD::write(const uint8_t *buffer, size_t size)
{
	uint8_t rc;

	pI2C->beginTransmission(i2cAddress);
	for (size_t i=0; i<size; i++) {
		pI2C->write(buffer[i]);
	}
	rc = pI2C->endTransmission();

	if (rc == 0) {
		return size;
	} else {
		return 0;
	}
}

size_t EBF_SparkFun_QWIIC_SerLCD::write(const char *str)
{
	if (str == NULL) {
		return 0;
	}

	return write((const uint8_t *)str, strlen(str));
}

uint8_t EBF_SparkFun_QWIIC_SerLCD::SendSettingCommand(uint8_t command)
{
	uint8_t rc;
	pI2C->beginTransmission(i2cAddress);
	pI2C->write(SETTING_COMMAND);
	pI2C->write(command);
	rc = pI2C->endTransmission();

	if (rc != 0) {
		EBF_REPORT_ERROR(EBF_COMMUNICATION_PROBLEM);
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

uint8_t EBF_SparkFun_QWIIC_SerLCD::SendSpecialCommand(uint8_t command)
{
	uint8_t rc;
	pI2C->beginTransmission(i2cAddress);
	pI2C->write(SPECIAL_COMMAND);
	pI2C->write(command);
	rc = pI2C->endTransmission();

	if (rc != 0) {
		EBF_REPORT_ERROR(EBF_COMMUNICATION_PROBLEM);
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

uint8_t EBF_SparkFun_QWIIC_SerLCD::SendSpecialCommand(uint8_t command, uint8_t count)
{
	uint8_t rc;
	pI2C->beginTransmission(i2cAddress);

	for (uint8_t i=0; i<count; i++) {
		pI2C->write(SPECIAL_COMMAND);
		pI2C->write(command);
	}

	rc = pI2C->endTransmission();

	if (rc != 0) {
		EBF_REPORT_ERROR(EBF_COMMUNICATION_PROBLEM);
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

// Clears the display and moves to the first row/col position
uint8_t EBF_SparkFun_QWIIC_SerLCD::Clear()
{
	uint8_t rc;

	rc = SendSettingCommand(LCD_SET_CLEAR);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// TODO: delay() is bad, but looks like there is no other way to do it
	delayMicroseconds(1500);	// According to datasheet, the HD44780 needs 1.52mSec after clear command

	return EBF_OK;
}

// Moves to the first row/col position without clearing the display
uint8_t EBF_SparkFun_QWIIC_SerLCD::Home()
{
	uint8_t rc;

	rc = SendSettingCommand(LCD_CMD_RETURNHOME);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	// TODO: delay() is bad, but looks like there is no other way to do it
	delayMicroseconds(1500);	// According to datasheet, the HD44780 needs 1.52mSec after home command

	return EBF_OK;
}

// Moves the cursor to specified row and column
uint8_t EBF_SparkFun_QWIIC_SerLCD::SetCursor(uint8_t col, uint8_t row)
{
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

	// keep variables in bounds
	row = min(row, (byte)(MAX_ROWS - 1)); //row cannot be greater than max rows

	//send the command
	return SendSpecialCommand(LCD_CMD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turns ON the displayed text
uint8_t EBF_SparkFun_QWIIC_SerLCD::DisplayOn()
{
	displayControl |= LCD_DISPLAYON;
	return SendSpecialCommand(LCD_CMD_DISPLAYCONTROL | displayControl);
}

// Turns OFF the displayed text
uint8_t EBF_SparkFun_QWIIC_SerLCD::DisplayOff()
{
	displayControl &= ~LCD_DISPLAYON;
	return SendSpecialCommand(LCD_CMD_DISPLAYCONTROL | displayControl);
}

// Turns ON the underline cursor
uint8_t EBF_SparkFun_QWIIC_SerLCD::CursorOn()
{
	displayControl |= LCD_CURSORON;
	return SendSpecialCommand(LCD_CMD_DISPLAYCONTROL | displayControl);
}

// Turns OFF the underline cursor
uint8_t EBF_SparkFun_QWIIC_SerLCD::CursorOff()
{
	displayControl &= ~LCD_CURSORON;
	return SendSpecialCommand(LCD_CMD_DISPLAYCONTROL | displayControl);
}

// Enable the blinking cursor
uint8_t EBF_SparkFun_QWIIC_SerLCD::BlinkOn()
{
	displayControl |= LCD_BLINKON;
	return SendSpecialCommand(LCD_CMD_DISPLAYCONTROL | displayControl);
}

// Disable the blinking cursor
uint8_t EBF_SparkFun_QWIIC_SerLCD::BlinkOff()
{
	displayControl &= ~LCD_BLINKON;
	return SendSpecialCommand(LCD_CMD_DISPLAYCONTROL | displayControl);
}

// Scroll the display left by 1 character
uint8_t EBF_SparkFun_QWIIC_SerLCD::ScrollLeft()
{
	return SendSpecialCommand(LCD_CMD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

// Scroll the display left by specified number of characters
uint8_t EBF_SparkFun_QWIIC_SerLCD::ScrollLeft(uint8_t count)
{
	return SendSpecialCommand(LCD_CMD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT, count);
}

// Scroll the display right by 1 character
uint8_t EBF_SparkFun_QWIIC_SerLCD::ScrollRight()
{
	return SendSpecialCommand(LCD_CMD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// Scroll the display right by specified number of characters
uint8_t EBF_SparkFun_QWIIC_SerLCD::ScrollRight(uint8_t count)
{
	return SendSpecialCommand(LCD_CMD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT, count);
}

// Move the cursor left by 1 character
uint8_t EBF_SparkFun_QWIIC_SerLCD::CursorLeft()
{
	return SendSpecialCommand(LCD_CMD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVELEFT);
}

// Move the cursor left by specified number of characters
uint8_t EBF_SparkFun_QWIIC_SerLCD::CursorLeft(uint8_t count)
{
	return SendSpecialCommand(LCD_CMD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVELEFT, count);
}

// Move the cursor right by 1 character
uint8_t EBF_SparkFun_QWIIC_SerLCD::CursorRight()
{
	return SendSpecialCommand(LCD_CMD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVERIGHT);
}

// Move the cursor right by specified number of characters
uint8_t EBF_SparkFun_QWIIC_SerLCD::CursorRight(uint8_t count)
{
	return SendSpecialCommand(LCD_CMD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVERIGHT, count);
}

// Set the backlight color to specified R,G,B values.
// This function will have no effect for LCDs with single backlight color
uint8_t EBF_SparkFun_QWIIC_SerLCD::SetBacklight(uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t rc;

	pI2C->beginTransmission(i2cAddress);
	pI2C->write(SETTING_COMMAND);
	pI2C->write(SET_RGB_COMMAND);
	pI2C->write(r);
	pI2C->write(g);
	pI2C->write(b);
	rc = pI2C->endTransmission();

	if (rc != 0) {
		EBF_REPORT_ERROR(EBF_COMMUNICATION_PROBLEM);
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}

// Set the backlight color to ULONG encoded value in format 0x00RRGGBB
uint8_t EBF_SparkFun_QWIIC_SerLCD::SetBacklight(uint32_t rgb)
{
	// convert from hex triplet to byte values
	uint8_t r = (rgb >> 16) & 0x0000FF;
	uint8_t g = (rgb >> 8) & 0x0000FF;
	uint8_t b = rgb & 0x0000FF;

	return SetBacklight(r, g, b);
}

// Enables the system messages, showing the connection settings, contrast, etc.
uint8_t EBF_SparkFun_QWIIC_SerLCD::SystemMessageOn()
{
	return SendSettingCommand(ENABLE_SYSTEM_MESSAGE_DISPLAY);
}

// Disables the system messages, showing the connection settings, contrast, etc.
uint8_t EBF_SparkFun_QWIIC_SerLCD::SystemMessageOff()
{
	return SendSettingCommand(DISABLE_SYSTEM_MESSAGE_DISPLAY);
}

// Enables splash screen after power up
uint8_t EBF_SparkFun_QWIIC_SerLCD::SplashScreenOn()
{
	return SendSettingCommand(ENABLE_SPLASH_DISPLAY);
}

// Disabled splash screen after power up
uint8_t EBF_SparkFun_QWIIC_SerLCD::SplashScreenOff()
{
	return SendSettingCommand(DISABLE_SPLASH_DISPLAY);
}

// Saves to EEPROM currently displayed text as a spash screen
uint8_t EBF_SparkFun_QWIIC_SerLCD::SaveSplashScreen()
{
	return SendSettingCommand(SAVE_CURRENT_DISPLAY_AS_SPLASH);
}

// Changes the direction of text flow to LTR - Left-To-Right
uint8_t EBF_SparkFun_QWIIC_SerLCD::DirectionLTR()
{
	displayMode |= LCD_ENTRYLEFT;
	return SendSpecialCommand(LCD_CMD_ENTRYMODESET | displayMode);
}

// Changes the direction of text flow to RTL - Right-To-Left
uint8_t EBF_SparkFun_QWIIC_SerLCD::DirectionRTL()
{
	displayMode &= ~LCD_ENTRYLEFT;
	return SendSpecialCommand(LCD_CMD_ENTRYMODESET | displayMode);
}

// Enable the autoscroll
uint8_t EBF_SparkFun_QWIIC_SerLCD::AutoScrollOn()
{
	displayMode |= LCD_ENTRYSHIFTINCREMENT;
	return SendSpecialCommand(LCD_CMD_ENTRYMODESET | displayMode);
}

// Disable the autoscroll
uint8_t EBF_SparkFun_QWIIC_SerLCD::AutoScrollOff()
{
	displayMode &= ~LCD_ENTRYSHIFTINCREMENT;
	return SendSpecialCommand(LCD_CMD_ENTRYMODESET | displayMode);
}

// Set the conrast from 0 to 255. Default value is 120
// This API changes the intensity of the backlight
uint8_t EBF_SparkFun_QWIIC_SerLCD::SetContrast(uint8_t contrast)
{
	uint8_t rc;

	pI2C->beginTransmission(i2cAddress);
	pI2C->write(SETTING_COMMAND);
	pI2C->write(CONTRAST_COMMAND);
	pI2C->write(contrast);
	rc = pI2C->endTransmission();

	if (rc != 0) {
		EBF_REPORT_ERROR(EBF_COMMUNICATION_PROBLEM);
		return EBF_COMMUNICATION_PROBLEM;
	}

	return EBF_OK;
}
