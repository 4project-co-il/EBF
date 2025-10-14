#include "EBF_HC-SR04_DistanceSensor.h"
#include "../Core/EBF_Core.h"

extern void EBF_EmptyCallback();

uint8_t EBF_HCSR04_DistanceSensor::Init(uint8_t triggerPinNumber, uint8_t echoPinNumber)
{
	uint8_t rc;

	// Use empty callback function to allow Process and ProcessCallback calls
	rc = EBF_DigitalInput::Init(echoPinNumber, EBF_EmptyCallback, EBF_DigitalInput::InterruptMode::MODE_CHANGE);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	rc = trigger.Init(triggerPinNumber);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	this->changePercent = 100;
	this->lastValue = GetValue();

	return EBF_OK;
}

void EBF_HCSR04_DistanceSensor::SetOnChange(EBF_CallbackType onChangeCallback, uint8_t changePercent)
{
	this->callbackFunc = onChangeCallback;
	this->changePercent = changePercent;
}

float EBF_HCSR04_DistanceSensor::GetValue()
{
	unsigned long duration;

	trigger.SetValue(LOW);
	delayMicroseconds(2);
	trigger.SetValue(HIGH);
	delayMicroseconds(10);
	trigger.SetValue(LOW);

	duration = pulseIn(pinNumber, HIGH);

	return (duration * 0.0343) / 2;
}

// Called to process digital input after pollInterval
// We can't use ProcessCallbacks in that case since we need to detect the change
// in distance and not for every change of the digital input line
uint8_t EBF_HCSR04_DistanceSensor::Process()
{
	float distance;
	float change;

	// Read current sample
	distance = GetValue();

	change = distance - lastValue;

	if (abs(change) > changePercent) {
		lastValue = distance;
		ProcessCallback();
	}

	return EBF_OK;
}
