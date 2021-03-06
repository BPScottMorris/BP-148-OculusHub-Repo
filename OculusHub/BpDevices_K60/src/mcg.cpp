#include "mcg.hpp"

//==============================================================================
//Class Implementation...
//==============================================================================
//CMcg
//==============================================================================
//Initialise static variables
TMcgConfig CMcg::Config;
//uint32 CMcg::ClkSrcFreq = 0;
//uint32 CMcg::ClkMcgFreq = 0;
uint32 CMcg::ClkSysFreq = 0;
uint32 CMcg::ClkIntBusFreq = 0;
uint32 CMcg::ClkFlexBusFreq = 0;
uint32 CMcg::ClkFlashFreq = 0;

/*!-----------------------------------------------------------------------------
Function that is called to initialise the MCG module
*/
void CMcg::Initialise(PMcgConfig cfg)
{
	//Store the config structure
	CMcg::Config = *cfg;

	// --- MCU powers up in FEI (FLL Engaged Internal) mode ---

	// Select the internal load capacitors for a crystal oscillator capacitance of about 18pF
	OSC0->CR = (uint8)(CMcg::Config.OSCCR);

	// --- Switch FEI to FBE (FLL Bypassed External) mode ---

	// Choose the clock source to the MCG
	if(cfg->OscSrc == OSCSRC_OSC) {
		//External Crystal Oscillator
		MCG->C2 = (uint8)(MCG_C2_EREFS0_MASK | MCG_C2_RANGE0(CMcg::Config.RANGE));
	}
	else {
		//External TTL Oscillator
		MCG->C2 = (uint8)(MCG_C2_RANGE0(CMcg::Config.RANGE));
	}

	// Select the external oscillator and set the FLL reference divider (divide 20MHz by 512 to get 39.0625kHz)
	MCG->C1 = (uint8)(MCG_C1_CLKS(2) | MCG_C1_FRDIV(CMcg::Config.FRDIV));

	// Wait for the oscillator to initialise
	if(cfg->OscSrc == OSCSRC_OSC) {
		while (!(MCG->S & MCG_S_OSCINIT0_MASK));
	}

	// Wait for the reference clock to switch to the external reference
	while (MCG->S & MCG_S_IREFST_MASK);

	// Wait for MCGOUTCLK to switch over to the external reference clock
	while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(2));

	// --- Switch FBE to PBE (PLL Bypassed External) mode ---

	// Setup PLL divisor (documentation says between 8 and 16Mhz > See Page 650 (section 25.5.3))
	MCG->C5 = (uint8)(MCG_C5_PRDIV0(CMcg::Config.PRDIV));

	// Setup PLL multiplier
	MCG->C6 = (uint8)(MCG_C6_CME0_MASK | MCG_C6_PLLS_MASK | MCG_C6_VDIV0(CMcg::Config.VDIV));

	// Wait for the PLLST bit to be set, when PLLCS has obtained a lock
	while (!(MCG->S & MCG_S_PLLST_MASK));

	// Wait for PLL0 to lock
	while (!(MCG->S & MCG_S_LOCK0_MASK));

	// Set up the clock dividers for Sys, Peripheral, Bus and Flash clocks
	SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(CMcg::Config.ClkDivSys - 1) | SIM_CLKDIV1_OUTDIV2(CMcg::Config.ClkDivPeripheral - 1) | SIM_CLKDIV1_OUTDIV3(CMcg::Config.ClkDivBus - 1) | SIM_CLKDIV1_OUTDIV4(CMcg::Config.ClkDivFlash - 1);

	// --- Switch PBE to PEE (PLL Engaged External) mode ---

	// Set the clock source back to PLL
	CLR_BITS(MCG->C1, MCG_C1_CLKS_MASK);

	// Wait for PLL output to be selected
	while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(3));

	/* SIM_SCGC7: MPU=1 */
	SET_BITS(SIM->SCGC7, SIM_SCGC7_MPU_MASK);

	//Reset the memory protection unit (not really clock initialisation)
	MPU->CESR = 0x00;

	//Initialise global MCG variables
	CMcg::ClkSysFreq = CMcg::Config.ClkMcgFreq / CMcg::Config.ClkDivSys;
	CMcg::ClkIntBusFreq = CMcg::Config.ClkMcgFreq / CMcg::Config.ClkDivPeripheral;
	CMcg::ClkFlexBusFreq = CMcg::Config.ClkMcgFreq / CMcg::Config.ClkDivBus;
	CMcg::ClkFlashFreq = CMcg::Config.ClkMcgFreq / CMcg::Config.ClkDivFlash;
}

/*!-----------------------------------------------------------------------------
Function that is called to implement a crude loop-based delay mechanism
that halts program execution for a specified number of microseconds.
THIS IS A VERY CRUDE AND NON-CALIBRATED TIMING FUNCTION. FOR MORE ACURATE
BLOCKING WAITS, SEE THE CRtc CLASS.
@param interval The approximate number of microseconds to wait for
*/
void CMcg::WaitMicroseconds(uint16 interval)
{
	//Loop for the specified number of microseconds
	for(uint16 cnt_us = 0; cnt_us < interval; cnt_us++) {
		//Loop for 1us
		for(uint32 cnt_tick = 0; cnt_tick < (CMcg::ClkSysFreq / 1000000) ; cnt_tick++) {}
	}
}

/*!-----------------------------------------------------------------------------
Function that is called to implement a crude loop-based delay mechanism
that halts program execution for a specified number of milliseconds.
THIS IS A VERY CRUDE AND NON-CALIBRATED TIMING FUNCTION. FOR MORE ACURATE
BLOCKING WAITS, SEE THE CRtc CLASS.
@param interval The approximate number of milliseconds to wait for
*/
void CMcg::WaitMilliseconds(uint16 interval)
{
	//Loop for the specified number of milliseconds
	for(uint16 cnt_ms = 0; cnt_ms < interval; cnt_ms++) {
		//Loop for 1ms
		for(uint32 cnt_tick = 0; cnt_tick < (CMcg::ClkSysFreq / 1000) ; cnt_tick++) {}
	}
}

//==============================================================================
