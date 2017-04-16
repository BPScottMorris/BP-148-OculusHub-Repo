#include "com_uart.hpp"

//==============================================================================
//CComUart
//=============================================================================
//Initialise static memory...

/*! Memory to hold which Uart objects are bound to each IRQ handler */
PComUart CComUart::Uart[UART_PERIPHERALS] = { NULL };

/*!-----------------------------------------------------------------------------
*/
CComUart::CComUart(uint8 port, uint32 rxBufSize, uint32 txBufSize)
{
	//Initialise the port to a closed state
	_port = port;
	_flags = 0;
	_baud = BAUD_9600;
	_loopback = false;
	_parity = PARITY_NONE;
	_open = false;

	_uart = NULL;

	//Create ring-buffers with default size
	_rxBuffer = new CByteFifoBuffer(rxBufSize);
	_txBuffer = new CByteFifoBuffer(txBufSize);

	//Initialise the TxEnable to the false state
	//(dont raise an interrupt here as nothing will be connected to it)
	_txEnable = false;
}

/*!-----------------------------------------------------------------------------
*/
CComUart::~CComUart()
{
	//Close the port (and release the UART)
	this->Close();

	//Destroy resources
	delete _rxBuffer;
	delete _txBuffer;
}

/*!-----------------------------------------------------------------------------
Function used to convert the enumerated baud-rate type
@param baudRate The baud rate enumeration to convert
@result The numerical value of the baud rate or 0 if an invalid enum is specified
*/
uint32 CComUart::CalcBaudValue(EUartBaud baudRate)
{
	switch(baudRate) {
		case BAUD_75 : return 75;
		case BAUD_110 : return 110;
		case BAUD_150 : return 150;
		case BAUD_300 : return 300;
		case BAUD_600 : return 600;
		case BAUD_1200 : return 1200;
		case BAUD_2400 : return 2400;
		case BAUD_4800 : return 4800;
		case BAUD_9600 : return 9600;
		case BAUD_14400 : return 14400;
		case BAUD_19200 : return 19200;
		case BAUD_38400 : return 38400;
		case BAUD_57600 : return 57600;
		case BAUD_115200 : return 115200;
		case BAUD_230400 : return 230400;
		case BAUD_460800 : return 460800;
		case BAUD_921600 : return 921600;
		default : return 0;
	}
}

/*!-----------------------------------------------------------------------------
Function that returns a Baud Rate enumeration value for the specified baud value
@param baudValue The baud value to round to the nearest enumerated value
@result The baud rate enumeration
*/
EUartBaud CComUart::CalcBaudRate(uint32 baudValue)
{
	if(baudValue <= 75) return BAUD_75;
	else if(baudValue <= 110) return BAUD_110;
	else if(baudValue <= 150) return BAUD_150;
	else if(baudValue <= 300) return BAUD_300;
	else if(baudValue <= 600) return BAUD_600;
	else if(baudValue <= 1200) return BAUD_1200;
	else if(baudValue <= 2400) return BAUD_2400;
	else if(baudValue <= 4800) return BAUD_4800;
	else if(baudValue <= 9600) return BAUD_9600;
	else if(baudValue <= 14400) return BAUD_14400;
	else if(baudValue <= 19200) return BAUD_19200;
	else if(baudValue <= 38400) return BAUD_38400;
	else if(baudValue <= 57600) return BAUD_57600;
	else if(baudValue <= 115200) return BAUD_115200;
	else if(baudValue <= 230400) return BAUD_230400;
	else if(baudValue <= 460800) return BAUD_460800;
	else return BAUD_921600; //if(baudValue <= 921600)
}

/*!-----------------------------------------------------------------------------
Function that clears down the specified UART buffers
*/
void CComUart::Clear(bool rx, bool tx)
{
	IRQ_DISABLE;
	if(rx)
		_rxBuffer->Clear();
	if(tx)
		_txBuffer->Clear();
	IRQ_ENABLE;
}

