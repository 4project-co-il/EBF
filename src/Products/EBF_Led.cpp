#include "EBF_Led.h"
#include "../Core/EBF_Core.h"

uint8_t EBF_Led::Init(uint8_t pinNumber)
{
	uint8_t rc;

	rc = EBF_PwmOutput::Init(pinNumber);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	state = LED_OFF;
	// Full brightness by default
	brightness = 255;

	return EBF_OK;
}

// SetValue acts as an ON/OFF function, value == 0 will perform as OFF, any other value as ON
uint8_t EBF_Led::SetValue(uint8_t value)
{
	if (value == 0) {
		state = LED_OFF;
	} else {
		state = LED_ON;
		// correct the value for PwmOutput
		value = brightness;
	}

	return EBF_PwmOutput::SetValue(value);
}

// GetValue returns current status of the led (ON or OFF)
uint8_t EBF_Led::GetValue()
{
	switch (state)
	{
		case LED_OFF:
		case LED_BLINKING_OFF:
			return 0;

		case LED_ON:
		case LED_BLINKING_ON:
		case LED_FADING_IN:
		case LED_FADING_OUT:
			return 1;
	}

	return 0;
}

uint8_t EBF_Led::On()
{
	state = LED_ON;

	return EBF_PwmOutput::SetValue(brightness);
}

uint8_t EBF_Led::Off()
{
	state = LED_OFF;

	return EBF_PwmOutput::SetValue(0);
}

// Brightness will work only on hardware PWM enabled pins
uint8_t EBF_Led::SetBrightness(uint8_t percent)
{
	uint8_t rc = EBF_OK;

	if (percent > 100) {
		percent = 100;
	}

	brightness = (uint8_t)(percent / 100.0 * 255);

	switch (state)
	{
	case LED_OFF:
	case LED_BLINKING_OFF:
	case LED_FADING_IN:
	case LED_FADING_OUT:
		// do nothing
		break;

	case LED_ON:
	case LED_BLINKING_ON:
		// Update the brightness
		rc = EBF_PwmOutput::SetValue(brightness);
		break;

	default:
		rc = EBF_INVALID_STATE;
		break;
	}

	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// Turns on for msOn milliSeconds and stay off for msOff milliSeconds
uint8_t EBF_Led::Blink(uint16_t msOn, uint16_t msOff)
{
	uint8_t rc;

	onDuration = msOn;
	offDuration = msOff;

	// Start from the BLINKING_ON state
	state = LED_BLINKING_ON;
	effectStart = micros();

	// Force processing, it will recalculate the needed polling interval there
	SetPollingInterval(0);

	rc = EBF_PwmOutput::SetValue(brightness);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// The led will fade in, from OFF to ON (up to brightness)
// The process will last msDuration milli-seconds
// and the value will be updated every msUpdate milli-seconds, 20mSec by default
uint8_t EBF_Led::FadeIn(uint16_t msDuration, uint8_t msUpdate)
{
	uint8_t rc;

	// Save the update period and set the polling interval accordingly
	fadeUpdatePeriod = msUpdate;
	SetPollingInterval(fadeUpdatePeriod);

	// Save total fading duration
	onDuration = msDuration;

	effectStart = micros();
	state = LED_FADING_IN;

	rc = EBF_PwmOutput::SetValue((uint8_t)offDuration);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

// The led will fade out, from ON (current brightness) to OFF
// The process will last msDuration milli-seconds
// and the value will be updated every msUpdate milli-seconds, 20mSec by default
uint8_t EBF_Led::FadeOut(uint16_t msDuration, uint8_t msUpdate)
{
	uint8_t rc;

	// Save the update period and set the polling interval accordingly
	fadeUpdatePeriod = msUpdate;
	SetPollingInterval(fadeUpdatePeriod);

	// Save total fading duration
	onDuration = msDuration;

	effectStart = micros();
	state = LED_FADING_OUT;

	rc = EBF_PwmOutput::SetValue((uint8_t)onDuration);
	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}

uint8_t EBF_Led::Process()
{
	uint8_t rc = EBF_OK;
	unsigned long timePassed;
	uint16_t fadingBrightness;

	switch (state)
	{
	case LED_ON:
	case LED_OFF:
		// Nothing to do
		// No polling needed
		SetPollingInterval(EBF_NO_POLLING);
		break;

	case LED_BLINKING_ON:
		timePassed = micros() - effectStart;

		if (timePassed > onDuration * 1000) {
			// On duration passed, turn the led off and set the data for BLINKING_OFF state
			rc = EBF_PwmOutput::SetValue(0);

			state = LED_BLINKING_OFF;
			effectStart = micros();

			// next polling is the duration of the OFF state
			SetPollingInterval(offDuration);
		} else {
			// On duration didn't pass yet, processing was called before needed
			// Set polling to the time left
			SetPollingInterval(onDuration - timePassed / 1000);
		}
		break;

	case LED_BLINKING_OFF:
		timePassed = micros() - effectStart;

		if (timePassed > offDuration * 1000) {
			// Off duration passed, turn the led on and set the data for BLINKING_ON state
			rc = EBF_PwmOutput::SetValue(brightness);

			state = LED_BLINKING_ON;
			effectStart = micros();

			// next polling is the duration of the OFF state
			SetPollingInterval(onDuration);
		} else {
			// On duration didn't pass yet, processing was called before needed
			// Set polling to the time left
			SetPollingInterval(offDuration - timePassed / 1000);
		}
		break;

	case LED_FADING_IN:
		timePassed = (micros() - effectStart) / 1000;	// in mSec

		// Calculate the brightness after passed time
		if (timePassed >= onDuration) {
			// effect time passed, set to max brightness
			fadingBrightness = brightness;
		} else {
			fadingBrightness = (uint8_t)((1.0 - (onDuration - timePassed) / (float)onDuration) * brightness);
		}

		// Set current brightness level
		rc = EBF_PwmOutput::SetValue(fadingBrightness);

		if (fadingBrightness == brightness) {
			// We've reached max fading level
			state = LED_ON;

			SetPollingInterval(EBF_NO_POLLING);
		} else {
			// Need more time for fading
			if (onDuration - timePassed > fadeUpdatePeriod) {
				SetPollingInterval(fadeUpdatePeriod);
			} else {
				SetPollingInterval(onDuration - timePassed);
			}
		}
		break;

	case LED_FADING_OUT:
		timePassed = (micros() - effectStart) / 1000;	// in mSec

		// Calculate the brightness after passed time
		if (timePassed >= onDuration) {
			// effect time passed, set to OFF
			fadingBrightness = 0;
		} else {
			fadingBrightness = (uint8_t)((onDuration - timePassed) / (float)onDuration * brightness);
		}

		// Set current brightness level
		rc = EBF_PwmOutput::SetValue(fadingBrightness);

		if (fadingBrightness == 0) {
			// We've reached min fading level (OFF)
			state = LED_OFF;

			SetPollingInterval(EBF_NO_POLLING);
		} else {
			// Need more time for fading
			if (onDuration - timePassed > fadeUpdatePeriod) {
				SetPollingInterval(fadeUpdatePeriod);
			} else {
				SetPollingInterval(onDuration - timePassed);
			}
		}
		break;

	default:
		rc = EBF_INVALID_STATE;
		break;
	}

	if (rc != EBF_OK) {
		EBF_REPORT_ERROR(rc);
		return rc;
	}

	return EBF_OK;
}