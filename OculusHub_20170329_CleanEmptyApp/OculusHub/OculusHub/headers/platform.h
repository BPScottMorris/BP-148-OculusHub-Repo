/*==============================================================================
The "Platform.h" file specifies all the common definitions and macros
that are available across all objects within the class heirachy, this may
include hardware definitions required by low-level modules to higher level
objects.
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef PLATFORM_H
#define PLATFORM_H

//Include common type definitions and macros
#include "common.h"

//CMSIS related definitions
//#include "processor.h"

//------------------------------------------------------------------------------
//Firmware & Hardware Information (& PCB Revision)
//------------------------------------------------------------------------------
//Define available flash programmer sections
#define FLASH_SECTION_BOOT				0
#define FLASH_SECTION_MAIN				1

//Define hardware part numbers for different products
#define HARDWARE_PARTNUMBER				1208			/*!< Part number for the product */
#define HARDWARE_PARTREV				1				/*!< Part revision for the product */

#define FIRMWARE_DATE					__DATE__		/*!< String containing software build date */

//Include the platform version definitions, separated to another file,
//so a command line auto-inc tool can easily parse and modify without making
//changes to this file.
#include "version.h"

//------------------------------------------------------------------------------
#if defined(DEBUG)
	PRAGMA_INFO("Compiling DEBUG firmware")
#else
	PRAGMA_INFO("Compiling RELEASE firmware")
#endif

#if (HARDWARE_PARTREV >= 1)
	PRAGMA_INFO("Compiling for REV 1 platform (v1 PCB with 50MHz external oscillator and 120MHz MCU")
#else
	PRAGMA_ERROR("Unsupported Board Revision")
#endif

//------------------------------------------------------------------------------
//Flash Memory Configuration
//------------------------------------------------------------------------------
/*! Size of Flash area - 512kb size */
#define FLASH_SIZE						0x00100000			/*!< Flash 1Mb in size */

/*! Program Flash sector size - 4kB size */
#define FLASH_SECTOR_SIZE				0x00001000			/*!< 4kb Sectors - minimum erasable unit */

/*! Program Flash block size, 256kB */
#define FLASH_BLOCK_SIZE				0x00040000			/*!< Flash built of 4 x 256kb blocks */

#define FLASH_BOOT_START				0x00000000			/*!< Boot loader starting address */
#define FLASH_BOOT_SIZE					0x00010000			/*!< Boot loader size - 64kb */

#define FLASH_MAIN_START				(FLASH_BOOT_SIZE)	/*!< Main application starting address, 64kb */
#define FLASH_MAIN_SIZE					0x00070000			/*!< Main application size, 448kb */

#define FLASH_SCRATCH_START				0x00080000			/*!< Temporary programming area starting address */
#define FLASH_SCRATCH_SIZE				(FLASH_MAIN_SIZE)	/*!< Temporary programming area size - same as main application */

#define FLASH_SETTINGS_START			0x000F0000			/*!< Settings data - 56kb, 14 x 4kb sectors of memory */
#define FLASH_SETTINGS_SIZE				0x0000F000

#define FLASH_PROGINFO_START			0x000FF000			/*!< Program Identification data - 1 x 4kb sector of memory */
#define FLASH_PROGINFO_SIZE				0x00001000

#define FLASH_HASH_KEY					"u86TzXFTDci1I0sW"	/*!< Defines a string used as part of the hashing process to sign firmware, for decryption and user access - must be 16 chars long min*/

#define FLASH_PROG_BLOCK_MAX			128					/*! Maximum number of bytes in a flash programming block */

//------------------------------------------------------------------------------
//Define the bit values of the Hardware Flags field

//------------------------------------------------------------------------------
//Settings Default Values
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//Clock & Oscillator Configuration
//------------------------------------------------------------------------------
#define SYSTICK_TIMER_FREQ				10000.0				/*!< Define the frequency of the SysTick Timer overflow timebase */

//------------------------------------------------------------------------------
//UART Configuration
//------------------------------------------------------------------------------
//Route GetChar/PutChar PrintF requests through the CUart handler
#define COM_TERMINAL					true

//Define the number of supported UART peripherals
#define UART_PERIPHERALS	 			6

//Enable UART IRQ's...
#define UART0_CONNECT_IRQ				true			/*!< For WIFI data transmission - not really used (COM1) */
#define UART1_CONNECT_IRQ				true			/*!< For WIFI primary control (COM2) */
//#define UART2_CONNECT_IRQ				true
#define UART3_CONNECT_IRQ				true			/*!< For AUX header for debugging */
//#define UART4_CONNECT_IRQ				true
//#define UART5_CONNECT_IRQ				true

//Define the setup each logical uart object uses...
#define UART_DEBUG						3				/*! Map to UART3 */
#define UART_DEBUG_TX_BUFFER			512
#define UART_DEBUG_RX_BUFFER			128
#define UART_DEBUG_BAUD					BAUD_115200

