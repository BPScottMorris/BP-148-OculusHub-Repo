#include "serialize.hpp"

//==============================================================================
//Class Implementation
//==============================================================================
/*!-----------------------------------------------------------------------------
As the specialisation of the Add<bool> template must be defined first,
Implement the function body for using it here after the definition.
*/
bool CSerialize::AddBool(bool value)
{
	return this->Add(value);
}

/*!-----------------------------------------------------------------------------
As the specialisation of the Read<bool> template must be defined first,
Implement the function body for using it here after the definition.
*/
bool CSerialize::ReadBool(bool* value)
{
	return this->Read(value);
}

//------------------------------------------------------------------------------
bool CSerialize::AddDouble(double value) { return this->Add(value); };
bool CSerialize::AddFloat(float value) { return this->Add(value); };
bool CSerialize::AddInt8(int8 value) { return this->Add(value); };
bool CSerialize::AddInt16(int16 value) { return this->Add(value); };
bool CSerialize::AddInt32(int32 value) { return this->Add(value); };
bool CSerialize::AddInt64(int64 value) { return this->Add(value); };
bool CSerialize::AddUint8(uint8 value) { return this->Add(value); };
bool CSerialize::AddUint16(uint16 value) { return this->Add(value); };
bool CSerialize::AddUint32(uint32 value) { return this->Add(value); };
bool CSerialize::AddUint64(uint64 value) { return this->Add(value); };

bool CSerialize::ReadBool(pbool value, bool defValue) { return this->Read(value, defValue); };
bool CSerialize::ReadDouble(pdouble value) { return this->Read(value); };
bool CSerialize::ReadDouble(pdouble value, double defValue) { return this->Read(value, defValue); };
bool CSerialize::ReadFloat(pfloat value) { return this->Read(value); };
bool CSerialize::ReadFloat(pfloat value, float defValue) { return this->Read(value, defValue); };
bool CSerialize::ReadInt8(pint8 value) { return this->Read(value); };
bool CSerialize::ReadInt8(pint8 value, int8 defValue) { return this->Read(value, defValue); };
bool CSerialize::ReadInt16(pint16 value) { return this->Read(value); };
bool CSerialize::ReadInt16(pint16 value, int16 defValue) { return this->Read(value, defValue); };
bool CSerialize::ReadInt32(pint32 value) { return this->Read(value); };
bool CSerialize::ReadInt32(pint32 value, int32 defValue) { return this->Read(value, defValue); };
bool CSerialize::ReadInt64(pint64 value) { return this->Read(value); };
bool CSerialize::ReadInt64(pint64 value, int64 defValue) { return this->Read(value, defValue); };
bool CSerialize::ReadUint8(puint8 value) { return this->Read(value); };
bool CSerialize::ReadUint8(puint8 value, uint8 defValue) { return this->Read(value, defValue); };
bool CSerialize::ReadUint16(puint16 value) { return this->Read(value); };
bool CSerialize::ReadUint16(puint16 value, uint16 defValue) { return this->Read(value, defValue); };
bool CSerialize::ReadUint32(puint32 value) { return this->Read(value); };
bool CSerialize::ReadUint32(puint32 value, uint32 defValue) { return this->Read(value, defValue); };
bool CSerialize::ReadUint64(puint64 value) { return this->Read(value); };
bool CSerialize::ReadUint64(puint64 value, uint64 defValue) { return this->Read(value, defValue); };

/*!-----------------------------------------------------------------------------
Adds a stream of null-terminated characters representing the string object.
@param maxChars specifies the maximum number of characters (excluding the Null Terminator) that can be written. A value of zero allows any length
*/
bool CSerialize::AddStringZ(const string& value, uint16 maxChars)
{
	puint8 buf = (puint8)value.c_str();
	return this->AddStringZ(buf, maxChars);
}

/*!-----------------------------------------------------------------------------
Adds a stream of null-terminated characters representing the buffer
@param maxChars specifies the maximum number of characters (excluding the Null Terminator) that can be written. A value of zero allows any length
*/
bool CSerialize::AddStringZ(puint8 buf, uint16 maxChars)
{
	while(true) {
		uint8 ch = *buf;

		//Abort if the character is a null
		if(!ch)
			break;

		//Add the character to the buffer
		this->AddUint8(ch);
		buf++;

		//If MaxLen is bigger than 0, then decrease it and abort at 0
		if(maxChars > 0) {
			maxChars--;
			if(maxChars == 0)
				break;
		}
	}

	//Add the null terminator
	this->AddUint8(0);
}

/*!-----------------------------------------------------------------------------
@param maxChars specifies the maximum number of characters (excluding the Null Terminator) that can be read into the buffer. A value of zero allows any length
*/
bool CSerialize::ReadStringZ(string& value, uint16 maxChars)
{
	uint8 ch;
	bool store = true;

	while(true) {
		bool success = this->ReadUint8(&ch, 0);

		if(!success) {
			//End of serialise buffer reached, abort
			break;
		}
		else if(ch) {
			//A valid character, so store

			if(maxChars > 0) {
				maxChars--;
				store = (maxChars > 0);
			}
			if(store) {
				value.append((char*)&ch, 1);
			}
		}
		else {
			//A null terminator so abort
			break;
		}
	}
}

/*!-----------------------------------------------------------------------------
@param maxChars specifies the maximum number of characters (excluding the Null Terminator) that can be read into the buffer. A value of zero allows any length
*/
bool CSerialize::ReadStringZ(puint8 buf, uint16 maxChars)
{
	uint8 ch;
	bool store = true;

	while(true) {
		bool success = this->ReadUint8(&ch, 0);

		if(!success) {
			//End of serialise buffer reached, abort
			break;
		}
		else if(ch) {
			//A valid character, so store
			if(maxChars > 0) {
				maxChars--;
				store = (maxChars > 0);
			}
			if(store) {
				*buf = ch;
				buf++;
			}
		}
		else {
			//A null terminator so abort
			break;
		}
	}
}

//==============================================================================
