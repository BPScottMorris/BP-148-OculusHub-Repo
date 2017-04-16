#include "com.hpp"

//==============================================================================
//Implementation...
//==============================================================================
//CCom
//==============================================================================
/*! Pointer to the object to handle terminal IO (PrintF) */
PCom CCom::Terminal = NULL;

/*!-----------------------------------------------------------------------------
Function that implements a PrintF like capability to write text to the uart
output
*/
void CCom::Print(string str, ...)
{
	//Initialise the arguments list
	va_list args;
	va_start(args, str);
	//Call the print function for the arguments list
	this->PrintArgs((pchar)str.c_str(), args);
	//Tidy up
	va_end(args);
}

/*!-----------------------------------------------------------------------------
Function that implements a PrintF like capability to write text to the uart
output

void CCom::Print(const pchar str, ...)
{
	//Initialise the arguments list
	va_list args;
	va_start(args, str);
	//Call the print function for the arguments list
	this->Print(str, args);
	//Tidy up
	va_end(args);
}
*/

/*!-----------------------------------------------------------------------------
Function that implements a PrintF like capability to write text to the uart
output
@param str Pointer to a null-terminated Format string of characters
@param args The variable args structure containing variables to insert in the format string
*/
void CCom::PrintArgs(pchar str, va_list args)
{
	if(this->IsOpen()) {
		//Allocate a buffer for output characters
		char buf[COM_PRINT_BUFFER];
		//Convert the variable-args and string into a character buffer
		vsnprintf(buf, COM_PRINT_BUFFER, str, args);
		//Get the length of the string to send
		uint32 len = strlen(buf);
		//Write the character buffer to the Uart
		this->Write((puint8)buf, len);
	}
}

/*!-----------------------------------------------------------------------------
Function that reads a specified number of bytes from the receive buffer. If
the required number of bytes arn't present, the function will block until they
have been received
@param pBuf Pointer to the buffer where the received bytes should be copied
@param count The number of bytes to read
*/
void CCom::Read(puint8 pBuf, uint32 count)
{
	//Don't allow data to be read if the port is closed
	if(!this->IsOpen())
		return;

	//Read the requested number of bytes
	for(uint32 i = 0; i < count; i++) {
		*pBuf = this->ReadByte();
		pBuf++;
	}
}

/*!-----------------------------------------------------------------------------
Function that is called to write the contents of a buffer onto the Serial Port
Data can only be written when the serial port is open.
If the buffer capacity is full, then this function will block, until UART
transmission frees further space in the buffer.
@param pBuf Pointer to the start of the buffer to get data from
@param count The number of bytes to write
*/
void CCom::Write(puint8 pBuf, uint32 count)
{
	//Only write data if the port is open
	if(this->IsOpen()) {
		//Write the buffer contents into the transmitter
		for(uint32 i = 0; i < count; i++) {
			this->WriteByte(*pBuf);
			pBuf++;
		}
	}
}

/*!-----------------------------------------------------------------------------
Funciton that write a Uint8 value out of the command processor as an Ascii-hex
encoded character pair.
Most significant nibble is written first
*/
void CCom::WriteHexUint8(uint8 data)
{
	this->WriteByte(CConversion::GetHexChar(data >> 4));
	this->WriteByte(CConversion::GetHexChar(data));
}

/*!-----------------------------------------------------------------------------
Function that write a Uint16 value out of the command processor as an Ascii-hex
encoded characters.
Least significant byte is written first
*/
void CCom::WriteHexUint16(uint16 data)
{
	puint8 pdata = (puint8)&data;
	this->WriteHexUint8(*pdata);
	this->WriteHexUint8(*(pdata + 1));
}

/*!-----------------------------------------------------------------------------
Function that write a Uint32 value out of the command processor as an Ascii-hex
encoded characters.
Least significant byte is written first
*/
void CCom::WriteHexUint32(uint32 data)
{
	puint8 pdata = (puint8)&data;
	this->WriteHexUint8(*pdata);
	this->WriteHexUint8(*(pdata + 1));
	this->WriteHexUint8(*(pdata + 2));
	this->WriteHexUint8(*(pdata + 3));
}

/*!-----------------------------------------------------------------------------
*/
void CCom::WriteHexData(puint8 data, uint16 len)
{
	for(uint16 idx = 0; idx < len; idx++) {
		this->WriteHexUint8(*data);
		data++;
	}
}

/*!-----------------------------------------------------------------------------
Function that writes the contents of the specified string to the UART
Does NOT output a null-terminator at the end of the string
@param value Reference to the string to write
@param newline True if a CR/LF char pair should sent after the string.
*/
void CCom::WriteString(const string& value, bool newline)
{
	uint32 len = value.length();
	const char* p = value.c_str();

	//Only write data if the port is open
	if(this->IsOpen()) {
		//Write the buffer contents into the transmitter
		for(uint32 i = 0; i < len; i++) {
			this->WriteByte(*p);
			p++;
		}
		//If required, write a CR/LF character after the string
		if(newline) {
			this->WriteByte(13);
			this->WriteByte(10);
		}
	}
}

//==============================================================================
