/*==============================================================================
Module that provides definitions and implementations for a Service class and
service manager that executes functions in registered enabled services
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef SERVICE_HPP
#define SERVICE_HPP

//Include system libraries

//Include common type definitions and macros
#include "common.h"

//Include the tick timer for determining service intervals
#include "ticktimer.hpp"

//==============================================================================
//Class Definition...
//==============================================================================
//------------------------------------------------------------------------------
/*!
Define a class that implements a service.
Inheriting classes can use these functions, and override DoExecute to
perform the work of the service.
Every time Service is called, the timer is polled and then the overridable DoService
method executed, with the timer flag passed to it.
*/
class CService {
	private:
		bool _enabled;
		bool _started;
		PTickTimer _timer;

	protected:
		virtual void DoDestroy();
		virtual bool DoService(bool timerEvent);
		virtual bool DoServiceStart();
		virtual void DoServiceStop();

	public:
		//Construction and Disposal
		CService();
		virtual ~CService();

		//Methods
		bool GetEnabled();
		double GetServiceInterval();
		bool GetStarted();
		bool Service();
		bool ServiceStart();
		void ServiceStop();
		void SetEnabled(bool value);
		void SetServiceFrequency(float value);
		void SetServiceInterval(double value);
		void SetServiceIntervalMS(uint32 value);
};

/*! Define a pointer to a Service class */
typedef CService* PService;

//==============================================================================
#endif
