#include "EBF_Serial.h"
#include "EBF_Core.h"

#if defined(ARDUINO_ARCH_AVR)
EBF_Serial::EBF_Serial() : Stream(Serial)
{
	type = SERIAL_HW;
	pHwSerial = &Serial;
}

EBF_Serial::EBF_Serial(HardwareSerial &serialInstance) : Stream(serialInstance)
{
	type = SERIAL_HW;
	pHwSerial = &serialInstance;
}
#endif

#if defined(ARDUINO_ARCH_SAMD)
EBF_Serial::EBF_Serial() : Stream(SerialUSB)
{
	type = SERIAL_USB;
	pUsbSerial = &SerialUSB;
}

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

	rc = EBF_HalInstance::Init(HAL_Type::UART_INTERFACE, serialNumber);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	this->callbackFunc = callbackFunc;

	if (callbackFunc == NULL) {
		// No callback. No need to poll in that case
		SetPollingInterval(EBF_NO_POLLING);
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

void EBF_Serial::SetPollingInterval(uint32_t ms)
{
	// No polling needed if there is no callback to call
	if (callbackFunc == NULL) {
		EBF_HalInstance::SetPollingInterval(EBF_NO_POLLING);
	} else {
		EBF_HalInstance::SetPollingInterval(ms);
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
	size_t rc = 0;

	switch (type)
	{
#if defined(ARDUINO_ARCH_AVR)
	case SERIAL_HW:
		noInterrupts();
		rc = pHwSerial->write((uint8_t)n);
		interrupts();
		break;
#endif

#if defined(ARDUINO_ARCH_SAMD)
	case SERIAL_USB:
		noInterrupts();
		rc = pUsbSerial->write((uint8_t)n);
		interrupts();
		break;

	case SERIAL_UART:
		noInterrupts();
		rc = pUartSerial->write((uint8_t)n);
		interrupts();
		break;
#endif

	default:
		rc = 0;
		break;
	}

	return rc;
}

int EBF_Serial::available(void)
{
	int rc = 0;

	switch (type)
	{
#if defined(ARDUINO_ARCH_AVR)
	case SERIAL_HW:
		noInterrupts();
		rc = pHwSerial->available();
		interrupts();
		break;
#endif

#if defined(ARDUINO_ARCH_SAMD)
	case SERIAL_USB:
		noInterrupts();
		rc = pUsbSerial->available();
		interrupts();
		break;

	case SERIAL_UART:
		noInterrupts();
		rc = pUartSerial->available();
		interrupts();
		break;
#endif

	default:
		rc = 0;
		break;
	}

	return rc;
}

int EBF_Serial::peek(void)
{
	int rc = 0;

	switch (type)
	{
#if defined(ARDUINO_ARCH_AVR)
	case SERIAL_HW:
		noInterrupts();
		rc = pHwSerial->peek();
		interrupts();
		break;
#endif

#if defined(ARDUINO_ARCH_SAMD)
	case SERIAL_USB:
		noInterrupts();
		rc = pUsbSerial->peek();
		interrupts();
		break;

	case SERIAL_UART:
		noInterrupts();
		rc = pUartSerial->peek();
		interrupts();
		break;
#endif

	default:
		rc = 0;
		break;
	}

	return rc;
}

int EBF_Serial::read(void)
{
	int rc = 0;

	switch (type)
	{
#if defined(ARDUINO_ARCH_AVR)
	case SERIAL_HW:
		noInterrupts();
		rc = pHwSerial->read();
		interrupts();
		break;
#endif

#if defined(ARDUINO_ARCH_SAMD)
	case SERIAL_USB:
		noInterrupts();
		rc = pUsbSerial->read();
		interrupts();
		break;

	case SERIAL_UART:
		noInterrupts();
		rc = pUartSerial->read();
		interrupts();
		break;
#endif

	default:
		rc = 0;
		break;
	}

	return rc;
}