/*!-----------------------------------------------------------------------------
Function that is called to close the serial ports connect to the COM port
and free up the UART hardware.
*/
void CComUart::Close(void)
{
	if(_open) {
		//Force the transmitter enable to false regardless of current state
		//(This allows inheriting classes to set half-duplex control states
		//back to receive before shut down)
		this->DoTxMode(false, true);

		//Disable the UART hardware and interrupts
		CLR_BITS(_uart->C2, UART_C2_TIE_MASK | UART_C2_RIE_MASK | UART_C2_TE_MASK | UART_C2_RE_MASK);

		//Turn off the UART port clock
		switch (_port) {
			case 0 : {
				NVIC_DisableIRQ(UART0_RX_TX_IRQn);
				CLR_BITS(SIM->SCGC4, SIM_SCGC4_UART0_MASK);
				break;
			}
			case 1 : {
				NVIC_DisableIRQ(UART1_RX_TX_IRQn);
				CLR_BITS(SIM->SCGC4, SIM_SCGC4_UART1_MASK);
				break;
			}
			case 2 : {
				NVIC_DisableIRQ(UART2_RX_TX_IRQn);
				CLR_BITS(SIM->SCGC4, SIM_SCGC4_UART2_MASK);
				break;
			}
			#if UART_PERIPHERALS >= 4
			case 3 : {
				NVIC_DisableIRQ(UART3_RX_TX_IRQn);
				CLR_BITS(SIM->SCGC4, SIM_SCGC4_UART3_MASK);
				break;
			}
			#endif
			#if UART_PERIPHERALS >= 5
			case 4 : {
				NVIC_DisableIRQ(UART4_RX_TX_IRQn);
				CLR_BITS(SIM->SCGC1, SIM_SCGC1_UART4_MASK);
				break;
			}
			#endif
			#if UART_PERIPHERALS >= 6
			case 5 : {
				NVIC_DisableIRQ(UART5_RX_TX_IRQn);
				CLR_BITS(SIM->SCGC1, SIM_SCGC1_UART5_MASK);
				break;
			}
			#endif
		}

		//Clear the UART 'serial port' handler pointer for the COM port
		CComUart::Uart[_port] = NULL;

		//Clear the UART control structure
		_uart = NULL;
	}

	//Clear the Transmit Buffer
	_txBuffer->Clear();

	//Indicate the port is closed
	_open = false;
}

/*!-----------------------------------------------------------------------------
Function that is called by the UART handler when the UART IRQ is raise.
As this function is called from an Interrupt, any class fields or members it
calls must be "thread safe".
*/
void CComUart::DoISR(void)
{
	//if(_open) {

		//Read the UART status register
		volatile uint8 status = _uart->S1;

		//Always read the data register to clear the interrupt flags
		uint8 data = _uart->D;

		//Service the Receiver...
		if(_uart->C2 & UART_C2_RIE_MASK) {

			if(status & UART_S1_OR_MASK)
				//An overrun error has occurred
				SET_BITS(_flags, UART_OVERRUN_ERR_MASK);

			else if(status & UART_S1_FE_MASK)
				//A framing error has occurred
				SET_BITS(_flags, UART_FRAMING_ERR_MASK);

			else if(status & UART_S1_PF_MASK)
				//A parity error has occurred
				SET_BITS(_flags, UART_PARITY_ERR_MASK);

			else if(status & UART_S1_RDRF_MASK) {
				//If the receiver has data, then read and store it...
				//data = _uart->D;

				if(!_rxBuffer->IsFull()) {
					//Store the data
					_rxBuffer->Push(data);
				}
				else {
					//The buffer is full, so discard the data, but set the error flag
					SET_BITS(_flags, UART_RXBUF_ERR_MASK);
				}
			}
		}

		//Service the Transmitter Data Register...
		if(IS_BITS_SET(_uart->C2, UART_C2_TIE_MASK) && IS_BITS_SET(status, UART_S1_TDRE_MASK)) {
			if(_txBuffer->IsEmpty()) {
				//If the buffer is empty, turn off transmitter interrupts, but enable the complete interrupt
				CLR_BITS(_uart->C2, UART_C2_TIE_MASK);
				SET_BITS(_uart->C2, UART_C2_TCIE_MASK);
			}
			else {
				//If the buffer has data, then start transmitting it
				_txBuffer->Pop(&data);
				_uart->D = data;
			}
		}

		//Service the Transmit Complete, to return Half-Duplex to receive mode
		if(IS_BITS_SET(_uart->C2, UART_C2_TCIE_MASK) && IS_BITS_SET(status, UART_S1_TC_MASK)) {
			//If the transmitter has finished sending data, then disable the Transmit Complete interrupt
			CLR_BITS(_uart->C2, UART_C2_TCIE_MASK);

			//Disable the transmitter hardware (for Half-Duplex use)
			this->DoTxMode(false);
		}

	//}
}

