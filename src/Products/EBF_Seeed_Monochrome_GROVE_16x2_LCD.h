#ifndef __EBF_SEEED_MONOCHROME_GROVE_16x2_LCD_H__
#define __EBF_SEEED_MONOCHROME_GROVE_16x2_LCD_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_HalInstance.h"
#include "../Core/EBF_I2CDevice.h"
#include "EBF_AiP31068_I2C_16x2_LCD.h"

class EBF_Seeed_Monochrome_GROVE_16x2_LCD : public EBF_AiP31068_I2C_16x2_LCD {
	public:
		EBF_Seeed_Monochrome_GROVE_16x2_LCD(EBF_I2C &i2cInterface) : EBF_AiP31068_I2C_16x2_LCD(&i2cInterface) { }
		EBF_Seeed_Monochrome_GROVE_16x2_LCD(EBF_I2C *pI2cInterface) : EBF_AiP31068_I2C_16x2_LCD(pI2cInterface) { }

	protected:
		uint8_t Process();
};

#endif
