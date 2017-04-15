#include "service.hpp"

//==============================================================================
//Class Implementation...
//==============================================================================
//CService
//==============================================================================
/*!-----------------------------------------------------------------------------
*/
CService::CService()
{
	//Create the service interval timer
	_timer = new CTickTimer(false);

	//Set the service to enabled
	_enabled = true;

	//Indicate the service isn't started
	_started = false;
}

/*!-----------------------------------------------------------------------------
*/
CService::~CService()
{
	//Stop the service if its still running
	if(_started) {
		this->ServiceStop();
	}

	//Tidy up objects resources
	this->DoDestroy();

	//Tidy up objects
	delete _timer;
}

/*!-----------------------------------------------------------------------------
Function called when the service destructor is called. Classes should
tidy up their resources here and then call the base member.
Having this seperate allows the single destructor to correctly and safely shut
down the service.
*/
void CService::DoDestroy()
{
}

/*!-----------------------------------------------------------------------------
Function that should be overwritten by inheriting classes to execute code
when the service is serviced.
*/
bool CService::DoService(bool timerEvent)
{
	return timerEvent;
}

/*!-----------------------------------------------------------------------------
Function that should be overwritten by inheriting classes to execute code
when the service is started.
@result True if the service is allowed to start, false to prevent it
*/
bool CService::DoServiceStart()
{
	return true;
}

/*!-----------------------------------------------------------------------------
Function that should be overwritten by inheriting classes to execute code
when the service is stopped.
*/
void CService::DoServiceStop()
{
}

/*!-----------------------------------------------------------------------------
Function that gets the enabled state of the service..
*/
bool CService::GetEnabled()
{
	return _enabled;
}

/*!-----------------------------------------------------------------------------
Function that returns the service interval, in seconds
*/
double CService::GetServiceInterval()
{
	return _timer->GetInterval();
}

/*!-----------------------------------------------------------------------------
*/
bool CService::GetStarted()
{
	return _started;
}

/*!-----------------------------------------------------------------------------
Function called to execute the servicing code if required.
@result Returns true is a service occurred, otherwise false.
*/
bool CService::Service()
{
	bool timerEvent = _timer->Poll();
	if(_enabled && _started) {
		return this->DoService(timerEvent);
	}
	else
		return false;
}

/*!-----------------------------------------------------------------------------
Function that sets the enabled state of the service.
Enabled is used to suspend a service without stopping or restarting it (and the
related overloaded function calls - effectively allowing the service the be
suspended or frozen).
*/
void CService::SetEnabled(bool value)
{
	_enabled = value;
}

/*!-----------------------------------------------------------------------------
Function called to set the interval frequency (in Hertz) between successive
Executions of the DoService method.
Use a value of <= 0.0 to specify continuous operation
*/
void CService::SetServiceFrequency(float value)
{
	_timer->SetFrequency(value);
}

/*!-----------------------------------------------------------------------------
Function called to set the interval (in seconds) between successive
Executions of the DoService method.
Use a value of "0" to specify continuous operation.
*/
void CService::SetServiceInterval(double value)
{
	_timer->SetInterval(value);
}

/*!-----------------------------------------------------------------------------
Function called to set the interval (in milliseconds) between successive
Executions of the DoService method.
Use a value of "0" to specify continuous operation.
*/
void CService::SetServiceIntervalMS(uint32 value)
{
	_timer->SetIntervalMS(value);
}

/*!-----------------------------------------------------------------------------
Function called to enable the Service class to call the DoService function at
specific intervals (or continuously) when the "Service" function is called.
@result True if the service was started, otherwise false
*/
bool CService::ServiceStart()
{
	//Abort if the service is already started
	if(_started)
		return false;

	//Call the overridable start method to determine if we can start
	bool allow = this->DoServiceStart();

	if(allow) {
		_started = true;
		//Reset and Enable the service timer
		_timer->Reset();
		_timer->SetEnabled(true);
	}

	return allow;
}

/*!-----------------------------------------------------------------------------
Function that stops the DoService routine from being executed
*/
void CService::ServiceStop()
{
	_started = false;

	//Disable the service timer
	_timer->SetEnabled(false);

	//Call the overridable stop method
	this->DoServiceStop();
}

//==============================================================================
