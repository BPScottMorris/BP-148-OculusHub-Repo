/*==============================================================================
 * Base class for deriving application objects from
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef APP_HPP
#define APP_HPP

//==============================================================================
//Library Includes
//==============================================================================
//Include C++ Libraries
#ifdef __cplusplus
	using namespace std;	//Allow use of the standard namespace without explicit declaration
#endif

//Include system libraries
//#include <math.h>
//#include <stdarg.h>		//For variable length arguments in functions
//#include <string.h>		//For string manipulation functions

//------------------------------------------------------------------------------
//Include fundamental header files
#include "common.h"

//==============================================================================
//Class Definitions...
//==============================================================================
//------------------------------------------------------------------------------
class CApp;	//Predeclare the class

/*! Declare a pointer to an application class */
typedef CApp* PApp;

/*!
Base class from which applications inherit
*/
class CApp {
	public:
		//Abstract (Pure Virtual) Methods
		virtual void Run() = 0;

		//Static variables
		static PApp Application;
};

//==============================================================================
#endif
