#include "bitutils.hpp"

//==============================================================================
//Class Implementation...
//==============================================================================
//CBitUtils
//==============================================================================
/*!-----------------------------------------------------------------------------
Function that counts the number of One's in the specified value
@param value is the value to count the number of one's in
@returns the number of one's counted in the specified value
*/
uint8 CBitUtils::CountOnes(uint8 value)
{
	uint8 result;
	result = g_BitCntLut[value];
	return result;
}

/*!-----------------------------------------------------------------------------
Function that counts the number of One's in the specified value
@param value is the value to count the number of one's in
@returns the number of one's counted in the specified value
*/
uint8 CBitUtils::CountOnes(uint16 value)
{
	uint8 result;
	puint8 ptrVal = (puint8)&value;
	result = g_BitCntLut[*(ptrVal + 0)];
	result += g_BitCntLut[*(ptrVal + 1)];
	return result;
}

/*!-----------------------------------------------------------------------------
Function that counts the number of One's in the specified value
@param value is the value to count the number of one's in
@returns the number of one's counted in the specified value
*/
uint8 CBitUtils::CountOnes(uint32 value)
{
	uint8 result;
	puint8 ptrVal = (puint8)&value;
	result = g_BitCntLut[*(ptrVal + 0)];
	result += g_BitCntLut[*(ptrVal + 1)];
	result += g_BitCntLut[*(ptrVal + 2)];
	result += g_BitCntLut[*(ptrVal + 3)];
	return result;
}

/*!-----------------------------------------------------------------------------
Function that very quickly obtains a 32 bit mask for the specified number of bits,
without using a barrel shifter method
*/
uint32 CBitUtils::GetMask(uint8 bits)
{
	//Ensure that bits does not exceed 32
	bits &= (32 - 1);

	//Lookup the bitmask
	if(bits)
		return g_BitMaskLut[bits];
	else
		return 0;
};

//==============================================================================
//CBitFilter
//==============================================================================
/*!-----------------------------------------------------------------------------
*/
CBitFilter::CBitFilter(uint8 length)
{
	this->Clear();
	this->SetLength(length);	//Sets length and default thresholds, and updates the output
}

/*!-----------------------------------------------------------------------------
*/
bool CBitFilter::Add(bool bit)
{
	//Add the bit into the shift register
	_shiftReg <<= 1;
	if(bit)
		_shiftReg |= 1;

	//Update the filter and return the result
	return this->Update();
}

/*!-----------------------------------------------------------------------------
*/
void CBitFilter::Clear()
{
	_shiftReg = 0;
	_count = 0;
	_output = false;
}

/*!-----------------------------------------------------------------------------
*/
uint8 CBitFilter::GetCount()
{
	return _count;
}

/*!-----------------------------------------------------------------------------
*/
bool CBitFilter::GetOutput()
{
	return _output;
}

/*!-----------------------------------------------------------------------------
*/
void CBitFilter::SetLength(uint8 value)
{
	//Set and clip the number of bits in the filter
	_length = value;
	if(_length > 32)
		_length = 32;

	//Generate a mask for the shift register
	_mask = CBitUtils::GetMask(_length);

	//Set the thresholds for half way, with no hysteresis as default
	this->SetThreshold(_length / 2);

	//Recompute the output
	this->Update();
}

/*!-----------------------------------------------------------------------------
*/
void CBitFilter::SetThreshold(uint8 value, uint8 hysteresis)
{
	_thresholdLow = value - (hysteresis / 2);
	_thresholdHigh = _thresholdLow + hysteresis;
}

/*!-----------------------------------------------------------------------------
*/
void CBitFilter::SetThresholds(uint8 high, uint8 low)
{
	_thresholdHigh = high;
	_thresholdLow = low;
}

/*!-----------------------------------------------------------------------------
Compute the filter output based on the number of bits in the shift register
*/
bool CBitFilter::Update()
{
	//Mask the contents of the shift register to the required length
	uint32 bits = _mask & _shiftReg;

	//Count the number of bits in the masked shift register
	_count = CBitUtils::CountOnes(bits);

	//Determine the output based on the hysteresis threshold
	if(_count >= _thresholdHigh)
		_output = true;
	else if(_count <= _thresholdLow)
		_output = false;

	return _output;
}

//==============================================================================