/*!-----------------------------------------------------------------------------
Function used to raise an OnTxEnableISR event when the Tx Enabled state changes
(or we wish to force a new state)
*/
void CComUart::DoTxMode(bool state, bool force)
{
	IRQ_DISABLE;

	//If the state has changed, or we forcing a new state
	if((state != _txEnable) || force) {
		_txEnable = state;

		if(_txEnable) {
			//Enable the transmitter interrupts
			CLR_BITS(_uart->C2, UART_C2_TCIE_MASK);
			SET_BITS(_uart->C2, UART_C2_TIE_MASK);
		}
		else {
			//Disable the transmitter interrupts, ensure the receiver is enabled
			CLR_BITS(_uart->C2, UART_C2_TIE_MASK | UART_C2_TCIE_MASK);
			SET_BITS(_uart->C2, UART_C2_RIE_MASK | UART_C2_TE_MASK | UART_C2_RE_MASK);
		}
	}

	IRQ_ENABLE;
}

/*!-----------------------------------------------------------------------------
Function that is called to wait while the transmit buffer empites.
Useful for waiting for chars to be transmitted before closing the UART
*/
void CComUart::Flush()
{
	//Don't allow data to be read if the port is closed
	if(!_open)
		return;

	//Wait while the transmit buffer isn't empty
	//Do this in an interrupt safe way, so the buffer cannot be accessed simultaneously
	volatile bool wait = true;
	while(wait) {
		IRQ_DISABLE;
		wait = !(_txBuffer->IsEmpty());
		IRQ_ENABLE;
		NOP;
		//### Perhaps need a Watchdog reset here!
	}
}

/*!-----------------------------------------------------------------------------
*/
EUartBaud CComUart::GetBaudRate()
{
	return _baud;
}

/*!-----------------------------------------------------------------------------
Function that returns the Serial Port status flags.
As the flags are set by interrupts, this is done in an ISR safe manor
@param clear True if the flags should be cleared after reading
@result The enumeration of flag bits - see ESerialFlags.
*/
TUartFlags CComUart::GetFlags(bool clear)
{
	//If the port is open, then disable interrupts
	IRQ_DISABLE;

	//Copying the flags should be an atomic operation, do no ISR protection used
	TUartFlags flags = _flags;

	//If required clear the flags
	if(clear)
		_flags = 0;

	//If the port is open, then re-enable interrupts
	IRQ_ENABLE;

	return flags;
}

/*!-----------------------------------------------------------------------------
*/
bool CComUart::GetLoopback()
{
	return _loopback;
}

/*!-----------------------------------------------------------------------------
*/
EUartParity CComUart::GetParity()
{
	return _parity;
}

/*!-----------------------------------------------------------------------------
*/
uint8 CComUart::GetPort()
{
	return _port;
}

/*!-----------------------------------------------------------------------------
*/
uint32 CComUart::GetRxBufferCount()
{
	IRQ_DISABLE;
	uint32 count = _rxBuffer->GetCount();
	IRQ_ENABLE;

	return count;
}

/*!-----------------------------------------------------------------------------
*/
uint32 CComUart::GetTxBufferCount()
{
	IRQ_DISABLE;
	uint32 count = _txBuffer->GetCount();
	IRQ_ENABLE;

	return count;
}

/*!-----------------------------------------------------------------------------
Function that returns if the serial port is open
@result True if the serial port is open
*/
bool CComUart::IsOpen(void) {
	return _open;
}

