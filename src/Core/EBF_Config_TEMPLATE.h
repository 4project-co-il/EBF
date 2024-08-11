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

// Uncomment the EBF_SLEEP_IMPLEMENTATION to allow power down modes for supported CPUs
//#define EBF_SLEEP_IMPLEMENTATION

// Plug-n-Play system configuration
// Uncomment the relevant line to the used PnP HUB board (only 1 line should be uncommented from the list)
//#define PNP_USE_SAMD21_MINI_DIRECT_SERCOM2_INT10
//#define PNP_USE_SAMD21_MINI_HUB
