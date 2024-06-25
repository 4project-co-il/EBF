#include "EBF_Serial.h"

#if defined(ARDUINO_ARCH_AVR)
EBF_Serial::EBF_Serial(HardwareSerial &serialInstance) : Stream(serialInstance)
{
	type = SERIAL_HW;
	pHwSerial = &serialInstance;
}
#endif

#if defined(ARDUINO_ARCH_SAMD)
EBF_Serial::EBF_Serial(Serial_ &serialInstance) : Stream(serialInstance)
{
	type = SERIAL_USB;
	pUsbSerial = &serialInstance;
}

EBF_Serial::EBF_Serial(Uart &serialInstance) : Stream(serialInstance)
{
	type = SERIAL_UART;
	pUartSerial = &serialInstance;
}
#endif

uint8_t EBF_Serial::Init(
	uint8_t serialNumber,
	EBF_CallbackType callbackFunc,
	uint32_t boudRate,
	uint16_t config
)
{
	uint8_t rc;

	rc = EBF_HalInstance::Init(HAL_Type::UART, serialNumber);
	if (rc != EBF_OK) {
		return rc;
	}

	this->hwNumber = hwNumber;
	this->callbackFunc = callbackFunc;

	if (callbackFunc == NULL) {
		// No callback. No need to poll in that case
		pollIntervalMs = EBF_NO_POLLING;
	}

	switch (type)
	{
#if defined(ARDUINO_ARCH_AVR)
	case SERIAL_HW:
		pHwSerial->begin(boudRate, config);
		break;
#endif
#if defined(ARDUINO_ARCH_SAMD)
	case SERIAL_USB:
		pUsbSerial->begin(boudRate, config);
		break;

	case SERIAL_UART:
		pUartSerial->begin(boudRate, config);
		break;
#endif

	default:
		break;
	}

	return EBF_OK;
}

void EBF_Serial::SetPollInterval(uint16_t ms)
{
	// No polling needed if there is no callback to call
	if (callbackFunc == NULL) {
		pollIntervalMs = EBF_NO_POLLING;
	} else {
		pollIntervalMs = ms;
	}
}

uint8_t EBF_Serial::Process()
{
	// Callback might not be set, nothing to do in that case
	if (callbackFunc == NULL) {
		return EBF_OK;
	}

	// The stream have data, call the callback
	if (available()) {
		callbackFunc();
	}

	return EBF_OK;
}

EBF_Serial::operator bool()
{
	switch (type)
	{
#if defined(ARDUINO_ARCH_AVR)
	case SERIAL_HW:
		return *pHwSerial;
		break;
#endif

#if defined(ARDUINO_ARCH_SAMD)
	case SERIAL_USB:
		return *pUsbSerial;
		break;

	case SERIAL_UART:
		return *pUartSerial;
		break;
#endif

	default:
		return NULL;
		break;
	}
}

size_t EBF_Serial::write(uint8_t n)
{
	switch (type)
	{
#if defined(ARDUINO_ARCH_AVR)
	case SERIAL_HW:
		return pHwSerial->write((uint8_t)n);
		break;
#endif

#if defined(ARDUINO_ARCH_SAMD)
	case SERIAL_USB:
		return pUsbSerial->write((uint8_t)n);
		break;

	case SERIAL_UART:
		return pUartSerial->write((uint8_t)n);
		break;
#endif

	default:
		return 0;
		break;
	}
}

int EBF_Serial::available(void)
{
	switch (type)
	{
#if defined(ARDUINO_ARCH_AVR)
	case SERIAL_HW:
		return pHwSerial->available();
		break;
#endif

#if defined(ARDUINO_ARCH_SAMD)
	case SERIAL_USB:
		return pUsbSerial->available();
		break;

	case SERIAL_UART:
		return pUartSerial->available();
		break;
#endif

	default:
		return 0;
		break;
	}
}

int EBF_Serial::peek(void)
{
	switch (type)
	{
#if defined(ARDUINO_ARCH_AVR)
	case SERIAL_HW:
		return pHwSerial->peek();
		break;
#endif

#if defined(ARDUINO_ARCH_SAMD)
	case SERIAL_USB:
		return pUsbSerial->peek();
		break;

	case SERIAL_UART:
		return pUartSerial->peek();
		break;
#endif

	default:
		return 0;
		break;
	}
}

int EBF_Serial::read(void)
{
	switch (type)
	{
#if defined(ARDUINO_ARCH_AVR)
	case SERIAL_HW:
		return pHwSerial->read();
		break;
#endif

#if defined(ARDUINO_ARCH_SAMD)
	case SERIAL_USB:
		return pUsbSerial->read();
		break;

	case SERIAL_UART:
		return pUsbSerial->read();
		break;
#endif

	default:
		return 0;
		break;
	}
}
