#ifndef __EBF_STTS22H_TEMPERATURESENSOR_H__
#define __EBF_STTS22H_TEMPERATURESENSOR_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_HalInstance.h"
#include "../HAL/EBF_HAL_STTS22H.h"

class EBF_STTS22H_TemperatureSensor : public EBF_HalInstance {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_STTS22H_TemperatureSensor");

	public:
		EBF_STTS22H_TemperatureSensor(EBF_I2C *pI2cInterface) : chip(pI2cInterface) { }
		EBF_STTS22H_TemperatureSensor(EBF_I2C &i2cInterface) : EBF_STTS22H_TemperatureSensor(&i2cInterface) { }

		typedef enum : uint8_t {
			POWER_DOWN = 0,
			MODE_ONE_SHOT,
			MODE_1HZ,
			MODE_25HZ,
			MODE_50HZ,
			MODE_100HZ,
			MODE_200HZ
		} OperationMode;

		uint8_t Init(uint8_t i2cAddress = 0x3F, OperationMode mode = POWER_DOWN);

#ifdef EBF_USE_INTERRUPTS
		// Call to attach the device to an interrupt line
		uint8_t AttachInterrupt(uint8_t interruptPin);
		uint8_t PostponeProcessing();
		uint8_t InInterrupt() {
			EBF_Logic *pLogic = EBF_Logic::GetInstance();
			return pLogic->IsRunFromIsr();
		}
#else
		uint8_t PostponeProcessing() { return EBF_INVALID_STATE; }
		uint8_t InInterrupt() { return 0; }
#endif

		void SetOnChange(EBF_CallbackType onChangeCallback, uint8_t changePercent = 5)
		{
			this->onChangeCallback = onChangeCallback;
			this->changePercent = changePercent;
		}
		void SetOnMeasureComplete(EBF_CallbackType onMeasureComplete) { this->onMeasureComplete = onMeasureComplete; }
		void SetOnThresholdHigh(EBF_CallbackType onThresholdHigh) { this->onThresholdHigh = onThresholdHigh; }
		void SetOnThresholdLow(EBF_CallbackType onThresholdLow) { this->onThresholdLow = onThresholdLow; }

		// Changes device operation mode
		uint8_t SetOperationMode(OperationMode mode);
		// Returns TRUE (1) while the device is busy performing the one-shot measurement
		uint8_t IsBusy() { return chip.IsBusy(); }

		// Sets high threshold value
		uint8_t SetThresholdHigh(float temp);
		// Sets low threshold value
		uint8_t SetThresholdLow(float temp);
		// Gets high threshold value
		float GetThresholdHigh();
		// Gets low threshold value
		float GetThresholdLow();
		// Disable high threshold triggering
		uint8_t DisableThresholdHigh();
		// Disable low threshold triggering
		uint8_t DisableThresholdLow();

		// Returns the measured temperature in Celsius
		float GetValueC();
		// Returns the measured temperature in Fahrenheit
		float GetValueF();
		// Returns the measured temperature in Kelvin
		float GetValueK();

	protected:
		// STTS22H chip
		EBF_HAL_STTS22H chip;

		typedef union {
			struct {
				uint8_t highThreshold	: 1;
				uint8_t lowThreshold	: 1;
				uint32_t reserved 		: 30;
			} fields;
			uint32_t uint32;
		} PostponedInterruptData;

		enum InstanceState : uint8_t {
			STATE_IDLE = 0,
			STATE_ONE_SHOT,
			STATE_MEASURING
		};

		InstanceState state;
		OperationMode operationMode;
		uint8_t highThresholdSet;
		uint8_t lowThresholdSet;
		float lastValue;
		uint8_t changePercent;
		uint8_t interruptAttached;

		// Callbacks
		EBF_CallbackType onChangeCallback;
		EBF_CallbackType onMeasureComplete;
		EBF_CallbackType onThresholdHigh;
		EBF_CallbackType onThresholdLow;

		uint8_t Process();
		void ExecuteCallback(volatile PostponedInterruptData& data);
		void UpdatePollInterval();
#ifdef EBF_USE_INTERRUPTS
		void ProcessInterrupt();
#endif

		// Interrupt processing will set currently processing flags, so it could be used as a parameter for post-processing
		volatile PostponedInterruptData currentInterruptData;
};

#endif
