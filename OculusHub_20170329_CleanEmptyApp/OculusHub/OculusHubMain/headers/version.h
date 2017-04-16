/*=============================================================================
File that contains definitions defining the version information
=============================================================================*/
#ifndef VERSION_H
#define VERSION_H

/*
These definitions are modified externally by the AutoInc.exe command line tool.
The compiler pre-build action script is passed the application type
set in the compiler project options, and will modify the required define tag
accordingly.
*/

#define FIRMWARE_PARTNUMBER		1227					/*!< Identification code for the firmware */
#define FIRMWARE_SECTION		FLASH_SECTION_MAIN		/*!< The Flash section the firmware programmer should put this into - 0=BOOT, 1=MAIN */

//Define the Main Application firmware version
#define FIRMWARE_VERSION_MAJOR	1
#define FIRMWARE_VERSION_MINOR	0
#define FIRMWARE_VERSION_BUILD	33    /*!< The auto-incrementing build number */

//=============================================================================
#endif
