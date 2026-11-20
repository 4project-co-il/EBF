#include "EBF_Led.h"
#include "../Core/EBF_Core.h"

uint8_t EBF_Led::Init(uint8_t pinNumber)
{
	uint8_t rc;

	rc = EBF_PwmOutput::Init(pinNumber);
	if (rc != EBF_OK) {
		EBF_REPORT_AND_RETURN(rc);
	}

	state = LED_OFF;
	// Full brightness by default
	brightness = 255;

	EBF_REPORT_AND_RETURN(EBF_OK);
}

// SetValue acts as an ON/OFF function, value == 0 will perform as OFF, any other value as ON
uint8_t EBF_Led::SetValue(uint8_t value)
{
	uint8_t rc;

	if (value == 0) {
		state = LED_OFF;
	} else {
		state = LED_ON;
		// correct the value for PwmOutput
		value = brightness;
	}

	rc = EBF_PwmOutput::SetValue(value);

	EBF_REPORT_AND_RETURN(rc);
}

// GetValue returns 0 when the LED is OFF, or brightness level when the LED is ON
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
			return brightness;
	}

	return 0;
}

uint8_t EBF_Led::On()
{
	uint8_t rc;

	state = LED_ON;

	rc = EBF_PwmOutput::SetValue(brightness);

	EBF_REPORT_AND_RETURN(rc);
}

uint8_t EBF_Led::Off()
{
	uint8_t rc;

	state = LED_OFF;

	rc = EBF_PwmOutput::SetValue(0);

	EBF_REPORT_AND_RETURN(rc);
}

