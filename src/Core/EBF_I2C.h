#ifndef __EBF_I2C_H__
#define __EBF_I2C_H__

#include <Arduino.h>
#include <Wire.h>

#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_HalInstance.h"

class EBF_I2C : protected EBF_HalInstance, public Stream {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_I2C");

	public:
		EBF_I2C() : Stream(Wire), i2c(Wire) {}
		EBF_I2C(TwoWire &i2cInstance) : Stream(i2cInstance), i2c(i2cInstance) { }

		uint8_t Init(EBF_CallbackType callbackFunc = NULL, uint8_t address = 0);
		void SetClock(uint32_t clock) { i2c.setClock(clock); }

		uint8_t Process();

		void SetPollInterval(uint32_t ms);

		// Stream and Print interfaces
		inline size_t write(unsigned long n) { return i2c.write((uint8_t)n); }
		inline size_t write(long n) { return i2c.write((uint8_t)n); }
		inline size_t write(unsigned int n) { return i2c.write((uint8_t)n); }
		inline size_t write(int n) { return i2c.write((uint8_t)n); }
		inline size_t write(uint8_t data) { return i2c.write(data); }
		inline size_t write(uint8_t* pData, uint8_t dataSize) { return i2c.write(pData, dataSize); }

		inline int available(void) { return i2c.available(); }
		inline int peek(void) { return i2c.peek(); }
		inline int read(void) { return i2c.read(); }

		virtual void beginTransmission(uint8_t address) { i2c.beginTransmission(address); }
		inline uint8_t endTransmission(void) { return i2c.endTransmission(); }
		inline uint8_t endTransmission(uint8_t sendStop) { return i2c.endTransmission(sendStop); }
		inline uint8_t requestFrom(uint8_t address, uint8_t quantity) { return i2c.requestFrom(address, quantity); }
		inline uint8_t requestFrom(int address, int quantity, int sendStop) { return i2c.requestFrom(address, quantity, sendStop); }


	protected:
		EBF_CallbackType callbackFunc;
		TwoWire &i2c;

};

#endif
