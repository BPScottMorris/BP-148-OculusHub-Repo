/*==============================================================================
C++ Module that provides the definitions and implementation for a wrapper class
to provide management and access functions for a UART Kinetis K60 peripheral.

UART objects can be dynamically bound to different physical UARTs using the "Port"
property, and the connection is made (and severed) with the issuing of the "Open"
(and "Close") method. While the port is open, none of its properties can be changed.

Interrupts for each implemented UART should be enabled by defining the appropriate
UARTx_CONNECT_IRQ preprocessor symbol.

The static CComUart::Terminal pointer, allows a global Uart define to be mapped to
handle the "PrintF" C type functions by overriding the GetChar and PutChar functions.
This feature is enabled by default. To disable, define UART_TERMINAL as false.

15/10/2013 - Created v2.0 of file (from Artemis C++ code by K.Webster & R.Sharphouse)
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef COM_UART_HPP
#define COM_UART_HPP

//Include the base class
#include "com.hpp"

//Include the MCG device to get clock timings
#include "mcg.hpp"

//Include the FIFO buffer used by the UART transmit and receive routines
#include "fifobuffer.hpp"

//==============================================================================
//Class Definition...
//==============================================================================
/*! Enumeration that defines valid baud rates for the serial port */
enum EUartBaud {
	BAUD_75 = 0,
	BAUD_110 = 1,
	BAUD_150 = 2,
	BAUD_300 = 3,
	BAUD_600 = 4,
	BAUD_1200 = 5,
	BAUD_2400 = 6,
	BAUD_4800 = 7,
	BAUD_9600 = 8,
	BAUD_14400 = 9,
	BAUD_19200 = 10,
	BAUD_38400 = 11,
	BAUD_57600 = 12,
	BAUD_115200 = 13,
	BAUD_230400 = 14,
	BAUD_460800 = 15,
	BAUD_921600 = 16
} ;

/*! Define the upper value of the EUartBaud, for range checking */
#define EUART_BAUD_HIGH		BAUD_921600

/*! Enumeration that defines the types of parity available */
enum EUartParity {
	PARITY_NONE = 0,
	PARITY_ODD = 1,
	PARITY_EVEN = 2
};

/*! Define an enumeration of flags that the serial port reports its status with */
typedef uint8 TUartFlags;

//Define bits for TUartFlags
#define UART_PARITY_ERR_BIT			0
#define UART_FRAMING_ERR_BIT		1
#define UART_OVERRUN_ERR_BIT		2
#define UART_RXBUF_ERR_BIT			3

#define UART_PARITY_ERR_MASK		BIT(UART_PARITY_ERR_BIT)
#define UART_FRAMING_ERR_MASK		BIT(UART_FRAMING_ERR_BIT)
#define UART_OVERRUN_ERR_MASK		BIT(UART_OVERRUN_ERR_BIT)
#define UART_RXBUF_ERR_MASK			BIT(UART_RXBUF_ERR_BIT)

//------------------------------------------------------------------------------
//Predefine the class
class CComUart;

/*! Define a pointer to a serial port */
typedef CComUart* PComUart;

/*! Define a class that creates an interface to a hardware UART */
class CComUart : public CCom {
	private:
		typedef CCom 		base;					/*!< Allow access to the base class properties */

	protected:
		EUartBaud			_baud;
		TUartFlags			_flags;
		bool				_loopback;
		EUartParity			_parity;
		bool				_open;					/*!< True if the Serial port is open */
		uint8				_port;					/*!< The COM port number of the serial port */
		PByteFifoBuffer		_rxBuffer;
		PByteFifoBuffer		_txBuffer;
		bool				_txEnable;
		UART_Type*			_uart;					/*!< Pointer to the struct accessing the UART registers */

		//Protected methods
		virtual void DoTxMode(bool state, bool force = false);

	public:
		//Construction & Disposal
		CComUart(uint8 port, uint32 rxBufSize = 16, uint32 txBufSize = 16);
		virtual ~CComUart();

		//Methods
		void Clear(bool rx = true, bool tx = true);
		void Close(void);
		void DoISR(void);
		void Flush(void);
		EUartBaud GetBaudRate();
		TUartFlags GetFlags(bool clear = false);
		bool GetLoopback();
		EUartParity GetParity();
		uint8 GetPort();
		uint32 GetRxBufferCount();
		uint32 GetTxBufferCount();
		bool IsOpen(void);
		bool Open(void);
		uint8 ReadByte();
		void SetBaudRate(EUartBaud value);
		void SetParity(EUartParity value);
		void SetPort(uint8 value);
		void SetLoopback(bool value);
		void SetRxBufferSize(uint32 value);
		void SetTxBufferSize(uint32 value);
		void WriteByte(uint8 data);

		//Static Variables
		static PComUart Uart[UART_PERIPHERALS];		/*!< Global method pointer for interrupt handlers */
		//static PComUart Terminal;					/*!< Global pointer to the UArt to handle PrintF terminal io */

		//Static Methods
		static uint32 CalcBaudValue(EUartBaud baudRate);
		static EUartBaud CalcBaudRate(uint32 baudValue);
};

//==============================================================================
//Interrupt Handler Definitions...
//==============================================================================
#ifndef UART_PERIPHERALS
	PRAGMA_ERROR("Number of UART peripherals not defined")
#endif

//Generate warnings if clock definitions are missing
/*
#if !defined(CLK_SYS_Hz)
	#error CComUart requires the 'CLK_SYS_Hz' value to be defined for UART0 and UART1
#endif
#if !defined(CLK_BUS_Hz)
	#error CComUart requires the 'CLK_BUS_Hz' value to be defined for UART2 to UART5
#endif
*/

//If explicitly previously allowed, disable IRQ's for UART's
#ifndef UART0_CONNECT_IRQ
	#define UART0_CONNECT_IRQ	false
#endif
#ifndef UART1_CONNECT_IRQ
	#define UART1_CONNECT_IRQ	false
#endif
#ifndef UART2_CONNECT_IRQ
	#define UART2_CONNECT_IRQ	false
#endif
#ifndef UART3_CONNECT_IRQ
	#define UART3_CONNECT_IRQ	false
#endif
#ifndef UART4_CONNECT_IRQ
	#define UART4_CONNECT_IRQ	false
#endif
#ifndef UART5_CONNECT_IRQ
	#define UART5_CONNECT_IRQ	false
#endif

//Include prototypes for hardware Interrupt handlers (See Interrupt Vector Table)
#ifdef __cplusplus
extern "C" {
#endif

#if UART0_CONNECT_IRQ
void ISR_UART0_RX_TX(void)	__attribute__ ((interrupt));
#endif
#if UART1_CONNECT_IRQ
void ISR_UART1_RX_TX(void)	__attribute__ ((interrupt));
#endif
#if UART2_CONNECT_IRQ
void ISR_UART2_RX_TX(void)	__attribute__ ((interrupt));
#endif
#if UART3_CONNECT_IRQ
void ISR_UART3_RX_TX(void)	__attribute__ ((interrupt));
#endif
#if UART4_CONNECT_IRQ
void ISR_UART4_RX_TX(void)	__attribute__ ((interrupt));
#endif
#if UART5_CONNECT_IRQ
void ISR_UART5_RX_TX(void)	__attribute__ ((interrupt));
#endif

#ifdef __cplusplus
}
#endif

//==============================================================================
#endif
