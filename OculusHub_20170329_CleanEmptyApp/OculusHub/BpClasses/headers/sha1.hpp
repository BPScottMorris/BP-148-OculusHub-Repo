/*==============================================================================
C++ Module that provides the definitions and implementation for an SHA1 hashing
algorithm

01/05/2014 - Created v1.0 of file based on from https://code.google.com/p/smallsha1/
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef SHA1_HPP
#define SHA1_HPP

//Include system libraries

//Include common type definitions and macros
#include "common.h"

/*! Define a type representing an array for the 20 byte (160 bit) Hash result */
//typedef Uint8[20] TSha1Hash;

//typedef PUint8 PSha1Hash;


class CSha1 {
	private:
		static inline void ClearW(puint32 w);
		static void InnerHash(puint32 result, puint32 w);
		static inline uint32 RotateLeft(const uint32 value, const uint32 steps);
	
	public:
		static void Calc(puint8 src, uint32 bytes, puint8 hash);	
		static void ToHexString(puint8 hash, pchar hexStr);
};

//==============================================================================
#endif
