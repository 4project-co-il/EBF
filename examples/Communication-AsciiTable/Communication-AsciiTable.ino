#include <Arduino.h>
#include "EBF.h"

// EBF objects creation, should be global
EBF_Core EBF;

// EBF serial object will use the Arduino's Serial (Hardware Serial) for communication by default
EBF_Serial serial;
// You can use any other Serial interface if availalbe for your device
//EBF_Serial serial(Serial2);

// first visible ASCIIcharacter '!' is number 33:
uint8_t thisByte = 33;
// you can also write ASCII characters in single quotes.
// for example, '!' is the same as 33, so you could also use this:
// int thisByte = '!';

// This example is exactly the same as Arduino's ASCII table example:
// https://docs.arduino.cc/built-in-examples/communication/ASCIITable/
//
// The only difference is that the EBF_Serial object is used.
// If you use the Serial for printouts only, you don't really need the EBF instance.
// It might still be useful if you'd like to switch the hardware serial port (for boards)
// that have more than one, or a SoftwareSerial implementation, without changing all the
// function calls in the code. All you will have to do is changing the declaration to:
// EBF_Serial serial(Serial2);
// Or other Serial instance

void setup()
{
	// EBF is the first thing that should be initialized
	EBF.Init();

	// Default Init is enough for printouts via Serial on 115200 boud speed
	serial.Init();

	while (!serial) {
    	;  // wait for serial port to connect. Needed for native USB port only
  	}

	// prints title with ending line break
	serial.println("ASCII Table ~ Character Map");

	// print up to last visible character '~' or 126
	while (thisByte <= 126) {
		// prints value unaltered, i.e. the raw binary version of the byte.
		// The Serial Monitor interprets all bytes as ASCII, so 33, the first number,
		// will show up as '!'
		serial.write(thisByte);

		serial.print(", dec: ");
		// prints value as string as an ASCII-encoded decimal (base 10).
		// Decimal is the default format for Serial.print() and Serial.println(),
		// so no modifier is needed:
		serial.print(thisByte);
		// But you can declare the modifier for decimal if you want to.
		// this also works if you uncomment it:

		// Serial.print(thisByte, DEC);


		serial.print(", hex: ");
		// prints value as string in hexadecimal (base 16):
		serial.print(thisByte, HEX);

		serial.print(", oct: ");
		// prints value as string in octal (base 8);
		serial.print(thisByte, OCT);

		serial.print(", bin: ");
		// prints value as string in binary (base 2) also prints ending line break:
		serial.println(thisByte, BIN);

		// go on to the next character
		thisByte++;
	}
}

void loop()
{
	// Let EBF to do all the processing
	// Your logic should be done in the callback functions
	EBF.Process();
}
