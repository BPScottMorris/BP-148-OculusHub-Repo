/*==============================================================================
File that defines commonly used Macros
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef MACROS_H
#define MACROS_H

//Include the common type definitions
#include "types.h"

//==============================================================================
//GCC Compiler Message Macros
//==============================================================================
/*! Directive that issues a parenthesised #pragma directive */
#define PRAGMA(x)				_Pragma (#x)

/*! Define a macro to output a compiler info message */
#define PRAGMA_INFO(x)			PRAGMA(message #x)

#define PRAGMA_WARNING(x)		PRAGMA(GCC warning #x)

#define PRAGMA_ERROR(x)			PRAGMA(GCC error #x)

//==============================================================================
//GCC Function Attribute Macros
//==============================================================================
//Interrupt definition template
#define FUNC_ISR				void __attribute__ ((interrupt))

//Weakly defined function template
#define FUNC_WEAK				void __attribute__ ((weak))

//Function that should be placed and executed in RAM (and accessed with a Long-Call if it is far away from the calling statement)
//(see http://stackoverflow.com/questions/15137214/how-to-run-code-from-ram-on-arm-architecture
//and https://ez.analog.com/thread/10607)
//You may get an compiler warning that can be safely ignored: "Warning: ignoring changed section attributes for .data"
//here linker will place function in ram, but ideally linker requires seperatly defined ram function setion, but this is tedious.
#define FUNC_RAM				void __attribute__ (( long_call, section(".data")))

#define FUNC_RAM_OPTIMIZE_OFF	void __attribute__ (( long_call, section(".data"), optimize("O0")))

/*! Macro that disables optimisation for a specific function - usage "FUNC_OPTIMIZE_OFF foo(void) {}" */
#define FUNC_OPTIMIZE_OFF		void __attribute__((optimize("O0")))

//==============================================================================
//Debug Optimisation Macros
//==============================================================================
#ifdef DEBUG
	/*! Macro that sets optimisation off for all following functions in the file until DEBUG_GLOBAL_OPTIMIZE_RESTORE is issued */
	#define DEBUG_GLOBAL_OPTIMIZE_OFF	\
		PRAGMA(GCC push_options) \
		PRAGMA(GCC optimize("O0"))

	#define DEBUG_GLOBAL_OPTIMIZE_RESTORE \
		PRAGMA(GCC pop_options)

	/*! Macro that disables optimisation for a specific function - usage "void DEBUG_FUNC_OPTIMIZE_OFF foo(void) {}" */
	#define DEBUG_FUNC_OPTIMIZE_OFF		__attribute__((optimize("O0")))

#else
	//#define DEBUG_NO_OPTIMIZE
	#define DEBUG_GLOBAL_OPTIMIZE_OFF
	#define DEBUG_GLOBAL_OPTIMIZE_RESTORE
	#define DEBUG_FUNC_OPTIMIZE_OFF
#endif

//==============================================================================
//Define Bit Manipulation & Testing Macros
//==============================================================================
#define BIT(b)						(1u << ((uint8)b))

#define SET_BITS(r, bmsk)			(r |= (bmsk))

#define CLR_BITS(r, bmsk)			(r &= ~(bmsk))

#define INIT_BITS(r, msk, value)	if(value) { SET_BITS(r, msk); } else { CLR_BITS(r, msk); }

#define LOAD_BITS(r, msk, load)		CLR_BITS(r, msk); SET_BITS(r, (load & msk))

#define SET_BIT(r, b)				SET_BITS(r, BIT(b))

#define CLR_BIT(r, b)				CLR_BITS(r, BIT(b))

#define INIT_BIT(r, b, value)		if(value) { SET_BIT(r, b); } else { CLR_BIT(r, b); }

#define IS_BITS_SET(r, bmsk)		(r & (bmsk))

#define IS_BITS_CLR(r, bmsk)		(!IS_BITS_SET(r, bmsk))

#define IS_BIT_SET(r, b)			(r & BIT(b))

#define IS_BIT_CLR(r, b)			(!IS_BIT_SET(r,b))

//==============================================================================
//Assembly & System Macros
//==============================================================================
//Interrupt control macros
#define CLI							__asm("CPSID i")		/*!< Disable global interrupts */

#define SEI							__asm("CPSIE i")		/*!< Enable global interrupts */

#define NOP							__asm("NOP")			/*!< Perform No-Operation*/

//#define HALT						__asm("BKPT #0")		/*!< Enter Debug state - only available on ARM CM4 cores */
#define HALT						__asm("BKPT 255")		/*!< Enter Debug state - only available on ARM CM4 cores */

