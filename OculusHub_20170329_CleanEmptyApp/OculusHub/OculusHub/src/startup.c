/* GNUC Startup library
 *    Copyright © 2005 Freescale semiConductor Inc. All Rights Reserved.
 *
 * $Date: 2011/09/21 06:41:34 $
 * $Revision: 1.4 $
 */


/*
 *	startup.c	-	entry-point for ARM programs
 *
 */
#include <string.h>
#include <stdlib.h>

#include "cpu.h"

extern void _start(void);

extern char __SP_INIT[];
//extern uint32 __SP_INIT;			//External definition of the Stack-Pointer register

/* To match iar debugger expectation */
void __iar_program_start(void);
void __thumb_startup(void);

//------------------------------------------------------------------------------
extern void __copy_rom_sections_to_ram(void);

/* format of the ROM table info entry ... */
typedef struct {
	unsigned long Source;
	unsigned long Target;
	unsigned long Size;
} TRomInfo;

extern TRomInfo __S_romp[] __attribute__((weak));    /* linker defined symbol */

/*!-----------------------------------------------------------------------------
Routine to copy a single section from ROM to RAM ...
*/
void __copy_rom_section(unsigned long dst, unsigned long src, unsigned long size)
{
	unsigned long len = size;

	const unsigned int size_int = sizeof(int);
	const unsigned int mask_int = sizeof(int)-1;

	const unsigned int size_short = sizeof(short);
	const unsigned int mask_short = sizeof(short)-1;

	const unsigned int size_char = sizeof(char);

	if( dst == src || size == 0)
	{
		return;
	}


	while( len > 0)
	{

		if( !(src & mask_int) && !(dst & mask_int) && len >= size_int)
		{
			*((int *)dst)  = *((int*)src);
			dst += size_int;
			src += size_int;
			len -= size_int;
		}
		else if( !(src & mask_short) && !(dst & mask_short) && len >= size_short)
		{
			*((short *)dst)  = *((short*)src);
			dst += size_short;
			src += size_short;
			len -= size_short;
		}
		else
		{
			*((char *)dst)  = *((char*)src);
			dst += size_char;
			src += size_char;
			len -= size_char;
		}
	}
}

/*!-----------------------------------------------------------------------------
Routine that copies all sections the user marked as ROM into
their target RAM addresses ...
__S_romp is defined in the linker command file
It is a table of RomInfo structures.  The final entry in the table has all-zero
fields.
*/
void __copy_rom_sections_to_ram(void)
{
	int				index;

	if (__S_romp == 0L)
		return;

	// Go through the entire table, copying sections from ROM to RAM.
	for (index = 0;
		 __S_romp[index].Source != 0 ||
		 __S_romp[index].Target != 0 ||
		 __S_romp[index].Size != 0;
		 ++index)
	{
		__copy_rom_section( __S_romp[index].Target,
							__S_romp[index].Source,
							__S_romp[index].Size );

	}
}

/*!-----------------------------------------------------------------------------
*/
void __iar_program_start()
{
	__thumb_startup();
}

/*!-----------------------------------------------------------------------------
*/
__attribute__((naked)) void __thumb_startup(void)
{
	int addr = (int)__SP_INIT;
	//uint32 addr = (uint32)__SP_INIT;

	/*
	Setup the stack before we attempt anything else
	skip stack setup if __SP_INIT is 0
	assume sp is already setup.
	*/
	__asm (
			"mov  r0,%0\n\t"
			"cmp  r0,#0\n\t"
			"beq  skip_sp\n\t"
			"mov  sp,r0\n\t"
			"sub  sp,#4\n\t"
			"mov  r0,#0\n\t"
			"mvn  r0,r0\n\t"
			"str  r0,[sp,#0]\n\t"
			"add  sp,#4\n\t"
			"skip_sp:\n\t"
			::"r"(addr));

    //Initialise the FPU co-processor
	SCB->CPACR |= (0xF << 20); 	//Set bits 20-23 to enable FPU full-access (CP10 and CP11 bits)

	//Set the interrupt vector table position
	SCB->VTOR = (uint32)(&__vect_table);

	//Disable the WDOG module
	WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xC520); /* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
	WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xD928); /* WDOG_UNLOCK: WDOGUNLOCK=0xD928 */
	WDOG->STCTRLH = WDOG_STCTRLH_BYTESEL(0x00) |
				 WDOG_STCTRLH_WAITEN_MASK |
				 WDOG_STCTRLH_STOPEN_MASK |
				 WDOG_STCTRLH_ALLOWUPDATE_MASK |
				 WDOG_STCTRLH_CLKSRC_MASK |
				 0x0100U;
    
    /* SUPPORT_ROM_TO_RAM */
    __copy_rom_sections_to_ram();
    
    //_start is a standard library symbol that calls main() - it should be defined crt0.o
	_start();	

	//We should never get here, so loop indefinitely if we do
	while(1);
}
