#include "systick.hpp"

//==============================================================================
//Implementation...
//==============================================================================
//Initialise static members
double CSysTick::_clkFrequency = 0.0;
volatile TTimeTicks CSysTick::_clkTicks = 0.0;

/*!-----------------------------------------------------------------------------
*/
void CSysTick::Initialise(double frequency)
{
	//Stop the SysTick timer
	CLR_BITS(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);

	//Clear the overall timer counter
	CSysTick::SetTicks(0);

	//Compute the load interval
	uint32 load = (uint32)(((double)CMcg::ClkSysFreq / frequency) - 1);
	SysTick->LOAD = load;

	//Compute the actual frequency, based on the integer nature of the clock
	CSysTick::_clkFrequency = (double)CMcg::ClkSysFreq / (double)(load + 1);

	//Start the SysTick timer with the new load value, and allow interrupts
	SET_BITS(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk);
}

/*!-----------------------------------------------------------------------------
Returns the actual tick frequency of the clock
*/
double CSysTick::GetFrequency()
{
	return CSysTick::_clkFrequency;
}

/*!-----------------------------------------------------------------------------
Returns the value of the clock, in seconds
*/
double CSysTick::GetSeconds()
{
	//Get the current time
	TTimeTicks ticks = CSysTick::GetTicks();
	//Convert to seconds by dividing by the actual frequency
	return ((double)ticks / CSysTick::_clkFrequency);
}

/*!-----------------------------------------------------------------------------
Returns the value of the clock in ticks (epochs of the SysTick timer)
*/
TTimeTicks CSysTick::GetTicks()
{
	TTimeTicks ticks;
	IRQ_DISABLE;
	ticks = CSysTick::_clkTicks;
	IRQ_ENABLE;
	return ticks;
}

/*!-----------------------------------------------------------------------------
*/
void CSysTick::SetSeconds(double seconds)
{
	//Convert the seconds into ticks
	TTimeTicks ticks = (TTimeTicks)(seconds * CSysTick::_clkFrequency);
	//Load the clock
	CSysTick::SetTicks(ticks);
}

/*!-----------------------------------------------------------------------------
Function the sets the value of the clock in ticks (epochs of the SysTick timer)
*/
void CSysTick::SetTicks(TTimeTicks ticks)
{
	IRQ_DISABLE;
	CSysTick::_clkTicks = ticks;
	IRQ_ENABLE;
}

/*!-----------------------------------------------------------------------------
Function called to wait a set number of milliseconds, based on the SysTick timer
*/
void CSysTick::WaitMilliseconds(float interval)
{
	if(interval > 0) {
		//Get the number of ticks at the starting time
		TTimeTicks start = CSysTick::GetTicks();

		//Compute the number of ticks per millisecond
		float ticksPerMsec = (float)(CSysTick::_clkFrequency / 1000.0);

		//Compute the ticks ending time
		float delta = interval * ticksPerMsec;
		TTimeTicks end = start + (TTimeTicks)delta;

		//Loop until we reach the ending time
		while(CSysTick::GetTicks() < end) {
			//Allow some time between polling the ISR value
			NOP;
			NOP;
			NOP;
			NOP;
		}
	}
}

//==============================================================================
//Interrupt Handler
//==============================================================================
#ifdef __cplusplus
extern "C" {
#endif

/*!-----------------------------------------------------------------------------
Handler for the SysTick interrupt
*/
void ISR_SysTick(void)
{
	//Clear interrupt by reading the Control/Status Register
	volatile uint32 dummy = SysTick->CTRL;

	//Increment the tick count
	CSysTick::_clkTicks++;
}

#ifdef __cplusplus
}
#endif

//==============================================================================
