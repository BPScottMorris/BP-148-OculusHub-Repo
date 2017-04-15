/*==============================================================================
C++ Module that provides a base hardware clock definition from
which RTC and SYSTICK based clocks can derive, and be used within a common
framework architecture.

20/05/2016 - Created V1.0 of file
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef SYSTICK_HPP
#define SYSTICK_HPP

//Include common type definitions and macros
#include "common.h"

//Include the event handler library
#include "callback.hpp"
#include "datetime.hpp"

//Include the processor platform
#include "processor.h"

//Include the MCG device to get clock timings
#include "mcg.hpp"

//==============================================================================
//Class Definition...
//==============================================================================
class CSysTick {
	public:
		//Static members
		static double				_clkFrequency;
		static volatile TTimeTicks	_clkTicks;

		//Static Methods
		static void Initialise(double frequency = 1000.0);
		static double GetFrequency();
		static double GetSeconds();
		static TTimeTicks GetTicks();
		static void SetSeconds(double seconds);
		static void SetTicks(TTimeTicks ticks);
		static void WaitMilliseconds(float interval);
};

/*! Define a pointer to a Clock class */
typedef CSysTick* PSysTick;

//==============================================================================
//Interrupt Handler Prototypes...
//==============================================================================
//Include prototypes for hardware Interrupt handlers (See Interrupt Vector Table)
#ifdef __cplusplus
extern "C" {
#endif

void ISR_SysTick(void) __attribute__ ((interrupt));

#ifdef __cplusplus
}
#endif

//==============================================================================
#endif
