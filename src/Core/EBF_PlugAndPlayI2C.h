#ifndef __EBF_PLUGANDPLAY_I2C_H__
#define __EBF_PLUGANDPLAY_I2C_H__

#include <Arduino.h>
#include <Wire.h>

#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_I2C.h"
#include "EBF_PlugAndPlayHub.h"

class EBF_PlugAndPlayI2C : public EBF_I2C {
	public:
		EBF_PlugAndPlayI2C(EBF_I2C &i2c, EBF_PlugAndPlayHub* pHub, uint8_t port);

		void beginTransmission(uint8_t address);

		uint8_t GetPortNumber() { return portNumber; }
		EBF_PlugAndPlayHub* GetHub() { return pHub; }

	private:
		EBF_PlugAndPlayHub* pHub;
		uint8_t portNumber;

};

#endif
