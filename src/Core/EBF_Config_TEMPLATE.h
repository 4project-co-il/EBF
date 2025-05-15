/*
	This is a template of the configuration file for the EBF - Event Based Framework

	If you're using PlatformIO, you can save the content of that file as "EBF_Config.h" file
	in your project's "include" directory.
	If you're using Arduino IDE, copy that file and save it's content as "EBF_Config.h" file
	in the library "src" directory

	EBF compilation will search the "EBF_Config.h" file to be included, if exist in the
	compilation path.
*/

// Uncomment the EBF_USE_INTERRUPTS definition to enable usage of the interrupts when possible
//#define EBF_USE_INTERRUPTS

// Uncomment the EBF_DIRECT_CALL_FROM_ISR definition if you want to receive the interrupt calls
// directly from the ISR functions (Interrupt Service Routine). You can add that IF statement
// to the code in order to request normal call for that interrupt processing function:
//if (EBF.InInterrupt()) {
//	// Pass the processing back to EBF
//	EBF.PostponeInterrupt(digitalInput);
//	// And return from that function
//	return;
//}
//
//#define EBF_DIRECT_CALL_FROM_ISR

// Uncomment the EBF_SLEEP_IMPLEMENTATION to allow power down modes for supported CPUs
//#define EBF_SLEEP_IMPLEMENTATION

// Uncomment the EBF_REMOVE_SPI_IMPLEMENTATION to to remove the code for SPI communication
// This will free some FLASH and RAM memory if SPI is not used (about 1K of FLASH)
//#define EBF_REMOVE_SPI_IMPLEMENTATION

//////////////////////////////////////
// Plug-n-Play system configuration
//////////////////////////////////////
// Uncomment the PNP_DEBUG_ENUMERATION to print PnP enumeration process to the SerialUSB interface
//#define PNP_DEBUG_ENUMERATION

// Uncomment the PNP_REMOVE_PORT_EXTENDER_IMPLEMENTATION if PnP port extender board is not used
// and you want to free some FLASH and RAM memory
//#define PNP_REMOVE_PORT_EXTENDER_IMPLEMENTATION