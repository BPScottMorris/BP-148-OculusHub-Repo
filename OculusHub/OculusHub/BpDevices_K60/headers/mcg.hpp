/*==============================================================================
C++ Module that provides the definitions and implementation for a class that
provides basic functionality for configuring the Master Clock Generator (MCG)
module.

Before including this module, the user should implement a #define directive
to specify the type of clock source required (i.e. 8MHz crystal, 50MHz osc etc),
so that MCG module can build and configure itself accordingly.
Only ONE of the following definitions should be made...
#define MCLK_EXT_8MHz_XTAL
#define MCLK_EXT_50MHz_OSC

Definitions are provided for ease of use by other code detailing the clock
frequencies produced by the MCG.

13/09/2013 - Created V1.0 of file, RSh (based on C code by KWe)
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef MCG_HPP
#define MCG_HPP

//Include system libraries
//#include <cmath>

//Include common type definitions and macros
#include "common.h"

//Include the processor platform
#include "processor.h"

//==============================================================================
//Class Definition...
//==============================================================================
enum EMcgOscSrc {
	OSCSRC_EXTREF,
	OSCSRC_OSC,
};

/*! Define a struct used to dynamically configure the MCG */
struct TMcgConfig {
	uint8 OSCCR;
	EMcgOscSrc OscSrc;
	uint8 RANGE;
	uint8 FRDIV;
	uint8 PRDIV;
	uint8 VDIV;
	uint32 ClkSrcFreq;	//In Hz
	uint32 ClkMcgFreq;	//In Hz
	uint8 ClkDivSys;
	uint8 ClkDivPeripheral;
	uint8 ClkDivBus;
	uint8 ClkDivFlash;
};

typedef TMcgConfig* PMcgConfig;

/*!
Define a class of static functions to manipulate the MCG module
*/
class CMcg {
	public:
		//Static Fields
		static TMcgConfig Config;
		//static uint32 ClkSrcFreq;				/*!< The Source clock frequency, in Hz */
		//static uint32 ClkMcgFreq;				/*!< The MCG output clock frequency, in Hz */
		static uint32 ClkSysFreq;				/*!< The System (Processor/Core) clock frequency, in Hz */
		static uint32 ClkIntBusFreq;			/*!< The Internal-Bus/Peripheral clock frequency, in Hz */
		static uint32 ClkFlexBusFreq;			/*!< The Flex-Bus clock frequency, in Hz */
		static uint32 ClkFlashFreq;				/*!< The Flash Memory clock frequency, in Hz */

		//Static Methods
		static void Initialise(PMcgConfig cfg);
		static void WaitMicroseconds(uint16 interval);
		static void WaitMilliseconds(uint16 interval);
};

//==============================================================================
#endif