// Brightness will work only on hardware PWM enabled pins
uint8_t EBF_Led::SetBrightness(float percent)
{
	uint8_t rc = EBF_OK;

	if (percent < 0.0) {
		percent = 0.0;
	}
	if (percent > 100.0) {
		percent = 100.0;
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

	EBF_REPORT_AND_RETURN(rc);
}

// Turns on for msOn milliSeconds and stay off for msOff milliSeconds, for the repeatTimes times
uint8_t EBF_Led::Blink(uint16_t msOn, uint16_t msOff, uint8_t repeatTimes)
{
	uint8_t rc;

	onDuration_FadingDuration = msOn;
	offDuration = msOff;
	fadeUpdatePeriod_repeatTimes = repeatTimes-1;

	// Start from the BLINKING_ON state
	state = LED_BLINKING_ON;
	effectStart = micros();

	// Force processing, it will recalculate the needed polling interval there
	SetPollingInterval(0);

	rc = EBF_PwmOutput::SetValue(brightness);

	EBF_REPORT_AND_RETURN(rc);
}

// The led will fade in, from OFF to ON (up to brightness)
// The process will last msDuration milli-seconds
// and the value will be updated every msUpdate milli-seconds, 20mSec by default
uint8_t EBF_Led::FadeIn(uint16_t msDuration, uint8_t msUpdate)
{
	uint8_t rc;

	// Save the update period and set the polling interval accordingly
	fadeUpdatePeriod_repeatTimes = msUpdate;
	SetPollingInterval(fadeUpdatePeriod_repeatTimes);

	// Save total fading duration
	onDuration_FadingDuration = msDuration;

	effectStart = micros();
	state = LED_FADING_IN;

	rc = EBF_PwmOutput::SetValue(0);

	EBF_REPORT_AND_RETURN(rc);
}

// The led will fade out, from ON (current brightness) to OFF
// The process will last msDuration milli-seconds
// and the value will be updated every msUpdate milli-seconds, 20mSec by default
uint8_t EBF_Led::FadeOut(uint16_t msDuration, uint8_t msUpdate)
{
	uint8_t rc;

	// Save the update period and set the polling interval accordingly
	fadeUpdatePeriod_repeatTimes = msUpdate;
	SetPollingInterval(fadeUpdatePeriod_repeatTimes);

	// Save total fading duration
	onDuration_FadingDuration = msDuration;

	effectStart = micros();
	state = LED_FADING_OUT;

	rc = EBF_PwmOutput::SetValue(brightness);

	EBF_REPORT_AND_RETURN(rc);
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

		if (timePassed > onDuration_FadingDuration * 1000) {
			// On duration passed, turn the led off and set the data for BLINKING_OFF state
			rc = EBF_PwmOutput::SetValue(0);

			// No more repeatitions for the blink
			if (fadeUpdatePeriod_repeatTimes == 0) {
				// Move to OFF state
				state = LED_OFF;

				SetPollingInterval(EBF_NO_POLLING);
			} else {
				state = LED_BLINKING_OFF;
				effectStart = micros();

				// next polling is the duration of the OFF state
				SetPollingInterval(offDuration);
			}
		} else {
			// On duration didn't pass yet, processing was called before needed
			// Set polling to the time left
			SetPollingInterval(onDuration_FadingDuration - timePassed / 1000);
		}
		break;

	case LED_BLINKING_OFF:
		timePassed = micros() - effectStart;

		if (timePassed > offDuration * 1000) {
			// Off duration passed, turn the led on and set the data for BLINKING_ON state
			rc = EBF_PwmOutput::SetValue(brightness);

			state = LED_BLINKING_ON;
			effectStart = micros();

			// Reduce one repeat count if it's not set to infinite repetition
			if (fadeUpdatePeriod_repeatTimes != INFINITE_REPEAT) {
				fadeUpdatePeriod_repeatTimes--;
			}

			// next polling is the duration of the OFF state
			SetPollingInterval(onDuration_FadingDuration);
		} else {
			// On duration didn't pass yet, processing was called before needed
			// Set polling to the time left
			SetPollingInterval(offDuration - timePassed / 1000);
		}
		break;

	case LED_FADING_IN:
		timePassed = (micros() - effectStart) / 1000;	// in mSec

		// Calculate the brightness after passed time
		if (timePassed >= onDuration_FadingDuration) {
			// effect time passed, set to max brightness
			fadingBrightness = brightness;
		} else {
			fadingBrightness = (uint8_t)((1.0 - (onDuration_FadingDuration - timePassed) / (float)onDuration_FadingDuration) * brightness);
		}

		// Set current brightness level
		rc = EBF_PwmOutput::SetValue(fadingBrightness);

		if (fadingBrightness == brightness) {
			// We've reached max fading level
			state = LED_ON;

			SetPollingInterval(EBF_NO_POLLING);
		} else {
			// Need more time for fading
			if (onDuration_FadingDuration - timePassed > fadeUpdatePeriod_repeatTimes) {
				SetPollingInterval(fadeUpdatePeriod_repeatTimes);
			} else {
				SetPollingInterval(onDuration_FadingDuration - timePassed);
			}
		}
		break;

	case LED_FADING_OUT:
		timePassed = (micros() - effectStart) / 1000;	// in mSec

		// Calculate the brightness after passed time
		if (timePassed >= onDuration_FadingDuration) {
			// effect time passed, set to OFF
			fadingBrightness = 0;
		} else {
			fadingBrightness = (uint8_t)((onDuration_FadingDuration - timePassed) / (float)onDuration_FadingDuration * brightness);
		}

		// Set current brightness level
		rc = EBF_PwmOutput::SetValue(fadingBrightness);

		if (fadingBrightness == 0) {
			// We've reached min fading level (OFF)
			state = LED_OFF;

			SetPollingInterval(EBF_NO_POLLING);
		} else {
			// Need more time for fading
			if (onDuration_FadingDuration - timePassed > fadeUpdatePeriod_repeatTimes) {
				SetPollingInterval(fadeUpdatePeriod_repeatTimes);
			} else {
				SetPollingInterval(onDuration_FadingDuration - timePassed);
			}
		}
		break;

	default:
		rc = EBF_INVALID_STATE;
		break;
	}

	EBF_REPORT_AND_RETURN(rc);
}