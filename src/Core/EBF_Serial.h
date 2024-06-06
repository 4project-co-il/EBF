#ifndef __EBF_SERIAL_H__
#define __EBF_SERIAL_H__

#include <Arduino.h>

#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_HalInstance.h"

class EBF_Serial : protected EBF_HalInstance, public Stream {
	public:
		EBF_Serial(HardwareSerial &serialInstance) : Stream(serialInstance), serial(serialInstance) {};

		uint8_t Init(
			uint8_t serialNumber = 0,
			EBF_CallbackType callbackFunc = NULL,
			uint32_t boudRate = 115200,
			uint8_t config = SERIAL_8N1
		);
		uint8_t Process();

		void SetPollInterval(uint16_t ms);

		// Stream and Print interfaces
		inline size_t write(unsigned long n) { return serial.write((uint8_t)n); }
		inline size_t write(long n) { return serial.write((uint8_t)n); }
		inline size_t write(unsigned int n) { return serial.write((uint8_t)n); }
		inline size_t write(int n) { return serial.write((uint8_t)n); }
		inline size_t write(uint8_t n) { return serial.write((uint8_t)n); }

		inline int available(void) { return serial.available(); }
		inline int peek(void) { return serial.peek(); }
		inline int read(void) { return serial.read(); }

		operator bool() { return (serial); }

	private:
		uint8_t hwNumber;
		EBF_CallbackType callbackFunc;

		HardwareSerial &serial;

};

#endif