#define UART_WIFICTRL					1				/*! Map to UART1 */
#define UART_WIFICTRL_TX_BUFFER			256
#define UART_WIFICTRL_RX_BUFFER			64
#define UART_WIFICTRL_BAUD				BAUD_57600

//#define UART_WIFI_DATA					0				/*! Map to UART0 */
//#define UART_WIFIDATA_TX_BUFFER			256
//#define UART_WIFIDATA_RX_BUFFER			64

//------------------------------------------------------------------------------
//Macros for controlling IO signals
//------------------------------------------------------------------------------
#if (HARDWARE_PARTREV >= 1)
	//Power Control & Status Signals
	#define PWR_LED_RED_ON					PTE->PCOR = BIT(10)
	#define PWR_LED_RED_OFF					PTE->PSOR = BIT(10)
	#define PWR_LED_RED(state)				{ if(state) { PWR_LED_RED_ON; } else { PWR_LED_RED_OFF; } }

	#define PWR_LED_GREEN_ON				PTE->PCOR = BIT(11)
	#define PWR_LED_GREEN_OFF				PTE->PSOR = BIT(11)
	#define PWR_LED_GREEN(state)			{ if(state) { PWR_LED_GREEN_ON; } else { PWR_LED_GREEN_OFF; } }

	#define PWR_LED_BLUE_ON					PTE->PCOR = BIT(12)
	#define PWR_LED_BLUE_OFF				PTE->PSOR = BIT(12)
	#define PWR_LED_BLUE(state)				{ if(state) { PWR_LED_BLUE_ON; } else { PWR_LED_BLUE_OFF; } }

	//Network Switch Chip Control & Status Signals
	#define LANSW_RESET_ON					NOP		//### Cannot be used on Rev1 PCB as connected to ADC only input
	#define LANSW_RESET_OFF					NOP		//### Cannot be used on Rev1 PCB as connected to ADC only input
	#define LANSW_RESET(state)				{ if(state) { LANSW_RESET_ON; } else { LANSW_RESET_OFF; } }

	#define LANSW_POWERDOWN_ON				PTC->PCOR = BIT(19)
	#define LANSW_POWERDOWN_OFF				PTC->PSOR = BIT(19)
	#define LANSW_POWERDOWN(state)			{ if(state) { LANSW_POWERDOWN_ON; } else { LANSW_POWERDOWN_OFF; } }

	#define LANSW_INT_ACTIVE				IS_BIT_CLR(PTC->DIR, 17)

	//DSL Module Control & Status Signals
	#define DSL_POWER_ACTIVE				IS_BIT_SET(PTA->PDIR, 27)	/*!< Signal is pulsed high by the DSL module when active */
	#define DSL_ETHERNET_ACTIVE				IS_BIT_SET(PTA->PDIR, 28)	/*!< Signal is pulsed high by the DSL module when active */
	#define DSL_LINK_ACTIVE					IS_BIT_SET(PTA->PDIR, 29)	/*!< Signal is pulsed high by the DSL module when active */

	#define DSL_LED_RED_ON					PTA->PCOR = BIT(19)
	#define DSL_LED_RED_OFF					PTA->PSOR = BIT(19)
	#define DSL_LED_RED(state)				{ if(state) { DSL_LED_RED_ON; } else { DSL_LED_RED_OFF; } }

	#define DSL_LED_GREEN_ON				PTB->PCOR = BIT(0)
	#define DSL_LED_GREEN_OFF				PTB->PSOR = BIT(0)
	#define DSL_LED_GREEN(state)			{ if(state) { DSL_LED_GREEN_ON; } else { DSL_LED_GREEN_OFF; } }

	#define DSL_LED_BLUE_ON					PTB->PCOR = BIT(1)
	#define DSL_LED_BLUE_OFF				PTB->PSOR = BIT(1)
	#define DSL_LED_BLUE(state)				{ if(state) { DSL_LED_BLUE_ON; } else { DSL_LED_BLUE_OFF; } }

	//WIFI Module Control & Status Signals
	#define WIFI_LED_RED_ON					PTE->PCOR = BIT(4)
	#define WIFI_LED_RED_OFF				PTE->PSOR = BIT(4)
	#define WIFI_LED_RED(state)				{ if(state) { WIFI_LED_RED_ON; } else { WIFI_LED_RED_OFF; } }

	#define WIFI_LED_GREEN_ON				PTE->PCOR = BIT(5)
	#define WIFI_LED_GREEN_OFF				PTE->PSOR = BIT(5)
	#define WIFI_LED_GREEN(state)			{ if(state) { WIFI_LED_GREEN_ON; } else { WIFI_LED_GREEN_OFF; } }

	#define WIFI_LED_BLUE_ON				PTE->PCOR = BIT(6)
	#define WIFI_LED_BLUE_OFF				PTE->PSOR = BIT(6)
	#define WIFI_LED_BLUE(state)			{ if(state) { WIFI_LED_BLUE_ON; } else { WIFI_LED_BLUE_OFF; } }

	#define WIFI_RELOAD_ON					PTD->PCOR = BIT(15)
	#define WIFI_RELOAD_OFF					PTD->PSOR = BIT(15)
	#define WIFI_RELOAD(state)				{ if(state) { WIFI_RELOAD_ON; } else { WIFI_RELOAD_OFF; } }

	#define WIFI_RESET_ON					PTD->PCOR = BIT(13)
	#define WIFI_RESET_OFF					PTD->PSOR = BIT(13)
	#define WIFI_RESET(state)				{ if(state) { WIFI_RESET_ON; } else { WIFI_RESET_OFF; } }

	#define WIFI_LINK_ACTIVE				IS_BIT_CLR(PTD->PDIR, 12)

	#define WIFI_READY_ACTIVE				IS_BIT_CLR(PTD->PDIR, 14)

	//WIFI_GPIO0
	//WIFI_GPIO1

	//Network Port (to PC) Connection Signals
	#define NET_LED_RED_ON					PTE->PCOR = BIT(7)
	#define NET_LED_RED_OFF					PTE->PSOR = BIT(7)
	#define NET_LED_RED(state)				{ if(state) { NET_LED_RED_ON; } else { NET_LED_RED_OFF; } }

	#define NET_LED_GREEN_ON				PTE->PCOR = BIT(8)
	#define NET_LED_GREEN_OFF				PTE->PSOR = BIT(8)
	#define NET_LED_GREEN(state)			{ if(state) { NET_LED_GREEN_ON; } else { NET_LED_GREEN_OFF; } }

	#define NET_LED_BLUE_ON					PTE->PCOR = BIT(9)
	#define NET_LED_BLUE_OFF				PTE->PSOR = BIT(9)
	#define NET_LED_BLUE(state)				{ if(state) { NET_LED_BLUE_ON; } else { NET_LED_BLUE_OFF; } }

	//Sonar Port Connection Signals
	#define SON_LED_RED_ON					PTA->PCOR = BIT(24)
	#define SON_LED_RED_OFF					PTA->PSOR = BIT(24)
	#define SON_LED_RED(state)				{ if(state) { SON_LED_RED_ON; } else { SON_LED_RED_OFF; } }

	#define SON_LED_GREEN_ON				PTA->PCOR = BIT(25)
	#define SON_LED_GREEN_OFF				PTA->PSOR = BIT(25)
	#define SON_LED_GREEN(state)			{ if(state) { SON_LED_GREEN_ON; } else { SON_LED_GREEN_OFF; } }

	#define SON_LED_BLUE_ON					PTA->PCOR = BIT(26)
	#define SON_LED_BLUE_OFF				PTA->PSOR = BIT(26)
	#define SON_LED_BLUE(state)				{ if(state) { SON_LED_BLUE_ON; } else { SON_LED_BLUE_OFF; } }

	//MCU Signals
	#define MCU_SW1_ACTIVE					IS_BIT_CLR(PTD->PDIR, 8)
	#define MCU_SW2_ACTIVE					IS_BIT_CLR(PTD->PDIR, 9)
	#define MCU_SW3_ACTIVE					IS_BIT_CLR(PTD->PDIR, 10)
	#define MCU_SW4_ACTIVE					IS_BIT_CLR(PTD->PDIR, 11)

	#define MCU_SW_MASK						(BIT(8) | BIT(9) | BIT(10) | BIT(11))
	#define MCU_SW_READ						((PTD->PDIR & MCU_SW_MASK) >> 8)	/*!< Returns a 4-bit value with SW4:1 mapping to bits 3:0 */

	#define MCU_LED_ON						PTC->PCOR = BIT(8)
	#define MCU_LED_OFF						PTC->PSOR = BIT(8)
	#define MCU_LED_TOGGLE					PTC->PTOR = BIT(8)
	#define MCU_LED(state)					{ if(state) { MCU_LED_ON; } else { MCU_LED_OFF; } }

	//SPI Bus signals
	//SPI_CS_AUX
	//SPI_CS_CFG
	//SPI_CS_LANSW

	//AUX Header signals
	#define AUX_IO0_ON						PTB->PSOR = BIT(4)
	#define AUX_IO0_OFF						PTB->PCOR = BIT(4)
	#define AUX_IO0(state)					{ if(state) { AUX_IO0_ON; } else { AUX_IO0_OFF; } }

	#define AUX_IO1_ON						PTB->PSOR = BIT(5)
	#define AUX_IO1_OFF						PTB->PCOR = BIT(5)
	#define AUX_IO1(state)					{ if(state) { AUX_IO1_ON; } else { AUX_IO1_OFF; } }

	//AUX2
	//AUX3

#endif

//==============================================================================
#endif
