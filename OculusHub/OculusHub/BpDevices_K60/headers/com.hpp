/*==============================================================================
C++ Module that provides a base communication interface framework.

20/11/2014 - Created V1.0 of file
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef COM_HPP
#define COM_HPP

//Include system libraries
#include <string.h>		//For strlen function
#include <stdio.h>		//For vnsprintf function
#include <stdarg.h>		//For variable length arguments in functions

//Include class libraries
#include "common.h"
#include "conversion.hpp"

//Include the processor platform
#include "processor.h"

//==============================================================================
//Class Definition...
//==============================================================================
//Size of the buffer for PRINTF like commands
#define COM_PRINT_BUFFER	256


#define COM_PRINT(fmt, args...) \
{ \
	if(CCom::Terminal) { \
		CCom::Terminal->Print(fmt, ##args); \
	} \
}

#ifndef DEBUG
	#define DEBUG_PRINT(fmt, args...) {}
#else
	#define DEBUG_PRINT(fmt, args...) COM_PRINT(fmt, ##args)
#endif

//------------------------------------------------------------------------------
//Pre-declare the CCom class
class CCom;

/*! Define a pointer to a communications object */
typedef CCom* PCom;

/*! Abstract base class from which communication interface modules are derived */
class CCom {
	public:
		//Methods
		virtual void Clear(bool rx = true, bool tx = true) = 0;
		virtual void Close(void) = 0;
		virtual void Flush(void) = 0;
		virtual uint32 GetRxBufferCount() = 0;
		virtual uint32 GetTxBufferCount() = 0;
		virtual bool IsOpen(void) = 0;
		virtual bool Open(void) = 0;
		void Print(string str, ...);
		//void Print(const pchar str, ...);	//Deprecated in C99
		void PrintArgs(pchar str, va_list args);
		void Read(puint8 pBuf, uint32 count);
		virtual uint8 ReadByte() = 0;
		void Write(puint8 pBuf, uint32 count);
		virtual void WriteByte(uint8 data) = 0;
		inline void WriteHexUint8(uint8 data);
		inline void WriteHexUint16(uint16 data);
		inline void WriteHexUint32(uint32 data);
		inline void	WriteHexData(puint8 data, uint16 len);
		void WriteString(const string& value, bool newline = false);

		//Static Variables
		static PCom Terminal;						/*!< Global pointer to the Com object to act as the terminal */
};

//==============================================================================
#endif