/*! Macro that performs a software reset of the Cortex-M processor */
#define REBOOT \
{ \
	SCB->AIRCR = (0x05FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk; \
	while(true) { } \
}

//#define LITTLE_ENDIAN

//#define ASSERT(expr)

//==============================================================================
//Interrupt Management Macros
//==============================================================================
//Macros that provide a method to lock and unlock interrupt requests.
//A counter ensures that the required number of enable calls must be made
//for disabled calls...
//(Value actually defined in macros.cpp)
extern volatile uint8 g_irqLockCnt;
//extern volatile uint8 g_irqEnable;

/*! Macro to initialise the interrupt system, and ensure interrupts are off */
#define IRQ_INIT \
{ \
	CLI; \
	g_irqLockCnt = 0; \
}

/*! Macro that temporarily disables interrupts until IRQ_ENABLE is called for each IRQ_DISABLE call */
#define	IRQ_DISABLE \
{ \
	CLI; \
	g_irqLockCnt++; \
}

/*! Macro the re-enables interrupts after each IRQ_DISABLE call, once IRQ_START has been executed */
#define IRQ_ENABLE \
{ \
	CLI; \
	if(g_irqLockCnt) { g_irqLockCnt--; } \
	if(!g_irqLockCnt) { SEI; } \
}

//==============================================================================
//ANSI Escape Code "Control Sequence Identifiers"
//see http://en.wikipedia.org/wiki/ANSI_escape_code
//For use in PrintF statements as %s parameters
//==============================================================================
#define CSI_ERASE_LINE		"\x1b[2K"		/*!< Erases the entire contents of the cursors line, does not move cursor */
#define CSI_RESET_LINE		"\x1b[2K\r"		/*!< Erases the entire contents of the cursors line, resets cursor to start */

#define CSI_NORMAL			"\x1b[30;0m"	/*!< Reset text parameters */
#define CSI_TEXT_BLACK		"\x1b[30m"		/*!< Text colour Black ANSI escape sequence */
#define CSI_TEXT_MAROON		"\x1b[31m"		/*!< Text colour Dark Red ANSI escape sequence */
#define CSI_TEXT_FOREST		"\x1b[32m"		/*!< Text colour Dark Green ANSI escape sequence */
#define CSI_TEXT_OLIVE		"\x1b[33m"		/*!< Text colour Olive ANSI escape sequence */
#define CSI_TEXT_NAVY		"\x1b[34m"		/*!< Text colour Navy ANSI escape sequence */
#define CSI_TEXT_PURPLE		"\x1b[35m"		/*!< Text colour Purple ANSI escape sequence */
#define CSI_TEXT_TEAL		"\x1b[36m"		/*!< Text colour Teal ANSI escape sequence */
#define CSI_TEXT_SILVER		"\x1b[37m"		/*!< Text colour Silver ANSI escape sequence */

#define CSI_TEXT_GRAY		"\x1b[30;1m"	/*!< Text colour Gray ANSI escape sequence */
#define CSI_TEXT_RED		"\x1b[31;1m"	/*!< Text colour Red ANSI escape sequence */
#define CSI_TEXT_GREEN		"\x1b[32;1m"	/*!< Text colour Green ANSI escape sequence */
#define CSI_TEXT_YELLOW		"\x1b[33;1m"	/*!< Text colour Yellow ANSI escape sequence */
#define CSI_TEXT_BLUE		"\x1b[34;1m"	/*!< Text colour Blue ANSI escape sequence */
#define CSI_TEXT_MAGENTA	"\x1b[35;1m"	/*!< Text colour Magenta ANSI escape sequence */
#define CSI_TEXT_CYAN		"\x1b[36;1m"	/*!< Text colour Cyan ANSI escape sequence */
#define CSI_TEXT_WHITE		"\x1b[37;1m"	/*!< Text colour White ANSI escape sequence */

//==============================================================================
//Compiler specific Macros
//==============================================================================
/*!
Macro that counts the number of zeros at the start of a word.
This macro uses compiler-specific constructs to perform an inline insertion of the "clz"
instruction, which counts the leading zeros directly.
*/

#ifdef __GNUC__
	//Macros for GNU C Compiler

	/*! Macro the counts the number of leading zero's at the start of a word */
	#define CNT_LEADING_ZEROES(x)		__builtin_clz(x)

#else
	//#define CNT_LEADING_ZEROES(x)		__CLZ(x)

	PRAGMA_ERROR("Cannot compile Compiler specific marcos")
#endif

//==============================================================================
#endif
