#ifndef __EBF_SEEED_MONOCHROME_GROVE_16x2_LCD_H__
#define __EBF_SEEED_MONOCHROME_GROVE_16x2_LCD_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_HalInstance.h"
#include "../HAL/EBF_HAL_AiP31068.h"

class EBF_Seeed_Monochrome_GROVE_16x2_LCD : protected EBF_HalInstance, public Print {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_Seeed_Monochrome_GROVE_16x2_LCD");

	public:
		EBF_Seeed_Monochrome_GROVE_16x2_LCD(EBF_I2C *pI2cInterface) : chip(pI2cInterface) {}
		EBF_Seeed_Monochrome_GROVE_16x2_LCD(EBF_I2C &i2cInterface) : EBF_Seeed_Monochrome_GROVE_16x2_LCD(&i2cInterface) { }

		uint8_t Init(uint8_t i2cAddress = EBF_HAL_AiP31068::defaultI2CAddress);

		// Print class interface
		size_t write(uint8_t b) { return chip.WriteChar(b); }
		size_t write(const uint8_t *buffer, size_t size) { return chip.WriteChars(buffer, size); }

		// LCD commands
		uint8_t Clear() { return chip.Clear(); }
		uint8_t Home() { return chip.Home(); }
		uint8_t SetCursor(uint8_t col, uint8_t row) { return chip.SetCursor(col, row); }
		uint8_t DisplayOn() { return chip.DisplayOn(); }
		uint8_t DisplayOff() { return chip.DisplayOff(); }
		uint8_t CursorOn() { return chip.CursorOn(); }
		uint8_t CursorOff() { return chip.CursorOff(); }
		uint8_t BlinkOn() { return chip.BlinkOn(); }
		uint8_t BlinkOff() { return chip.BlinkOff(); }
		uint8_t ScrollLeft() { return chip.ScrollLeft(); }
		uint8_t ScrollLeft(uint8_t count) { return chip.ScrollLeft(count); }
		uint8_t ScrollRight() { return chip.ScrollRight(); }
		uint8_t ScrollRight(uint8_t count) { return chip.ScrollRight(count); };
		uint8_t DirectionLTR() { return chip.DirectionLTR(); }
		uint8_t DirectionRTL() { return chip.DirectionRTL(); }
		uint8_t AutoScrollOn() { return chip.AutoScrollOn(); }
		uint8_t AutoScrollOff() { return chip.AutoScrollOff(); }

	protected:
		// LCD Driver AiP31068 chip
		EBF_HAL_AiP31068 chip;

		uint8_t Process();
};

#endif
