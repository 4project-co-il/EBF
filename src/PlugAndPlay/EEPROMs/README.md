Every PlugAndPlay device have an EEPROM chip on it, that instructs the PlugAndPlay system what it can do and how the communication should work.

We use those programs in production.\
**The average user should not use those programs**, unless instructed to update a device he/she have.

Those programs are also used with generic adapters, such as QWIIC/STEMMA to PlugAndPlay adapter.\
In order to connect QWIIC or STEMMA device to PlugAndPlay system, the device have to be recognized by the EEPROM content and have the matching code in the PlugAndPlay system that can communicate with the device (the appropriate PnP_* class).\
The EEPROM programming code is generally created together with those PnP_* classes.

If you do use those programs to re-program the EEPROM of your PlugAndPlay device, make sure to run it only once.\
Although those EEPROMs can be re-written million times, it is advised to flash any other program on your microcontroller (like the blink example code) right after you finished programming the EEPROM.

PlugAndPlay devices that don't have the microcotrollers on-board, should be connected to the first PnP port on the microcontroller that executes that program, such as the PlugAndPlay SAMD21 Dev board.

As an indication of successful programming of the EEPROM, the status LED of the Dev board will perform a short flash every second.\
Fast flashing (every 100-200mSec) is an indication of a failed EEPROM programming attempt.

