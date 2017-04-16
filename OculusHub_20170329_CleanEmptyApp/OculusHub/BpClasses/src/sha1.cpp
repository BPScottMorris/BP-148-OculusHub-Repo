#include "sha1.hpp"

//==============================================================================
//Class Implementation...
//==============================================================================
/*!-----------------------------------------------------------------------------
Rotate an integer value to left.
*/
uint32 CSha1::RotateLeft(const uint32 value, const uint32 steps)
{
	return ((value << steps) | (value >> (32 - steps)));
}

/*!-----------------------------------------------------------------------------
Sets the first 16 integers in the buffer to zero.
Used for clearing the W buffer.
*/
void CSha1::ClearW(puint32 w)
{
	for(uint32 idx = 0; idx < 16; idx++) {
		w[idx] = 0;
	}
}

/*!-----------------------------------------------------------------------------
*/
void CSha1::InnerHash(puint32 result, puint32 w)
{
	uint32 a = result[0];
	uint32 b = result[1];
	uint32 c = result[2];
	uint32 d = result[3];
	uint32 e = result[4];

	int32 round = 0;

	#define sha1macro(func, val) \
	{ \
		const uint32 t = CSha1::RotateLeft(a, 5) + (func) + e + val + w[round]; \
		e = d; \
		d = c; \
		c = CSha1::RotateLeft(b, 30); \
		b = a; \
		a = t; \
	}

	while(round < 16) {
		sha1macro((b & c) | (~b & d), 0x5a827999)
		++round;
	}
	while(round < 20) {
		w[round] = CSha1::RotateLeft((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
		sha1macro((b & c) | (~b & d), 0x5a827999)
		++round;
	}
	while(round < 40) {
		w[round] = CSha1::RotateLeft((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
		sha1macro(b ^ c ^ d, 0x6ed9eba1)
		++round;
	}
	while(round < 60) {
		w[round] = CSha1::RotateLeft((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
		sha1macro((b & c) | (b & d) | (c & d), 0x8f1bbcdc)
		++round;
	}
	while(round < 80) {
		w[round] = CSha1::RotateLeft((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
		sha1macro(b ^ c ^ d, 0xca62c1d6)
		++round;
	}

	#undef sha1macro

	result[0] += a;
	result[1] += b;
	result[2] += c;
	result[3] += d;
	result[4] += e;
}

/*!-----------------------------------------------------------------------------
@param src Pointer to the source data to hash
@param bytes The number of bytes to run through the hash function.
@param hash Pointer to a 20-byte array where the resultant hash is stored
*/
void CSha1::Calc(puint8 src, uint32 bytes, puint8 hash)
{
	//Init the result array.
	uint32 result[5] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };

	//The reusable round buffer
	uint32 w[80];
	uint32 wIdx;
	uint32 wByte;
	uint32 wShift;
	int64 bits;

	//Loop through all complete 64 byte blocks.
	uint32 remaining = bytes;
	while(remaining >= 64) {
		//Break each 512 bit block into sixteen 32-bit big-endian words w[i], 0 <= i <= 15
		for(wIdx = 0; wIdx < 16; wIdx++) {
			uint32 wVal;
			wVal = ((uint32)*src) << 24; src++;
			wVal |= ((uint32)*src) << 16; src++;
			wVal |= ((uint32)*src) << 8; src++;
			wVal |= ((uint32)*src) << 0; src++;
			w[wIdx] = wVal;
		}

		//Hash the block
		CSha1::InnerHash(result, w);

		//Decrease bytes remaining by the block size.
		remaining -= 64;
	}

	//Handle the last and not full 64 byte block if existing.
	//i.e. we should have 15 or less bytes at this point
	CSha1::ClearW(w);
	wByte = 0;
	while(remaining > 0) {
		//Make 32-bit big-endian words from remaining bytes
		wIdx = wByte / 4;
		wShift = (3 - (wByte % 4)) * 8;
		w[wIdx] |= ((uint32)*src) << wShift;
		remaining--;
		src++;
		wByte++;
	}

	//Append the bit '1' to the message, i.e. by adding 0x80 for 8-bit characters
	wIdx = wByte / 4;
	wShift = (3 - (wByte % 4)) * 8;
	w[wIdx] |= (0x80 << wShift);
	wByte++;

	//If we havn't room in the w buffer of 512-bits to append the 64-bit number of
	//bits encoded then hash the current block
	if(wByte >= 56) {
		CSha1::InnerHash(result, w);
		CSha1::ClearW(w);
	}

	//Finally append the number of bits as a big-endian integer
	bits = ((int64)bytes) * 8;
	w[14] = (uint32)(bits >> 32);
	w[15] = (uint32)(bits & 0xFFFFFFFF);
	CSha1::InnerHash(result, w);

	//Store hash result pointer, and make sure we get in in the correct endian order.
	for(wByte = 0; wByte < 20; wByte++) {
		wIdx = wByte / 4;
		wShift = (3 - (wByte % 4)) * 8;
		hash[wByte] = (result[wIdx] >> wShift) & 0xFF;
	};
}

/*!-----------------------------------------------------------------------------
Function that reads a has array and compiles a hexadecimal array of characters
representing its value.
@param hash	Pointer to a 20-byte array of bytes containing the hash
@param hexStr Pointer to where the array of characters should be written, this should be 41 bytes (for 20 hex char pairs and a null terminator).
*/
void CSha1::ToHexString(puint8 hash, pchar hexStr)
{
	const char hexDigits[] = { "0123456789abcdef" };

	for (int hashByte = 20; --hashByte >= 0;) {
		hexStr[hashByte << 1] = hexDigits[(hash[hashByte] >> 4) & 0xf];
		hexStr[(hashByte << 1) + 1] = hexDigits[hash[hashByte] & 0xf];
	}
	//Append a null terminator
	hexStr[40] = 0;
}

//==============================================================================
