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
		EBF_Serial(HardwareSerial &serialInstance);
#if defined(ARDUINO_ARCH_SAMD)
		EBF_Serial(Serial_ &serialInstance);
#endif
		uint8_t Init(
			uint8_t serialNumber = 0,
			EBF_CallbackType callbackFunc = NULL,
			uint32_t boudRate = 115200,
			uint16_t config = SERIAL_8N1
		);
		uint8_t Process();

		void SetPollInterval(uint16_t ms);

		// Stream and Print interfaces
		inline size_t write(unsigned long n) { return write((uint8_t)n); }
		inline size_t write(long n) { return write((uint8_t)n); }
		inline size_t write(unsigned int n) { return write((uint8_t)n); }
		inline size_t write(int n) { return write((uint8_t)n); }
		size_t write(uint8_t n);

		int available(void);
		int peek(void);
		int read(void);

		operator bool();

	private:
		enum SerialType : uint8_t {
			SERIAL_HW = 0,
			SERIAL_USB,
		};

		uint8_t hwNumber;
		EBF_CallbackType callbackFunc;

		// Need to keep different pointer types for initialization
		SerialType type;
		HardwareSerial* pHwSerial;
#if defined(ARDUINO_ARCH_SAMD)
		Serial_* pUsbSerial;
#endif


};

#endif
