#include "ticktimer.hpp"

//==============================================================================
//Class Implementation...
//==============================================================================
//CTickTimer
//==============================================================================
/*!-----------------------------------------------------------------------------
Constructor for a periodic polled interval timer object
@param enabled True to create the timer in enabled mode
*/
CTickTimer::CTickTimer(bool enabled)
{
	_enabled = enabled;
	_tickDelta = 0;		//Set "poll" function to always execute
	_tickValid = false;
}

/*!-----------------------------------------------------------------------------
*/
CTickTimer::~CTickTimer()
{
	//Nothing to clear up
}

/*!-----------------------------------------------------------------------------
*/
bool CTickTimer::GetEnabled()
{
	return _enabled;
}

/*!-----------------------------------------------------------------------------
*/
double CTickTimer::GetFrequency()
{
	return CSysTick::GetFrequency() / _tickDelta;
}

/*!-----------------------------------------------------------------------------
*/
double CTickTimer::GetInterval()
{
	//return _tickDelta / CSysTick::GetFrequency();
	return 1.0 / this->GetFrequency();
}

/*!-----------------------------------------------------------------------------
*/
uint32 CTickTimer::GetIntervalMS()
{
	return (uint32)(this->GetInterval() * 1000.0);
}

/*!-----------------------------------------------------------------------------
Function that should be called periodically to poll the timer and see how many
time periods have elapsed since the last poll.
@result The number of complete time periods that have elapsed, or 0 for none
*/
uint32 CTickTimer::Poll()
{
	//If timer is disabled, then return no epochs have elapsed
	if(!_enabled)
		return 0;

	//If tickDelta is 0, then always return an elapsed period
	if(_tickDelta == 0.0)
		return 1;

	//Reset the timer if it isn't valid
	if(!_tickValid)
		this->Reset();

	// Read the number of from the timer
	TTimeTicks ticksNow = CSysTick::GetTicks();

	// Reset the last tick counter if we go back in time
	if(ticksNow < _tickLast) {
		//printf("* %lld, %lld\r\n", ticksNow, _tickLast);
		_tickLast = ticksNow;
	}

	//printf("Ticks Now: %f, %f, %f\r\n", ticksNow, _tickLast, _tickDelta);

	// Compute the difference since the last poll
	TTimeTicks ticksDiff = ticksNow - _tickLast;

	// work out how many complete epochs have passed since the last poll
	uint32 epochs = (int32)(ticksDiff / _tickDelta);

	// Move the last time on by the number of complete epochs passed
	if (epochs > 0) {
		_tickLast += _tickDelta * epochs;
	}

	// Return the number of completed epochs
	return epochs;
}

/*!-----------------------------------------------------------------------------
Function called to reset the timer to the current time when subsequent Poll
functions are called.
*/
void CTickTimer::Reset()
{
	_tickValid = true;
	_tickLast = CSysTick::GetTicks();
}

/*!-----------------------------------------------------------------------------
Function that sets weather the timer is enabled. When disabled the "poll" function
always return 0 epochs.
@param value True to enable the timer
*/
void CTickTimer::SetEnabled(bool value)
{
	_enabled = value;
}

/*!-----------------------------------------------------------------------------
Specified the frequency (approximate within resolution of the tick counter) that
the timer will generate epoch counter with when the Poll method is called.
@param value The frequency in Hertz. Use a value of zero, for "always execute" on poll
*/
void CTickTimer::SetFrequency(double value)
{
	//Compute the number of tick counts per interval
	if(value <= 0.0)
		_tickDelta = 0.0;
	else
		_tickDelta = CSysTick::GetFrequency() / value;
}

/*!-----------------------------------------------------------------------------
Specified the interval (approximate within resolution of the tick counter) that
the timer will generate epoch counter with when the Poll method is called
@param The period in seconds. Use a value of zero, for "always execute" on poll
*/
void CTickTimer::SetInterval(double value)
{
	//Compute the number of tick counts per interval
	/*
	if(value <= 0)
		_tickDelta = 0.0;
	else
		_tickDelta = value * CSysTick::GetFrequency();
	*/
	this->SetFrequency(1.0 / value);
}

/*!-----------------------------------------------------------------------------
*/
void CTickTimer::SetIntervalMS(uint32 value)
{
	//Compute the number of tick counts per interval
	this->SetInterval((double)value / 1000.0);
}

//==============================================================================