/*!-----------------------------------------------------------------------------
Function that is called to open the serial ports on the specified COM port
and initialise the UART hardware
@result True if the port was opened successfully, False if the port cannot be opened
*/
bool CComUart::Open(void)
{
	uint32 uartClk;
	uint32 uartBaudMin;
	uint32 uartBaud;
	double uartDivisor;
	uint16 uartSBR;
	uint8 uartBRFD;

	//Abort if the serial port is already open, but return success
	if(_open)
		return true;

	//If another serial port object has already connected to the UART, then abort
	if((_port >= UART_PERIPHERALS) || (CComUart::Uart[_port] != NULL))
		return false;

	//Get the UART access structure for the specified COM port
	//and other hardware specific variables
	switch (_port) {
		case 0 : {
			_uart = UART0;
			uartClk = CMcg::ClkSysFreq; //CLK_SYS_Hz;
			SET_BITS(SIM->SCGC4, SIM_SCGC4_UART0_MASK);
			NVIC_EnableIRQ(UART0_RX_TX_IRQn);
			break;
		}
		case 1 : {
			_uart = UART1;
			uartClk = CMcg::ClkSysFreq; //CLK_SYS_Hz;
			SET_BITS(SIM->SCGC4, SIM_SCGC4_UART1_MASK);
			NVIC_EnableIRQ(UART1_RX_TX_IRQn);
			break;
		}
		case 2 : {
			_uart = UART2;
			uartClk = CMcg::ClkIntBusFreq; //CLK_BUS_Hz;
			SET_BITS(SIM->SCGC4, SIM_SCGC4_UART2_MASK);
			NVIC_EnableIRQ(UART2_RX_TX_IRQn);
			break;
		}
		#if UART_PERIPHERALS >= 4
		case 3 : {
			_uart = UART3;
			uartClk = CMcg::ClkIntBusFreq; //CLK_BUS_Hz;
			SET_BITS(SIM->SCGC4, SIM_SCGC4_UART3_MASK);
			NVIC_EnableIRQ(UART3_RX_TX_IRQn);
			break;
		}
		#endif
		#if UART_PERIPHERALS >= 5
		case 4 : {
			_uart = UART4;
			uartClk = CMcg::ClkIntBusFreq; //CLK_BUS_Hz;
			SET_BITS(SIM->SCGC1, SIM_SCGC1_UART4_MASK);
			NVIC_EnableIRQ(UART4_RX_TX_IRQn);
			break;
		}
		#endif
		#if UART_PERIPHERALS >= 6
		case 5 : {
			_uart = UART5;
			uartClk = CMcg::ClkIntBusFreq; //CLK_BUS_Hz;
			SET_BITS(SIM->SCGC1, SIM_SCGC1_UART5_MASK);
			NVIC_EnableIRQ(UART5_RX_TX_IRQn);
			break;
		}
		#endif
		default: {
			return false;	//Abort if the port is invalid
		}
	}

	//Connect the serial port object to the COM port.
	//This prevents another serial port object form connecting while this is open.
	CComUart::Uart[_port] = this;

	//Empty buffers
	_rxBuffer->Clear();
	_txBuffer->Clear();

	//------------------------------
	//SETUP BAUD RATE...
	//Calculate the minimum baud rate the UART can achieve from the CLK
	//NB: 8191 is the maximum value than can be loaded into SBR (2^13 - 1)
	uartBaudMin = uartClk / (16 * 8191);

	//Calculate the actual baud rate from the enumeration
	uartBaud = CComUart::CalcBaudValue(_baud);

	//Ensure the baud rate is achievable on the hardware, and recompute if not
	if(uartBaud < uartBaudMin) {
		_baud = CComUart::CalcBaudRate(uartBaudMin);
		uartBaud = CComUart::CalcBaudValue(_baud);
	}

	//Calculate the UART divisor (SBR + BRFD)
	uartDivisor = (double)uartClk / (16.0 * (double)uartBaud);

	//Calculate the UART's baud rate divisors - SBR for the Serial Baud Rate
	//and BRFD for Baud Rate Fractional Divisor, in 1/32'nds
	uartSBR = ((uint16)uartDivisor) & 0x0FFF;
	uartBRFD = ((uint8)((uartDivisor - (double)uartSBR) * 32.0)) & 0x1F;

	//Setup the UART's baud rate
	_uart->BDH = (uint8)((uartSBR >> 8) & 0xFF);
	_uart->BDL = (uint8)((uartSBR) & 0xFF);
	CLR_BITS(_uart->C4, UART_C4_BRFA_MASK);
	SET_BITS(_uart->C4, UART_C4_BRFA(uartBRFD));

	//------------------------------
	//SETUP PARITY...
	if(_parity == PARITY_NONE) {
		//Disable Parity
		CLR_BITS(_uart->C1, UART_C1_PE_MASK);
	}
	else {
		//Enable parity
		_uart->C1 |= UART_C1_PE_MASK;
		if(_parity == PARITY_ODD)
			SET_BITS(_uart->C1, UART_C1_PT_MASK);
		else
			CLR_BITS(_uart->C1, UART_C1_PT_MASK);
	}

	//------------------------------
	//SETUP LOOPBACK...
	if(_loopback)
		SET_BITS(_uart->C1, UART_C1_LOOPS_MASK);
	else
		CLR_BITS(_uart->C1, UART_C1_LOOPS_MASK);

	//------------------------------
	//Setup the Transmitter into Receive mode (regardless of current state)
	//this disables the transmitter interrupts and allows reception interrupts to start
	this->DoTxMode(false, true);

	//Mark the port as open
	_open = true;

	//Indicate success
	return true;
}

/*!-----------------------------------------------------------------------------
Function that reads a single byte from the receive buffer. If no byte is present
the function will block until the Serial Port received a byte
@result The read byte from the buffer
*/
uint8 CComUart::ReadByte()
{
	uint8 data;

	//Don't allow data to be read if the port is closed
	if(!_open)
		return 0;

	//If the buffer is empty, then loop until we receive a byte
	//Do this in an interrupt safe way, so the buffer cannot be accessed simultaneously
	volatile bool wait = true;
	while(wait) {
		IRQ_DISABLE;
		wait = _rxBuffer->IsEmpty();
		IRQ_ENABLE;
		NOP;
		//### Perhaps need a Watchdog reset here!
	}

	//Read the byte from the buffer
	IRQ_DISABLE;
	_rxBuffer->Pop(&data);
	IRQ_ENABLE;

	return data;
}

