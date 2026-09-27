#ifndef __EBF_TMP102_TEMPERATURESENSOR_H__
#define __EBF_TMP102_TEMPERATURESENSOR_H__

#include <Arduino.h>
#if __has_include("Project_Config.h")
	#include "Project_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_Logic.h"
#include "../Core/EBF_HalInstance.h"
#include "../HAL/EBF_HAL_TMP102.h"

class EBF_TMP102_TemperatureSensor : public EBF_HalInstance {
	private:
		EBF_DEBUG_MODULE_NAME("EBF_TMP102_TemperatureSensor");

	public:
		EBF_TMP102_TemperatureSensor(EBF_I2C *pI2cInterface) : chip(pI2cInterface) { }
		EBF_TMP102_TemperatureSensor(EBF_I2C &i2cInterface) : EBF_TMP102_TemperatureSensor(&i2cInterface) { }

		typedef enum : uint8_t {
			POWER_DOWN = 0,
			MODE_ONE_SHOT,
			MODE_0_25HZ,
			MODE_1HZ,
			MODE_4HZ,
			MODE_8HZ,
		} OperationMode;

		typedef enum : uint8_t {
			FAULTS_1	= EBF_HAL_TMP102::FAULTS_1,
			FAULTS_2	= EBF_HAL_TMP102::FAULTS_2,
			FAULTS_4	= EBF_HAL_TMP102::FAULTS_4,
			FAULTS_6	= EBF_HAL_TMP102::FAULTS_6,
		} ConsecutiveFaults;

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
		void SetOnThresholdHigh(EBF_CallbackType onThresholdHigh) { this->onThresholdHigh = onThresholdHigh; }
		void SetOnThresholdLow(EBF_CallbackType onThresholdLow) { this->onThresholdLow = onThresholdLow; }

		// Changes device operation mode
		uint8_t SetOperationMode(OperationMode mode);
		// Returns TRUE (1) while the device is busy performing the one-shot measurement
		uint8_t IsBusy() { return chip.IsBusy(); }

		// Sets high threshold value
		uint8_t SetThresholdHigh(float value);
		// Sets low threshold value
		uint8_t SetThresholdLow(float value);
		// Gets high threshold value
		float GetThresholdHigh();
		// Gets low threshold value
		float GetThresholdLow();
		// Sets the consecutive number of times for the reading to pass threshold to produce interrupt
		uint8_t SetConsecutiveFaults(ConsecutiveFaults faults);
		// Gets the consecutive number of times for the reading to pass threshold to produce interrupt
		EBF_TMP102_TemperatureSensor::ConsecutiveFaults GetConsecutiveFaults();

		// Returns the measured temperature in Celsius
		float GetValueC();
		// Returns the measured temperature in Fahrenheit
		float GetValueF();
		// Returns the measured temperature in Kelvin
		float GetValueK();

	protected:
		// TMP102 chip
		EBF_HAL_TMP102 chip;

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
			STATE_MEASURING
		};

		InstanceState state;
		OperationMode operationMode;
		float highThreshold;
		float lowThreshold;
		float lastValue;
		uint8_t changePercent;
		uint8_t interruptAttached;

		// Callbacks
		EBF_CallbackType onChangeCallback;
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
