/*
	This is a template of the configuration file for the EBF - Event Based Framework library

	If you're using PlatformIO, you can save the content of that file as "Project_Config.h" file
	in your project's "include" directory.
	If you're using Arduino IDE, copy that file and save it's content as "Project_Config.h" file
	in the library "src" directory

	EBF compilation will search the "Project_Config.h" file to be included, if exist in the
	compilation path.
*/

// Uncomment the EBF_REMOVE_DEBUG_CODE definition to remove the debug code from the EBF implementation.
// This is highly not advised, unless the code was extensively checked and there is a high confidence
// in your implementation.
// The extra code will help you understand why your code doesn't work even without a dedicated debugger
// and without a need to add printouts. To enable EBF's debug printouts, call EBF.SetErrorHandlerSerial(serial)
// with serial defined as "EBF_Serial serial;"
//#define EBF_REMOVE_DEBUG_CODE

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

// Uncomment the EBF_SLEEP_IMPLEMENTATION to allow power save modes for supported CPUs
//#define EBF_SLEEP_IMPLEMENTATION

// Uncomment the EBF_REMOVE_SPI_IMPLEMENTATION to to remove the code for SPI communication
// This will free some FLASH and RAM memory if SPI is not used (about 1K of FLASH)
//#define EBF_REMOVE_SPI_IMPLEMENTATION