/*!-----------------------------------------------------------------------------
*/
void CComUart::SetBaudRate(EUartBaud value)
{
	//Store the baud rate if the port is closed
	if(!_open)
		_baud = value;
}

/*!-----------------------------------------------------------------------------
*/
void CComUart::SetParity(EUartParity value)
{
	//Setup the parity if the port is closed
	if(!_open)
		_parity = value;
}

/*!-----------------------------------------------------------------------------
Function that sets the COM port (not UART port!) that the serial port object
should connect to
@param value The COM port number form 0 to COM_PORTS that the serial port should connect to
*/
void CComUart::SetPort(uint8 value)
{
	//Change the port if the port is current closed
	if(!_open)
		_port = value;
}

/*!-----------------------------------------------------------------------------
*/
void CComUart::SetLoopback(bool value)
{
	//Store the loopback state if the port is closed
	if(!_open)
		_loopback = value;
}

/*!-----------------------------------------------------------------------------
*/
void CComUart::SetRxBufferSize(uint32 value)
{
	if(!_open)
		_rxBuffer->SetCapacity(value);
}

/*!-----------------------------------------------------------------------------
*/
void CComUart::SetTxBufferSize(uint32 value)
{
	if(!_open)
		_txBuffer->SetCapacity(value);
}

/*!-----------------------------------------------------------------------------
Function that is called to write the contents of a buffer onto the Serial Port
Data can only be written when the serial port is open.
If the buffer capacity is full, then this function will block, until UART
transmission frees further space in the buffer.
@param data The data byte to write
*/
void CComUart::WriteByte(uint8 data)
{
	//Only write data if the port is open
	if(_open) {
		//If the buffer is full, wait for it to empty. Do this in an interrupt
		//safe way, so the buffer cannot be accessed simultaneously
		volatile bool wait = true;
		while(wait) {
			IRQ_DISABLE;
			wait = _txBuffer->IsFull();
			IRQ_ENABLE;
			NOP;

			//### Perhaps need a Watchdog reset here!
		}

		IRQ_DISABLE;

		//Enable the transmitter hardware to start interrupt driven transmission
		//(Inheriting classes can override DoTxMode to disable the receiver
		//for half-duplex operation)
		this->DoTxMode(true);

		//Store the byte to the buffer
		_txBuffer->Push(data);

		IRQ_ENABLE;
	}
}

//==============================================================================
//Interrupt Handlers...
//==============================================================================
/*!-----------------------------------------------------------------------------
Function called from the Interrupt Vector Table.
*/
#if UART0_CONNECT_IRQ
void ISR_UART0_RX_TX(void) {
	if (CComUart::Uart[0])
		CComUart::Uart[0]->DoISR();
}
#endif

/*!-----------------------------------------------------------------------------
Function called from the Interrupt Vector Table.
*/
#if UART1_CONNECT_IRQ
void ISR_UART1_RX_TX(void) {
	if (CComUart::Uart[1])
		CComUart::Uart[1]->DoISR();
}
#endif

/*!-----------------------------------------------------------------------------
Function called from the Interrupt Vector Table.
*/
#if UART2_CONNECT_IRQ
void ISR_UART2_RX_TX(void) {
	if (CComUart::Uart[2])
		CComUart::Uart[2]->DoISR();
}
#endif

/*!-----------------------------------------------------------------------------
Function called from the Interrupt Vector Table.
*/
#if UART3_CONNECT_IRQ
void ISR_UART3_RX_TX(void) {
	if (CComUart::Uart[3])
		CComUart::Uart[3]->DoISR();
}
#endif

/*!-----------------------------------------------------------------------------
Function called from the Interrupt Vector Table.
*/
#if UART4_CONNECT_IRQ
void ISR_UART4_RX_TX(void) {
	if (CComUart::Uart[4])
		CComUart::Uart[4]->DoISR();
}
#endif

/*!-----------------------------------------------------------------------------
Function called from the Interrupt Vector Table.
*/
#if UART5_CONNECT_IRQ
void ISR_UART5_RX_TX(void) {
	if (CComUart::Uart[5])
		CComUart::Uart[5]->DoISR();
}
#endif

//==============================================================================
