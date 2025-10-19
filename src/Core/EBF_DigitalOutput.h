#ifndef __EBF_DIGITALOUTPUT_H__
#define __EBF_DIGITALOUTPUT_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "EBF_Global.h"
#include "EBF_HalInstance.h"

class EBF_DigitalOutput : protected EBF_HalInstance {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_DigitalOutput");

	public:
		uint8_t Init(uint8_t pinNumber);

		uint8_t SetValue(uint8_t value);

	protected:
		// No processing is needed for digital outputs
		uint8_t Process() { return EBF_OK; }

	protected:
		uint8_t pinNumber;
};

#endif
