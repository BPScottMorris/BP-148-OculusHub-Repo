//Include the file header
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Global variables */
volatile uint8 SR_reg;               /* Current value of the FAULTMASK register */
volatile uint8 SR_lock = 0x00U;      /* Lock */

//==============================================================================
//Flash configuration field
//==============================================================================
/*!
This section configures the default Flash Protection of the device.
*/
__attribute__ ((section (".cfmconfig"))) const uint8 _cfm[0x10] = {
	0xFFU,	/* NV_BACKKEY3: KEY=0xFF */
	0xFFU,	/* NV_BACKKEY2: KEY=0xFF */
	0xFFU,	/* NV_BACKKEY1: KEY=0xFF */
	0xFFU,	/* NV_BACKKEY0: KEY=0xFF */
	0xFFU,	/* NV_BACKKEY7: KEY=0xFF */
	0xFFU,	/* NV_BACKKEY6: KEY=0xFF */
	0xFFU,	/* NV_BACKKEY5: KEY=0xFF */
	0xFFU,	/* NV_BACKKEY4: KEY=0xFF */
	0xFFU,	/* NV_FPROT3: PROT=0xFF */
	0xFFU,	/* NV_FPROT2: PROT=0xFF */
	0xFFU,	/* NV_FPROT1: PROT=0xFF */
	0xFFU,	/* NV_FPROT0: PROT=0xFF */
#ifdef DEBUG
	//Backdoor Disabled, Mass Erase Enabled, Failure Analysis Granted, Flash Security UNSECURE
	(uint8)(FTFE_FSEC_KEYEN(3) | FTFE_FSEC_MEEN(3) | FTFE_FSEC_FSLACC(3) | FTFE_FSEC_SEC(2)),	/* NV_FSEC: */
#else
	//Backdoor Disabled, Mass Erase Enabled, Failure Analysis Granted, Flash Security SECURE
	//(KEYEN=3 allows reprogramming to 2 to enable access by Flash programmer)
	(uint8)(FTFE_FSEC_KEYEN(3) | FTFE_FSEC_MEEN(3) | FTFE_FSEC_FSLACC(3) | FTFE_FSEC_SEC(3)),	/* NV_FSEC: */
#endif
	0xFFU,	/* NV_FOPT: ??=1,??=1,??=1,??=1,??=1,NMI_DIS=1,EZPORT_DIS=1,LPBOOT=1 */
	0xFFU,	/* NV_FEPROT: EPROT=0xFF */
	0xFFU	/* NV_FDPROT: DPROT=0xFF */
};

//==============================================================================
//Interrupt Vector Table Definition
//==============================================================================
extern uint32 __SP_INIT;			//External definition of the Stack-Pointer register
extern void __thumb_startup(void);	//External definition of the Startup Function

