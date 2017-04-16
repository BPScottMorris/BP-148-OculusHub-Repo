#include "tea.hpp"

//==============================================================================
//Class Implementation...
//==============================================================================
//CXxTea
//==============================================================================
#define CXXTEA_MX ((((z >> 5) ^ (y << 2)) + ((y >> 3) ^ (z << 4))) ^ ((sum ^ y) + (k[(p & 3) ^ e] ^ z)))

/*!-----------------------------------------------------------------------------
Function that decrypts an array of Uint32 values in-situe.
@param v	Pointer to the data to decrypt. Data is decrypted in-situ.
@param n	Number of UInt32's to decrypt
@param k	Pointer to an array of 4 x UInt32's describing the 128-bit key to use for decryption.
*/
void CXxTea::Decrypt(puint32 v, uint32 n, puint32 k)
{
	uint32 z, y, sum, e, p, q;

	if(n < 1) return;

	y = v[0];
	q = 6 + (52 / n);
	sum = q * CXXTEA_DELTA;

	while(sum != 0) {
		e = (sum >> 2) & 3;
		for(p = n - 1; p > 0; p--) {
			z = v[p - 1];
			v[p] -= CXXTEA_MX;
			y = v[p];
		}
		z = v[n - 1];
		v[0] -= CXXTEA_MX;
		y = v[0];
		sum -= CXXTEA_DELTA;
	}
}

/*!-----------------------------------------------------------------------------
Function that encrypts an array of Uint32 value in-situe.
@param v	Pointer to the data to encrypt. Data is encrypted in-situ.
@param n	Number of UInt32's to encrypt
@param k	Pointer to an array of 4 x UInt32's describing the 128-bit key to use for encryption.
*/
void CXxTea::Encrypt(puint32 v, uint32 n, puint32 k)
{
	uint32 z, y, sum, e, p, q;

	if(n < 1) return;

	z = v[n - 1];
	y = v[0];
	sum = 0;
	q = 6 + (52 / n);

	do {
		sum += CXXTEA_DELTA;
		e = (sum >> 2) & 3;
		for(p = 0; p < (n - 1); p++) {
			y = v[p + 1];
			v[p] += CXXTEA_MX;
			z = v[p];
		}
		y = v[0];
		v[p] += CXXTEA_MX;
		z = v[p];
		q--;
	} while(q > 0);
}

//------------------------------------------------------------------------------
//Remove macro definitions
#undef CXXTEA_MX

//==============================================================================
