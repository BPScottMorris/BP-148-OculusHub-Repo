/*==============================================================================

==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef TICKTIMER_HPP
#define TICKTIMER_HPP

//Include common type definitions and macros
#include "common.h"
#include "datetime.hpp"

//Include the processor platform
#include "processor.h"

#include "systick.hpp"

//==============================================================================
//Class Definition...
//==============================================================================
/*!
Define a class that implements a polled period timer based on the Ticks clock
*/
class CTickTimer {
	protected :
		bool _enabled;
		double _tickDelta;
		TTimeTicks _tickLast;
		bool _tickValid;

	public :
		//Construction & Disposal
		CTickTimer(bool enabled = false);
		virtual ~CTickTimer();

		//Methods
		bool GetEnabled();
		double GetFrequency();
		double GetInterval();
		uint32 GetIntervalMS();
		uint32 Poll();
		void Reset();
		void SetEnabled(bool value);
		void SetFrequency(double value);
		void SetInterval(double value);
		void SetIntervalMS(uint32 value);
};

/*!
Declare a pointer to a tick timer
*/
typedef CTickTimer* PTickTimer;

//==============================================================================
#endif
