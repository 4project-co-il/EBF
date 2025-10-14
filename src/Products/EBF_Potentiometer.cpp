#include "EBF_Potentiometer.h"
#include "../Core/EBF_Core.h"

extern void EBF_EmptyCallback();

uint8_t EBF_Potentiometer::Init(uint8_t pinNumber, uint8_t changePercent, uint8_t numberOfAveragingSamples)
{
	uint8_t rc;

	// Use empty callback function to allow Process and ProcessCallback calls
	rc = EBF_AnalogInput::Init(pinNumber, EBF_EmptyCallback, changePercent);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	numOfSamples = numberOfAveragingSamples;

	// allocate samples array
	pSamples = (uint16_t*)malloc(sizeof(uint16_t) * numOfSamples);
	if (pSamples == NULL) {
		EBF_REPORT_ERROR(EBF_NOT_ENOUGH_MEMORY);
		return EBF_NOT_ENOUGH_MEMORY;
	}

	// Fill all the samples with the same reading at the beginning to have initial value
	averageRawValue = EBF_AnalogInput::GetRawValue();

	for (currentSample=0; currentSample<numOfSamples; currentSample++) {
		pSamples[currentSample] = averageRawValue;
	}

	// Start sampling from the beginning of the array
	currentSample = 0;

	return EBF_OK;
}

uint16_t EBF_Potentiometer::GetRawAverage()
{
	uint8_t i;
	uint32_t sum = 0;

	for (i=0; i<numOfSamples; i++) {
		sum += pSamples[i];
	}

	return sum / numOfSamples;
}

float EBF_Potentiometer::GetValue()
{
	return (int32_t)averageRawValue * 100.0 / adcResolution;
}

// Called to process analog input after pollInterval
// We can't use ProcessCallbacks in that case since we need to calculate the average value
// for every poll interval and not for every change
uint8_t EBF_Potentiometer::Process()
{
	int32_t change;
	uint16_t currentAverage;

	// Read current sample
	pSamples[currentSample] = EBF_AnalogInput::GetRawValue();

	// Advance index
	currentSample++;
	if (currentSample >= numOfSamples) {
		currentSample = 0;
	}

	currentAverage = GetRawAverage();

	change = ((int32_t)currentAverage - (int32_t)averageRawValue) * 100 / (int32_t)adcResolution;

	if (abs(change) > changePercent) {
		averageRawValue = currentAverage;
		ProcessCallback();
	}

	return EBF_OK;
}
