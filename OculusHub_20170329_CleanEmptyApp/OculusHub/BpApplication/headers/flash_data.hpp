/*==============================================================================
Module that provides definitions and implementations for a non-volatile data
storage system with self-levelling in FLASH memory.

2016-09-08 - v1.1, R.Sharphouse
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef FLASH_DATA_HPP
#define FLASH_DATA_HPP

//Include system libraries

//Include common type definitions and macros
#include "common.h"

//Include helper classes
#include "crc16.hpp"

//Include device drivers
#include "flash.hpp"

//==============================================================================
//General Definitions and Types
//==============================================================================
//------------------------------------------------------------------------------
/*! Enumeration defining the states a flash data record may be in.
The values are chosen to allow over-programming without erase in flash,
and must be 8-bytes in length! - one programming phrase. */
enum EFlashDataState {
	FDATA_STATE_FREE   = 0xFFFFFFFFFFFFFFFF,
	FDATA_STATE_ACTIVE = 0xFFFFFFFF00000000,
	FDATA_STATE_IGNORE = 0x0000000000000000
};

/*! Record that starts each data record in flash, indicating its state and its length
This record MUST be a multiple of 8-bytes in length, and programmed into flash on
an 8-byte boundary (for the ProgramPhrase command).
*/
#pragma pack(8)
struct TFlashDataHeader {
	EFlashDataState State;	/*! The programming state of the following data (must be 8 bytes long) */
	uint32 NextHeader;		/*! The address of the next header, or free programming location, on an 8-byte boundary - i.e. a linked list */
	uint16 Length;			/*! The number of bytes in the following data record */
	uint16 Checksum;		/*! The 16 bit CRC checksum of the following data record */
};
#pragma pack()

/*! Define a pointer to a FTFL data header */
typedef TFlashDataHeader* PFlashDataHeader;

/*! Enumeration specifying return codes from FlashData functions */
enum EFlashDataReturn {
	FDATA_OK = 0,
	FDATA_ERR_EMPTY = 1,
	FDATA_ERR_RANGE = 2,
	FDATA_ERR_CORRUPT = 3,
	FDATA_ERR_CHECKSUM = 4
};

//------------------------------------------------------------------------------
/*!
Class that forms the parent class for any class wishing to store data into program-flash
memory.
The class implements a simple file system, performing data levelling preventing
flash wear-out from repeated write cycles. It does this by allocating storage
space in multiples of sectors (the minimum unit of flash erasure), then writes
a header strut followed by the data into the flash. When the flash is changed,
the header is invalidated, and a new header+data block appended, until the flash
becomes full - at which point it is erased, and storage starts again.
*/
class CFlashData {
	private:
		/*! Enumeration specifying states of the Write FSM */
		enum EFlashDataWrState {
			WR_FIND_ACTIVE,
			WR_ERASE,
			WR_MODIFY,
			WR_INVALIDATE,
			WR_WRITE,
			WR_DONE
		};

	private:
		PFlash	 _flash;			//Pointer to the flash access class
		uint32	_storeAddr;			//Address where data storage allocation starts
		uint32	_storeSize;			//Length of data storage area, in bytes
		uint32	_readAddr;			//Address of where valid data can be read from, 0 for invalid data
		uint16	_readLength;		//The number of bytes valid data to be read occupies, 0 for invalid data

		//Private methods
		bool EraseCheck();
		EFlashDataReturn FindActiveRecord(puint32 dataAddr);
		EFlashDataReturn ReadFind();

	public:
		//Construction & Disposal
		CFlashData(PFlash flash, uint32 storeAddr, uint32 storeSize);
		~CFlashData();

		//Methods
		bool Erase();
		uint16 GetReadLength();
		uint16 Read(pointer destData, uint16 maxLength = 0);
		template <typename T> int32 ReadType(T* destData);
		bool Write(pointer srcData, uint16 length);
		template <typename T> bool WriteType(T* srcData);
};

/*! Define a pointer to a Flash Data manager class */
typedef CFlashData* PFlashData;

//==============================================================================
//Template Methods Implementation
//==============================================================================
/*!-----------------------------------------------------------------------------
Function that reads out the storage data record into the specified struct/storage
type.
If more bytes are available in the data store than will fit in the dest type, then
only the required number of bytes are copied, and a negative result indicates how many
wouldn't fit.
Any uninitialised bytes in the destData type are blanked to 0.
If no data is available, the functions returns 0, otherwise if more data is available
than can fit in the storage type, then a negative value (indicating bytes that couldn't fit)
is returned. Otherwise, the number of bytes read is returned.
@param	destData Pointer to the type where read data should be stored
@result The return code, or +ve number of bytes copied, or -ve number of bytes that couldn't fit.
*/
template <typename T>
int32 CFlashData::ReadType(T* destData)
{
	//Read out data and ensure it fits in the specified type
	uint16 readBytes = this->Read((pointer)destData, sizeof(T));

	//If the read type is smaller than available data, then return -1 to indicate
	//returned data, but size doesn't match.
	if(_readLength > sizeof(T)) {
		//Return a negative value for number of bytes extra available that wern't stored in struct
		return (sizeof(T) - _readLength);
	}
	else {
		//Return the number of bytes read
		return readBytes;
	}
}

/*!-----------------------------------------------------------------------------
Function that writes the specified type into the settings memory
@return True if the write occurred successfully, false if it failed.
*/
template <typename T>
bool CFlashData::WriteType(T* srcData)
{
	return this->Write((pointer)srcData, sizeof(T));
}

//==============================================================================
#endif