__attribute__ ((section (".vectortable"))) const TIsrVectorTable __vect_table = {
	/* ISR name                             		     Vect	IRQ	Address		IRQ Name	Description                  */
	&__SP_INIT,										/*!< 0		-16	0x00000000							Initial_Stack_Pointer        */
	{
		(TIsrFunc)&__thumb_startup,					/*!< 1		-15	0x00000004							ISR_Initial_Program_Counter  */
		(TIsrFunc)&ISR_NMI,							/*!< 2		-14	0x00000008	NonMaskableInt_IRQn		Non Maskable Interrupt		 */
		(TIsrFunc)&ISR_Hard_Fault,					/*!< 3		-13	0x0000000C	HardFault_IRQn			Cortex-M4 SV Hard Fault Interrupt		 */
		(TIsrFunc)&ISR_Mem_Manage_Fault,			/*!< 4		-12	0x00000010	MemoryManagement_IRQn	Cortex-M4 Memory Management Interrupt		 */
		(TIsrFunc)&ISR_Bus_Fault,					/*!< 5		-11	0x00000014	BusFault_IRQn			Cortex-M4 Bus Fault Interrupt		 */
		(TIsrFunc)&ISR_Usage_Fault,					/*!< 6		-10	0x00000018	UsageFault_IRQn			Cortex-M4 Usage Fault Interrupt		 */
		(TIsrFunc)&ISR_Reserved7,					/*!< 7		-9	0x0000001C				 */
		(TIsrFunc)&ISR_Reserved8,					/*!< 8		-8	0x00000020				 */
		(TIsrFunc)&ISR_Reserved9,					/*!< 9		-7	0x00000024				 */
		(TIsrFunc)&ISR_Reserved10,					/*!< 10		-6	0x00000028				 */
		(TIsrFunc)&ISR_SVCall,						/*!< 11		-5	0x0000002C	SVCall_IRQn				Cortex-M4 SV Call Interrupt		 */
		(TIsrFunc)&ISR_DebugMonitor,				/*!< 12		-4	0x00000030	DebugMonitor_IRQn		Cortex-M4 Debug Monitor Interrupt		 */
		(TIsrFunc)&ISR_Reserved13,					/*!< 13		-3	0x00000034				 */
		(TIsrFunc)&ISR_PendableSrvReq,				/*!< 14		-2	0x00000038	PendSV_IRQn				Cortex-M4 Pend SV Interrupt		 */
		(TIsrFunc)&ISR_SysTick,						/*!< 15		-1	0x0000003C	SysTick_IRQn			Cortex-M4 System Tick Interrupt		 */
		(TIsrFunc)&ISR_DMA0,						/*!< 16		0	0x00000040	DMA0_DMA16_IRQn			DMA channel 0/16 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA1,						/*!< 17		1	0x00000044	DMA1_DMA17_IRQn			DMA channel 1/17 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA2,						/*!< 18		2	0x00000048	DMA2_DMA18_IRQn			DMA channel 2/18 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA3,						/*!< 19		3	0x0000004C	DMA3_DMA19_IRQn			DMA channel 3/19 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA4,						/*!< 20		4	0x00000050	DMA4_DMA20_IRQn			DMA channel 4/20 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA5,						/*!< 21		5	0x00000054	DMA5_DMA21_IRQn			DMA channel 5/21 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA6,						/*!< 22		6	0x00000058	DMA6_DMA22_IRQn			DMA channel 6/22 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA7,						/*!< 23		7	0x0000005C	DMA7_DMA23_IRQn			DMA channel 7/23 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA8,						/*!< 24		8	0x00000060	DMA8_DMA24_IRQn			DMA channel 8/24 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA9,						/*!< 25		9	0x00000064	DMA9_DMA25_IRQn			DMA channel 9/25 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA10,						/*!< 26		10	0x00000068	DMA10_DMA26_IRQn		DMA channel 10/26 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA11,						/*!< 27		11	0x0000006C	DMA11_DMA27_IRQn		DMA channel 11/27 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA12,						/*!< 28		12	0x00000070	DMA12_DMA28_IRQn		DMA channel 12/28 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA13,						/*!< 29		13	0x00000074	DMA13_DMA29_IRQn		DMA channel 13/29 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA14,						/*!< 30		14	0x00000078	DMA14_DMA30_IRQn		DMA channel 14/30 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA15,						/*!< 31		15	0x0000007C	DMA15_DMA31_IRQn		DMA channel 15/31 transfer complete interrupt		 */
		(TIsrFunc)&ISR_DMA_Error,					/*!< 32		16	0x00000080	DMA_Error_IRQn			DMA error interrupt		 */
		(TIsrFunc)&ISR_MCM,							/*!< 33		17	0x00000084	MCM_IRQn				Normal interrupt		 */
		(TIsrFunc)&ISR_FTFE,						/*!< 34		18	0x00000088	FTFE_IRQn				FTFE interrupt		 */
		(TIsrFunc)&ISR_Read_Collision,				/*!< 35		19	0x0000008C	Read_Collision_IRQn		Read collision interrupt		 */
		(TIsrFunc)&ISR_LVD_LVW,						/*!< 36		20	0x00000090	LVD_LVW_IRQn			Low Voltage Detect, Low Voltage Warning		 */
		(TIsrFunc)&ISR_LLWU,						/*!< 37		21	0x00000094	LLW_IRQn				Low Leakage Wakeup		 */
		(TIsrFunc)&ISR_WDOG_EWM,					/*!< 38		22	0x00000098	Watchdog_IRQn			WDOG interrupt		 */
		(TIsrFunc)&ISR_RNGB,						/*!< 39		23	0x0000009C	RNG_IRQn				RNGB interrupt		 */
		(TIsrFunc)&ISR_I2C0,						/*!< 40		24	0x000000A0	I2C0_IRQn				I2C0 interrupt		 */
		(TIsrFunc)&ISR_I2C1,						/*!< 41		25	0x000000A4	I2C1_IRQn				I2C1 interrupt		 */
		(TIsrFunc)&ISR_SPI0,						/*!< 42		26	0x000000A8	SPI0_IRQn				SPI0 interrupt		 */
		(TIsrFunc)&ISR_SPI1,						/*!< 43		27	0x000000AC	SPI1_IRQn				SPI1 interrupt		 */
		(TIsrFunc)&ISR_SPI2,						/*!< 44		28	0x000000B0	SPI2_IRQn				SPI2 interrupt		 */
		(TIsrFunc)&ISR_CAN0_ORed_Message_buffer,	/*!< 45		29	0x000000B4	CAN0_ORed_Message_buffer_IRQn	CAN0 OR'd message buffers interrupt		 */
		(TIsrFunc)&ISR_CAN0_Bus_Off,				/*!< 46		30	0x000000B8	CAN0_Bus_Off_IRQn		CAN0 bus off interrupt		 */
		(TIsrFunc)&ISR_CAN0_Error,					/*!< 47		31	0x000000BC	CAN0_Error_IRQn			CAN0 error interrupt		 */
		(TIsrFunc)&ISR_CAN0_Tx_Warning,				/*!< 48		32	0x000000C0	CAN0_Tx_Warning_IRQn	CAN0 Tx warning interrupt		 */
		(TIsrFunc)&ISR_CAN0_Rx_Warning,				/*!< 49		33	0x000000C4	CAN0_Rx_Warning_IRQn	CAN0 Rx warning interrupt		 */
		(TIsrFunc)&ISR_CAN0_Wake_Up,				/*!< 50		34	0x000000C8	CAN0_Wake_Up_IRQn		CAN0 wake up interrupt		 */
		(TIsrFunc)&ISR_I2S0_Tx,						/*!< 51		35	0x000000CC	I2S0_Tx_IRQn			I2S0 transmit interrupt		 */
		(TIsrFunc)&ISR_I2S0_Rx,						/*!< 52		36	0x000000D0	I2S0_Rx_IRQn			I2S0 receive interrupt		 */
		(TIsrFunc)&ISR_CAN1_ORed_Message_buffer,	/*!< 53		37	0x000000D4	CAN1_ORed_Message_buffer_IRQn	CAN1 OR'd message buffers interrupt		 */
		(TIsrFunc)&ISR_CAN1_Bus_Off,				/*!< 54		38	0x000000D8	CAN1_Bus_Off_IRQn		CAN1 bus off interrupt		 */
		(TIsrFunc)&ISR_CAN1_Error,					/*!< 55		39	0x000000DC	CAN1_Error_IRQn			CAN1 error interrupt		 */
		(TIsrFunc)&ISR_CAN1_Tx_Warning,				/*!< 56		40	0x000000E0	CAN1_Tx_Warning_IRQn	CAN1 Tx warning interrupt		 */
		(TIsrFunc)&ISR_CAN1_Rx_Warning,				/*!< 57		41	0x000000E4	CAN1_Rx_Warning_IRQn	CAN1 Rx warning interrupt		 */
		(TIsrFunc)&ISR_CAN1_Wake_Up,				/*!< 58		42	0x000000E8	CAN1_Wake_Up_IRQn		CAN1 wake up interrupt		 */
		(TIsrFunc)&ISR_Reserved59,					/*!< 59		43	0x000000EC	Reserved59_IRQn			Reserved interrupt 59		 */
		(TIsrFunc)&ISR_UART0_LON,					/*!< 60		44	0x000000F0	UART0_LON_IRQn			UART0 LON interrupt		 */
		(TIsrFunc)&ISR_UART0_RX_TX,					/*!< 61		45	0x000000F4	UART0_RX_TX_IRQn		UART0 receive/transmit interrupt		 */
		(TIsrFunc)&ISR_UART0_ERR,					/*!< 62		46	0x000000F8	UART0_ERR_IRQn			UART0 error interrupt		 */
		(TIsrFunc)&ISR_UART1_RX_TX,					/*!< 63		47	0x000000FC	UART1_RX_TX_IRQn		UART1 receive/transmit interrupt		 */
		(TIsrFunc)&ISR_UART1_ERR,					/*!< 64		48	0x00000100	UART1_ERR_IRQn			UART1 error interrupt		 */
		(TIsrFunc)&ISR_UART2_RX_TX,					/*!< 65		49	0x00000104	UART2_RX_TX_IRQn		UART2 receive/transmit interrupt		 */
		(TIsrFunc)&ISR_UART2_ERR,					/*!< 66		50	0x00000108	UART2_ERR_IRQn			UART2 error interrupt		 */
		(TIsrFunc)&ISR_UART3_RX_TX,					/*!< 67		51	0x0000010C	UART3_RX_TX_IRQn		UART3 receive/transmit interrupt		 */
		(TIsrFunc)&ISR_UART3_ERR,					/*!< 68		52	0x00000110	UART3_ERR_IRQn			UART3 error interrupt		 */
		(TIsrFunc)&ISR_UART4_RX_TX,					/*!< 69		53	0x00000114	UART4_RX_TX_IRQn		UART4 receive/transmit interrupt		 */
		(TIsrFunc)&ISR_UART4_ERR,					/*!< 70		54	0x00000118	UART4_ERR_IRQn			UART4 error interrupt		 */
		(TIsrFunc)&ISR_UART5_RX_TX,					/*!< 71		55	0x0000011C	UART5_RX_TX_IRQn		UART5 receive/transmit interrupt		 */
		(TIsrFunc)&ISR_UART5_ERR,					/*!< 72		56	0x00000120	UART5_ERR_IRQn			UART5 error interrupt		 */
		(TIsrFunc)&ISR_ADC0,						/*!< 73		57	0x00000124	ADC0_IRQn				ADC0 interrupt		 */
		(TIsrFunc)&ISR_ADC1,						/*!< 74		58	0x00000128	ADC1_IRQn				ADC1 interrupt		 */
		(TIsrFunc)&ISR_CMP0,						/*!< 75		59	0x0000012C	CMP0_IRQn				CMP0 interrupt		 */
		(TIsrFunc)&ISR_CMP1,						/*!< 76		60	0x00000130	CMP1_IRQn				CMP1 interrupt		 */
		(TIsrFunc)&ISR_CMP2,						/*!< 77		61	0x00000134	CMP2_IRQn				CMP2 interrupt		 */
		(TIsrFunc)&ISR_FTM0,						/*!< 78		62	0x00000138	FTM0_IRQn				FTM0 fault, overflow and channels interrupt		 */
		(TIsrFunc)&ISR_FTM1,						/*!< 79		63	0x0000013C	FTM1_IRQn				FTM1 fault, overflow and channels interrupt		 */
		(TIsrFunc)&ISR_FTM2,						/*!< 80		64	0x00000140	FTM2_IRQn				FTM2 fault, overflow and channels interrupt		 */
		(TIsrFunc)&ISR_CMT,							/*!< 81		65	0x00000144	CMT_IRQn				CMT interrupt		 */
		(TIsrFunc)&ISR_RTC,							/*!< 82		66	0x00000148	RTC_IRQn				RTC interrupt		 */
		(TIsrFunc)&ISR_RTC_Seconds,					/*!< 83		67	0x0000014C	RTC_Seconds_IRQn		RTC seconds interrupt		 */
		(TIsrFunc)&ISR_PIT0,						/*!< 84		68	0x00000150	PIT0_IRQn				PIT timer channel 0 interrupt		 */
		(TIsrFunc)&ISR_PIT1,						/*!< 85		69	0x00000154	PIT1_IRQn				PIT timer channel 1 interrupt		 */
		(TIsrFunc)&ISR_PIT2,						/*!< 86		70	0x00000158	PIT2_IRQn				PIT timer channel 2 interrupt		 */
		(TIsrFunc)&ISR_PIT3,						/*!< 87		71	0x0000015C	PIT3_IRQn				PIT timer channel 3 interrupt		 */
		(TIsrFunc)&ISR_PDB0,						/*!< 88		72	0x00000160	PDB0_IRQn				PDB0 interrupt		 */
		(TIsrFunc)&ISR_USB0,						/*!< 89		73	0x00000164	USB0_IRQn				USB0 interrupt		 */
		(TIsrFunc)&ISR_USBDCD,						/*!< 90		74	0x00000168	USBDCD_IRQn				USBDCD interrupt		 */
		(TIsrFunc)&ISR_ENET_1588_Timer,				/*!< 91		75	0x0000016C	ENET_1588_Timer_IRQn	Ethernet MAC IEEE 1588 timer interrupt	*/
		(TIsrFunc)&ISR_ENET_Transmit,				/*!< 92		76	0x00000170	ENET_Transmit_IRQn		Ethernet MAC transmit interrupt		 		*/
		(TIsrFunc)&ISR_ENET_Receive,				/*!< 93		77	0x00000174	ENET_Receive_IRQn		Ethernet MAC receive interrupt				*/
		(TIsrFunc)&ISR_ENET_Error,					/*!< 94		78	0x00000178	ENET_Error_IRQn			Ethernet MAC error and miscelaneous interrupt	*/
		(TIsrFunc)&ISR_Reserved95,					/*!< 95		79	0x0000017C	Reserved95_IRQn			Reserved interrupt 95		 					*/
		(TIsrFunc)&ISR_SDHC,						/*!< 96		80	0x00000180	SDHC_IRQn				SDHC interrupt		 								*/
		(TIsrFunc)&ISR_DAC0,						/*!< 97		81	0x00000184	DAC0_IRQn				DAC0 interrupt		 								*/
		(TIsrFunc)&ISR_DAC1,						/*!< 98		82	0x00000188	DAC1_IRQn				DAC1 interrupt										*/
		(TIsrFunc)&ISR_TSI0,						/*!< 99		83	0x0000018C	TSI0_IRQn				TSI0 interrupt		 								*/
		(TIsrFunc)&ISR_MCG,							/*!< 100	84	0x00000190	MCG_IRQn				MCG interrupt		 								*/
		(TIsrFunc)&ISR_LPTMR0,						/*!< 101	85	0x00000194	LPTimer_IRQn			LPTimer interrupt		 */
		(TIsrFunc)&ISR_Reserved102,					/*!< 102	86	0x00000198	Reserved102_IRQn		Reserved interrupt 102		 */
		(TIsrFunc)&ISR_PORTA,						/*!< 103	87	0x0000019C	PORTA_IRQn				Port A interrupt		 */
		(TIsrFunc)&ISR_PORTB,						/*!< 104	88	0x000001A0	PORTB_IRQn				Port B interrupt		 */
		(TIsrFunc)&ISR_PORTC,						/*!< 105	89	0x000001A4	PORTC_IRQn				Port C interrupt		 */
		(TIsrFunc)&ISR_PORTD,						/*!< 106	90	0x000001A8	PORTD_IRQn				Port D interrupt		 */
		(TIsrFunc)&ISR_PORTE,						/*!< 107	91	0x000001AC	PORTE_IRQn				Port E interrupt		 */
		(TIsrFunc)&ISR_PORTF,						/*!< 108	92	0x000001B0	PORTF_IRQn				Port F interrupt		 */
		(TIsrFunc)&ISR_Reserved109,					/*!< 109	93	0x000001B4	Reserved109_IRQn		Reserved interrupt 109		 */
		(TIsrFunc)&ISR_SWI,							/*!< 110	94	0x000001B8	SWI_IRQn				Software interrupt		 */
		(TIsrFunc)&ISR_NFC,							/*!< 111	95	0x000001BC	NFC_IRQn				NAND flash controller interrupt		 */
		(TIsrFunc)&ISR_USBHS,						/*!< 112	96	0x000001C0	USBHS_IRQn				USB high speed OTG interrupt		 */
		(TIsrFunc)&ISR_Reserved113,					/*!< 113	97	0x000001C4	Reserved113_IRQn		Reserved interrupt 113		 */
		(TIsrFunc)&ISR_CMP3,						/*!< 114	98	0x000001C8	CMP3_IRQn				CMP3 interrupt		 */
		(TIsrFunc)&ISR_Reserved115,					/*!< 115	99	0x000001CC	Reserved115_IRQn		Reserved interrupt 115		 */
		(TIsrFunc)&ISR_Reserved116,					/*!< 116	100	0x000001D0	Reserved116_IRQn		Reserved interrupt 116		 */
		(TIsrFunc)&ISR_FTM3,						/*!< 117	101	0x000001D4	FTM3_IRQn				FTM3 fault, overflow and channels interrupt		 */
		(TIsrFunc)&ISR_ADC2,						/*!< 118	102	0x000001D8	ADC2_IRQn				ADC2 interrupt		 */
		(TIsrFunc)&ISR_ADC3,						/*!< 119	103	0x000001DC	ADC3_IRQn				ADC3 interrupt		 */
		(TIsrFunc)&ISR_I2S1_Tx,						/*!< 120	104	0x000001E0	I2S1_Tx_IRQn			I2S1 transmit interrupt		 */
		(TIsrFunc)&ISR_I2S1_Rx						/*!< 121	105	0x000001E4	I2S1_Rx_IRQn			I2S1 receive interrupt		 */
	}
};

