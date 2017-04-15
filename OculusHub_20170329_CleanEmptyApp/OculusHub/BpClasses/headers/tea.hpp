/*==============================================================================
C++ Module that provides the definitions and implementation for the implementation
of the "Tiny Encryption Algorithm" (TEA), and it modified versions.

For further details of the algorithm, refer to...
http://en.wikipedia.org/wiki/XXTEA
https://github.com/bluelovers/phprpc/blob/master/delphi/Source/XXTEA.pas

27/05/2014 - Created v1.0 of file
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef TEA_HPP
#define TEA_HPP

//Include system libraries

//Include common type definitions and macros
#include "common.h"

//==============================================================================
#define CXXTEA_DELTA		0x9e3779b9

/*! Implements a class providing XXTEA encryption and decryption functions.
http://en.wikipedia.org/wiki/XXTEA
https://github.com/bluelovers/phprpc/blob/master/delphi/Source/XXTEA.pas
*/
class CXxTea {
	public :
		//Static methods
		static void Decrypt(puint32 v, uint32 n, puint32 k);
		static void Encrypt(puint32 v, uint32 n, puint32 k);
};


//==============================================================================
#endif
