/*==============================================================================
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef OCULUSHUB_HPP
#define OCULUSHUB_HPP

//==============================================================================
//Library Includes
//==============================================================================
//Include C++ Libraries
#ifdef __cplusplus
using namespace std;	//Allow use of the standard namespace without explicit declaration
#endif

//Include system libraries
#include <math.h>
#include <stdarg.h>		//For variable length arguments in functions
#include <string.h>		//For string manipulation functions

//------------------------------------------------------------------------------
//Include fundamental header files
#include "common.h"
#include "processor.h"
#include "platform.h"
#include "cmd_defs.h"

//Include helper classes
//#include "crc16.hpp"

//Include device drivers
#include "mcg.hpp"
#include "systick.hpp"
#include "com_uart.hpp"
#include "flash.hpp"
#include "flash_data.hpp"
#include "flash_prog.hpp"

//Include device based classes
#include "ticktimer.hpp"

//Include the system level classes
#include "app.hpp"
//#include "cmd_proc.hpp"

//==============================================================================
//Class Definitions...
//==============================================================================
/*! Macro that prints the current system time for debug messages */
#define PRINT_TIME	COM_PRINT("[%7.1fs] ", CSysTick::GetSeconds())

//------------------------------------------------------------------------------
/*!
Class that provides base hardware and operating system objects for the platform,
other application classes are derived from this
*/
class COculusHub : public CApp {
	private:
		//Private Methods
		typedef CApp base;							/*!< Declare access to the parent class */

	protected:
		//Hardware Devices
		PComUart				_comDebug;			/*!< Pointer to the Com Port UART */
		PFlash					_flash;				/*!< Flash memory controller */

		//System Objects
		//PCmdProc				_cmd;				/*!< Class that implements the command processor */
		PFlashProg				_flashProg;			/*!< Class that manages in-system programming of firmware */

		//Variables
		TFlashProgHardwareInfo	_hardware;			/*!< Struct containing hardware information */
		volatile bool			_run;				/*!< True while the application is allowed to run */

		//Protected Methods
		//virtual void CmdExecuteEvent(PCmdProcExecute params);	/*!< Handler that processes received serial commands */
		//void CmdExecute_SysAlive(PCmdProcExecute params);
		//void CmdExecute_SysInfo(PCmdProcExecute params);
		//void CmdExecute_SysReboot(PCmdProcExecute params);
		//void CmdExecute_ProgInit(PCmdProcExecute params);
		//void CmdExecute_ProgBlock(PCmdProcExecute params);
		//void CmdExecute_ProgUpdate(PCmdProcExecute params);
		//void CmdSend_Ready();
		virtual void DoInitialiseGpio();
		virtual void DoReboot() = 0;
		virtual void DoRun() = 0;
		virtual void FlashProgActionEvent(PFlashProgActionParams params);

	public:
		//Construction & Disposal
		COculusHub();
		virtual ~COculusHub();

		//Methods
		void Run();
};

//==============================================================================
#endif
