#ifndef __EBF_STTS22H_TEMPERATURESENSOR_H__
#define __EBF_STTS22H_TEMPERATURESENSOR_H__

#include <Arduino.h>
#if __has_include("EBF_Config.h")
	#include "EBF_Config.h"
#endif

#include "../Core/EBF_Global.h"
#include "../Core/EBF_HalInstance.h"
#include "../Core/EBF_I2C.h"
#include <Wire.h>

class EBF_STTS22H_TemperatureSensor : protected EBF_HalInstance {
	public:
		EBF_STTS22H_TemperatureSensor(EBF_I2C &i2cInterface) : pI2C(&i2cInterface) { }
		EBF_STTS22H_TemperatureSensor(EBF_I2C *pI2cInterface) : pI2C(pI2cInterface) { }

		typedef enum : uint8_t {
			POWER_DOWN = 0,
			MODE_ONE_SHOT,
			MODE_1HZ,
			MODE_25HZ,
			MODE_50HZ,
			MODE_100HZ,
			MODE_200HZ
		} OperationMode;

		uint8_t Init(uint8_t i2cAddress, OperationMode mode = POWER_DOWN);
		// Setting polling interval in milli-seconds
		void SetPollInterval(uint32_t ms) { pollIntervalMs = ms; }
		// Call to attach the device to an interrupt line
		uint8_t AttachInterrupt(uint8_t interruptPin);

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
		uint8_t IsBusy();

		// Sets high threshold value
		uint8_t SetThresholdHigh(float temp);
		// Sets low threshold value
		uint8_t SetThresholdLow(float temp);
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
		enum InstanceState : uint8_t {
			STATE_IDLE = 0,
			STATE_ONE_SHOT,
			STATE_MEASURING
		};

		EBF_I2C *pI2C;
		uint8_t i2cAddress;
		InstanceState state;
		OperationMode operationMode;
		uint8_t highThresholdSet;
		uint8_t lowThresholdSet;
		float lastValue;
		uint8_t changePercent;
		uint8_t interruptAttached;

		// Post processing that should be done after interrupt handler
		volatile uint8_t postProcessingFlags;

		const uint8_t postProcessHighThreshold	= 1<<0;
		const uint8_t postProcessLowThreshold	= 1<<1;


		// Callbacks
		EBF_CallbackType onChangeCallback;
		EBF_CallbackType onMeasureComplete;
		EBF_CallbackType onThresholdHigh;
		EBF_CallbackType onThresholdLow;

		uint8_t Process();
		void ProcessInterrupt();
		void UpdatePollInterval();

	private:
		// Device registers data
		const uint8_t regTempHighLimit 	= 0x02;
		const uint8_t regTempLowLimit 	= 0x03;
		const uint8_t regControl 		= 0x04;
		const uint8_t regStatus			= 0x05;
		const uint8_t regTempOutput		= 0x06;

		typedef union {
			struct {
				uint8_t oneShot		: 1;
				uint8_t timeOutDis	: 1;
				uint8_t freeRun		: 1;
				uint8_t addrInc		: 1;
				uint8_t avg			: 2;
				uint8_t bdu			: 1;
				uint8_t mode_1Hz	: 1;
			} fields;
			uint8_t reg;
		} ControlRegister_t;

		const uint8_t avgMode_25Hz	= 0x00;
		const uint8_t avgMode_50Hz	= 0x01;
		const uint8_t avgMode_100Hz	= 0x02;
		const uint8_t avgMode_200Hz	= 0x03;

		typedef union {
			struct {
				uint8_t busy			: 1;
				uint8_t overThreshold	: 1;
				uint8_t underThreshold	: 1;
				uint8_t notUsed			: 5;
			} fields;
			uint8_t reg;
		} StatusRegister_t;

		uint8_t GetControlRegister(ControlRegister_t &ctrl);
		uint8_t SetControlRegister(ControlRegister_t ctrl);
		uint8_t GetStatusRegister(StatusRegister_t &status);
		uint8_t GetValueRaw(int16_t &value);
};

#endif
