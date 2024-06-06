#include <Arduino.h>
#include "EBF.h"

enum {
	LED_TIMER = 0,

	NUMBER_OF_TIMERS
};

// EBF objects creation, should be global
EBF_Core EBF;

const uint8_t pinCount = 6;
// Array of DigitalOutput objects
EBF_DigitalOutput leds[pinCount];
// Array of pin numbers to which LEDs are attached
// will be used in setup() to initialize the objects
uint8_t ledPins[pinCount] = {2, 7, 4, 6, 5, 3};

int currentPin = 0;
int direction = 1;

// The timer function will turn OFF current LED,
// turn ON next LED and re-start the timer, so when
// it will expire, it will do the same for the next LED
void onTimer()
{
	// Turn OFF current LED
	leds[currentPin].SetValue(LOW);

	// Are we at the end of the array
	if (direction > 0 && currentPin == pinCount-1) {
		direction = -1;
	} else if (direction < 0 && currentPin == 0) {
		direction = 1;
	}

	// Move to the next LED
	currentPin += direction;

	// Turn ON next LED
	leds[currentPin].SetValue(HIGH);

	// Restart the timer
	EBF.StartTimer(LED_TIMER);
}

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init(NUMBER_OF_TIMERS);

	// Initialize the timer for 100 mSec
	EBF.InitTimer(LED_TIMER, onTimer, 1000);

	// Initialize DigitalOutput objects in the loop
	for (int currentLed = 0; currentLed < pinCount; currentLed++) {
		leds[currentLed].Init(ledPins[currentLed]);
	}

	// Turn the first LED ON here, so it will visually start from the first LED
	// The timer first turns it OFF and then turns the next led ON.
	leds[currentPin].SetValue(HIGH);

	// Start the timer to initiate the sequence
	EBF.StartTimer(LED_TIMER);
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
