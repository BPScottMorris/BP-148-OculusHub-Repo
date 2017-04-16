/*==============================================================================
C++ Module that provides the definitions and implementation functions that
provide quick bit manipulation functions

26/08/2016 - Created v1.0, R.Sharphouse

==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef BITUTILS_HPP
#define BITUTILS_HPP

//Include system libraries

//Include common type definitions and macros
#include "common.h"

//==============================================================================
//==============================================================================
#ifdef __cplusplus
extern "C" {
#endif

/*! Define a flash-based look up table for counting the number of One's in an 8 bit word */
const uint8 g_BitCntLut[256] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

/*! Define a flash based look up table for working out a 32-bit mask */
const uint32 g_BitMaskLut[32] = {
	0x00000001, 0x00000003, 0x00000007, 0x0000000F,
	0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF,
	0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
	0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF,
	0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
	0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF,
	0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF,
	0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
};

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
/*!
Class of static helper functions for manipulating acoustic messages and packets
*/
class CBitUtils {
	private:
		//Declare static variables

	public:
		//Static methods
		static uint8 CountOnes(uint8 value);
		static uint8 CountOnes(uint16 value);
		static uint8 CountOnes(uint32 value);
		static uint32 GetMask(uint8 bits);
};

//------------------------------------------------------------------------------
/*!
Class that bits a Bit Filter - comprising a shift register, bit counter and
thresholded output with hysteresis
*/
class CBitFilter {
	private:
		uint8 _count;
		uint8 _length;
		uint32 _mask;
		bool _output;
		uint8 _thresholdHigh;
		uint8 _thresholdLow;
		uint32 _shiftReg;

		bool Update();

	public:
		CBitFilter(uint8 length = 32);

		//Methods
		bool Add(bool bit);
		void Clear();
		uint8 GetCount();
		bool GetOutput();
		void SetLength(uint8 value);
		void SetThreshold(uint8 value, uint8 hysteresis = 0);
		void SetThresholds(uint8 high, uint8 low);
};

typedef CBitFilter* PBitFilter;

//==============================================================================
#endif
