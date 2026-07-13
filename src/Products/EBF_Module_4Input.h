#ifndef __EBF_MODULE_4INPUT_H__
#define __EBF_MODULE_4INPUT_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_HalInstance.h"
#include "../HAL/EBF_HAL_TCAL9538.h"

class EBF_Module_4Input : public EBF_HalInstance {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_Module_4Input");

	public:
		EBF_Module_4Input(EBF_I2C &i2cInterface) : chip(i2cInterface) {}
		EBF_Module_4Input(EBF_I2C* pI2cInterface) : chip(pI2cInterface) {}

		static const uint8_t numberOfInputs = 4;

		uint8_t Init(uint8_t i2cAddress = EBF_HAL_TCAL9538::defaultI2CAddress);

		// Returns current value of the specified input line
		uint8_t GetValue(uint8_t index);
		// Returns current values of all the input lines
		uint8_t GetValues();
		// Returns last value of the specified input line as it appeared while reading from the chip
		uint8_t GetLastValue(uint8_t index);
		// Returns all last values as it appeared while reading from the chip
		uint8_t GetLastValues();

#ifdef EBF_USE_INTERRUPTS
		// Call to attach the device to an interrupt line
		uint8_t AttachInterrupt(uint8_t interruptPin);
		uint8_t PostponeProcessing(uint8_t eventIndex, uint8_t inputValues);
		uint8_t InInterrupt() {
			EBF_Logic *pLogic = EBF_Logic::GetInstance();
			return pLogic->IsRunFromIsr();
		}
#else
		uint8_t PostponeProcessing() { return EBF_INVALID_STATE; }
		uint8_t InInterrupt() { return 0; }
#endif

		uint8_t SetOnChange(uint8_t index, EBF_CallbackType onChangeCallback);

		// Returns input index that caused the callback function call
		// You can have the same callback function for all the inputs events
		// where you can call the GetEventIndex to know which input actually changed
		uint8_t GetEventIndex() { return currentEventIndex; }

		typedef union {
			struct {
				uint32_t index : 3;		// up to 8 inputs
				uint32_t event : 8;		// input event that should be executed
				uint32_t reserved : 21;
			} fields;
			uint32_t uint32;
		} PostponedInterruptData;

	protected:
		// I2C I/O ports chip
		EBF_HAL_TCAL9538 chip;

		// Callbacks
		EBF_CallbackType onChangeCallback[numberOfInputs];

		uint8_t lastValues;
		uint8_t currentEventIndex;

		virtual uint8_t Process();
		virtual void ExecuteCallback();

#ifdef EBF_USE_INTERRUPTS
		void ProcessInterrupt();
#endif
};

#endif