//==============================================================================
//Interrupt Handler Definitions
//==============================================================================
/*!-----------------------------------------------------------------------------
Define the default implementation of the ISR that executes for all undefined
ISR table vectors.
*/
void ISR_Default()
{
	HALT;
}

/*------------------------------------------------------------------------------
Define weak declarations for the implementation of interrupt vectors in the
ISR vector table. By default these map to the ISR_Default handler, but
Implementing a function of the same name will override the weak declaration.
*/
void ISR_NMI()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Hard_Fault()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Mem_Manage_Fault()			__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Bus_Fault()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Usage_Fault()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved7()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved8()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved9()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved10()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_SVCall()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DebugMonitor()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved13()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PendableSrvReq()			__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_SysTick()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA0()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA1()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA2()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA3()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA4()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA5()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA6()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA7()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA8()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA9()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA10()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA11()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA12()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA13()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA14()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA15()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DMA_Error()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_MCM()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_FTFE()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Read_Collision()			__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_LVD_LVW()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_LLWU()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_WDOG_EWM()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_RNGB()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_I2C0()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_I2C1()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_SPI0()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_SPI1()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_SPI2()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN0_ORed_Message_buffer()	__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN0_Bus_Off()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN0_Error()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN0_Tx_Warning()			__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN0_Rx_Warning()			__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN0_Wake_Up()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_I2S0_Tx()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_I2S0_Rx()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN1_ORed_Message_buffer()	__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN1_Bus_Off()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN1_Error()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN1_Tx_Warning()			__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN1_Rx_Warning()			__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CAN1_Wake_Up()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved59()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART0_LON()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART0_RX_TX()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART0_ERR()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART1_RX_TX()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART1_ERR()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART2_RX_TX()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART2_ERR()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART3_RX_TX()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART3_ERR()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART4_RX_TX()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART4_ERR()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART5_RX_TX()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_UART5_ERR()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_ADC0()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_ADC1()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CMP0()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CMP1()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CMP2()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_FTM0()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_FTM1()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_FTM2()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CMT()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_RTC()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_RTC_Seconds()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PIT0()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PIT1()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PIT2()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PIT3()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PDB0()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_USB0()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_USBDCD()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_ENET_1588_Timer()			__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_ENET_Transmit()			__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_ENET_Receive()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_ENET_Error()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved95()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_SDHC()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DAC0()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_DAC1()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_TSI0()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_MCG()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_LPTMR0()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved102()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PORTA()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PORTB()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PORTC()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PORTD()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PORTE()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_PORTF()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved109()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_SWI()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_NFC()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_USBHS()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved113()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_CMP3()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved115()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_Reserved116()				__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_FTM3()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_ADC2()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_ADC3()						__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_I2S1_Tx()					__attribute__ ((interrupt, weak, alias("ISR_Default")));
void ISR_I2S1_Rx()					__attribute__ ((interrupt, weak, alias("ISR_Default")));

#ifdef __cplusplus
}  /* extern "C" */
#endif

//==============================================================================
