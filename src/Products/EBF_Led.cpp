#include "EBF_Led.h"

uint8_t EBF_Led::Init(uint8_t pinNumber)
{
	EBF_PwmOutput::Init(pinNumber);

	state = LED_OFF;
	// Full brightness by default
	brightness = 255;

	return 1;
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
	uint8_t rc = 1;

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
		EBF_PwmOutput::SetValue(brightness);
		break;

	default:
		rc = 0;
		break;
	}

	return rc;
}

// Turns on for msOn milliSeconds and stay off for msOff milliSeconds
uint8_t EBF_Led::Blink(uint16_t msOn, uint16_t msOff)
{
	onDuration = msOn;
	offDuration = msOff;

	// Start from the BLINKING_ON state
	state = LED_BLINKING_ON;
	effectStart = micros();

	// Force processing, it will recalculate the needed polling interval there
	pollIntervalMs = 0;

	EBF_PwmOutput::SetValue(brightness);

	return 1;
}

// The led will fade in, from OFF to ON (up to brightness)
// The process will last msDuration milli-seconds
// and will be done in the specified number of steps
uint8_t EBF_Led::FadeIn(uint16_t msDuration, uint8_t steps)
{
	// onDuration will be used to store step duration
	onDuration = msDuration / steps;
	pollIntervalMs = onDuration;

	// offDuration will be used to store current brightness
	// We will start from 0
	offDuration = 0;

	fadeStep = brightness / steps;

	effectStart = micros();
	state = LED_FADING_IN;

	EBF_PwmOutput::SetValue((uint8_t)offDuration);

	return 1;
}

// The led will fade out, from ON (current brightness) to OFF
// The process will last msDuration milli-seconds
// and will be done in the specified number of steps
uint8_t EBF_Led::FadeOut(uint16_t msDuration, uint8_t steps)
{
	// onDuration will be used to store step duration
	onDuration = msDuration / steps;
	pollIntervalMs = onDuration;

	// offDuration will be used to store current brightness
	// We will start from max brightness
	offDuration = brightness;

	fadeStep = brightness / steps;

	effectStart = micros();
	state = LED_FADING_OUT;

	EBF_PwmOutput::SetValue((uint8_t)offDuration);

	return 1;
}

uint8_t EBF_Led::Process()
{
	uint8_t rc = 1;
	unsigned long timePassed;

	switch (state)
	{
	case LED_ON:
	case LED_OFF:
		// Nothing to do
		// No polling needed
		pollIntervalMs = EBF_NO_POLLING;
		break;

	case LED_BLINKING_ON:
		timePassed = micros() - effectStart;

		if (timePassed > onDuration * 1000) {
			// On duration passed, turn the led off and set the data for BLINKING_OFF state
			EBF_PwmOutput::SetValue(0);

			state = LED_BLINKING_OFF;
			effectStart = micros();

			// next polling is the duration of the OFF state
			pollIntervalMs = offDuration;
		} else {
			// On duration didn't pass yet, processing was called before needed
			// Set polling to the time left
			pollIntervalMs = onDuration - timePassed / 1000;
		}
		break;

	case LED_BLINKING_OFF:
		timePassed = micros() - effectStart;

		if (timePassed > offDuration * 1000) {
			// Off duration passed, turn the led on and set the data for BLINKING_ON state
			EBF_PwmOutput::SetValue(brightness);

			state = LED_BLINKING_ON;
			effectStart = micros();

			// next polling is the duration of the OFF state
			pollIntervalMs = onDuration;
		} else {
			// On duration didn't pass yet, processing was called before needed
			// Set polling to the time left
			pollIntervalMs = offDuration - timePassed / 1000;
		}
		break;

	case LED_FADING_IN:
		timePassed = micros() - effectStart;

		if (timePassed > onDuration * 1000) {
			// offDuration will be used to store current brightness
			offDuration += fadeStep;

			if (offDuration > brightness) {
				offDuration = brightness;
			}

			EBF_PwmOutput::SetValue((uint8_t)offDuration);

			if (offDuration >= brightness) {
				// We've reached max brightness
				state = LED_ON;

				pollIntervalMs = EBF_NO_POLLING;
			} else {
				effectStart = micros();
				pollIntervalMs = onDuration;
			}
		} else {
			// Processing was called before needed
			// Set polling to the time left
			pollIntervalMs = onDuration - timePassed / 1000;
		}
		break;

	case LED_FADING_OUT:
		timePassed = micros() - effectStart;

		if (timePassed > onDuration * 1000) {
			// offDuration will be used to store current brightness
			if (fadeStep > offDuration) {
				offDuration = 0;
			} else {
				offDuration -= fadeStep;
			}

			EBF_PwmOutput::SetValue((uint8_t)offDuration);

			if (offDuration == 0) {
				// We've reached min brightness
				state = LED_OFF;

				pollIntervalMs = EBF_NO_POLLING;
			} else {
				effectStart = micros();
				pollIntervalMs = onDuration;
			}
		} else {
			// Processing was called before needed
			// Set polling to the time left
			pollIntervalMs = onDuration - timePassed / 1000;
		}
		break;

	default:
		rc = 0;
		break;
	}

	return rc;
}