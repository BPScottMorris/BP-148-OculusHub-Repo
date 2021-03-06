/*==============================================================================
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef OCULUSHUB_MAIN_HPP
#define OCULUSHUB_MAIN_HPP

//==============================================================================
//Library Includes
//==============================================================================
#include "oculushub.hpp"

//Include helper classes

//Include device drivers

//Include the system level classes

//==============================================================================
//Class Definitions...
//==============================================================================
//------------------------------------------------------------------------------
/*!
Class that provides base hardware and operating system objects for the platform,
other application classes are derived from this
*/
class COculusHubMain : public COculusHub {
	private:
		//Private Methods
		typedef COculusHub base;									/*!< Declare access to the parent class */

	protected:
		//Hardware Devices
		PComUart				_comWifi;							/*!< Pointer to the Com Port for Wifi Control signals */

		PTickTimer				_tmrAlive;							/*!< Timer that flashes the heartbeat LED */

		//Protected Methods
		void DoReboot();
		void DoRun();
		void FlashProgActionEvent(PFlashProgActionParams params);

	public:
		//Construction & Disposal
		COculusHubMain();
		virtual ~COculusHubMain();
};

/*! Define a pointer to an application object */
typedef COculusHubMain* POculusHubMain;

//==============================================================================
#endif
