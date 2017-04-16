/*==============================================================================
SRS Fusion - Thruster Board Main Application
==============================================================================*/
//------------------------------------------------------------------------------
//Include common type definitions and macros
#include "common.h"
#include "processor.h"
#include "platform.h"

//Include the application object
#include "oculushub_main.hpp"

//==============================================================================
//Definitions...
//==============================================================================
//See Common.hpp for hardware io definition macros.

//==============================================================================
//Interrupt Handler Prototypes...#
//==============================================================================
#ifdef __cplusplus
extern "C" {
#endif

void ISR_NMI()						__attribute__ ((interrupt));
void ISR_Hard_Fault()				__attribute__ ((interrupt));
void ISR_Mem_Manage_Fault()			__attribute__ ((interrupt));
void ISR_Bus_Fault()				__attribute__ ((interrupt));
void ISR_Usage_Fault()				__attribute__ ((interrupt));

#ifdef __cplusplus
}
#endif

//==============================================================================
// Application Entry Point...
//==============================================================================
/*!-----------------------------------------------------------------------------
Main program function.
*/
int main(void)
{
	//###Hello World
	//Create the appropriate application class object and initialise
	CApp::Application = (PApp)(new COculusHubMain());

	//Run the program
	CApp::Application->Run();

	//This should never be reached
	return false;
}

//==============================================================================
//Interrupt Handlers...
//==============================================================================
#ifdef __cplusplus
extern "C" {
#endif

/*!-----------------------------------------------------------------------------
Function called when a Non-Maskable interrupt occurs
*/
void ISR_NMI()
{
	HALT;
	REBOOT;
}

/*!-----------------------------------------------------------------------------
Function called when a Hard Fault interrupt occurs
*/
void ISR_Hard_Fault()
{
	HALT;
	REBOOT;
}

/*!-----------------------------------------------------------------------------
Function called when a Menory Manager Fault interrupt occurs
*/
void ISR_Mem_Manage_Fault()
{
	HALT;
	REBOOT;
}

/*!-----------------------------------------------------------------------------
Function called when a Bus Fault interrupt occurs
*/
void ISR_Bus_Fault()
{
	HALT;
	REBOOT;
}

/*!-----------------------------------------------------------------------------
Function called when a Usage Fault interrupt occurs
*/
void ISR_Usage_Fault()
{
	HALT;
	REBOOT;
}


#ifdef __cplusplus
}
#endif

//==============================================================================
