AutoInc command line usage...

%INC_FILE% is the fully qualified or relative file name to look inside for the value to modify.
%INC_TOKEN% is the string represneting a #DEFINE constant name in a C/C++ file, then find the next integer after it and incriment.

AutoInc.exe /f:"%INC_FILE%" /s:"%INC_TOKEN%"




For example, a C/C++ project may contain a file called "version.h"...


/*=============================================================================
File that contains definitions defining the version information
=============================================================================*/
#ifndef VERSION_H
#define VERSION_H

/*
These definitions are modified externally by the AutoInc.exe command line tool.
The compiler pre-build action script is passed the applicaiton type
set in the compiler project options, and will modify the required define tag
accordingly.
*/

//Define the Main Application firmware version
#define FIRMWARE_BUILD		1	/*!< The autoincrimenting build number */

//=============================================================================
#endif



In the IDE the pre-build command line is written to look something like...
(assuming AutoInc.exe is located in the project root and source code is in an 'src' subfolder

AutoInc.exe /f:"src\version.h" /s:"FIRMWARE_BUILD"