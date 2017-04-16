#include "flash_data.hpp"

//==============================================================================
//Class Implementation...
//==============================================================================
//CFlashData
//==============================================================================
/*!-----------------------------------------------------------------------------
Constructor
@param flash		Pointer to the flash manager object
@param storeAddr	The flash memory address where data storage is allowed. This will be rounded down to the nearest sector address
@param storeSize	The number of bytes allocated for data storage (this will be rounded up to the nearest sector size)
*/
CFlashData::CFlashData(PFlash flash, uint32 storeAddr, uint32 storeSize)
{
	//Store the storage details
	_flash = flash;
	_storeAddr = storeAddr;
	_storeSize = storeSize;

	//Ensure we have a sector address, by masking the lower address bits to zero
	CLR_BITS(_storeAddr, (FLASH_SECTOR_SIZE - 1));

	//Ensure storage size if a multiple of sector length, but round up to the
	//nearest sector.
	CLR_BITS(_storeSize, (FLASH_SECTOR_SIZE - 1));
	if((storeSize % FLASH_SECTOR_SIZE) > 0)
		_storeSize += FLASH_SECTOR_SIZE;
	if(_storeSize == 0)
		_storeSize = FLASH_SECTOR_SIZE;

	//Find if we have valid data stored, ready for reading
	EFlashDataReturn returnCode = this->ReadFind();

	//If we have no valid read data record, then check to see if the data storage
	//area is fully erased.
	if(returnCode != FDATA_OK) {
		//Check if the storage area is blank
		bool blank = this->EraseCheck();

		if(!blank) {
			//If not fully erased, then erase it now so its ready and formatted for new writes.
			this->Erase();
		}
	}
}

/*!-----------------------------------------------------------------------------
Destructor
*/
CFlashData::~CFlashData()
{
}

/*!-----------------------------------------------------------------------------
Function that erases all data in the storage area, and reset everything back
to defaults
*/
bool CFlashData::Erase()
{
	//Reset internal params
	_readAddr = 0;
	_readLength = 0;

	//Erase the flash sectors for storage
	EFlashReturn returnCode = _flash->FlashEraseSectors(_storeAddr, (_storeSize / FLASH_SECTOR_SIZE));
	return (returnCode == FLASH_OK);
}

/*!-----------------------------------------------------------------------------
Function that determines if the data storage area is blank and ready for programming
@result True if the storage area read's as all 1's, ready for programming
*/
bool CFlashData::EraseCheck()
{
	/*
	bool blank = true;
	puint8 p = (puint8)_storeAddr;

	for(uint32 i = 0; i < _storeSize; i++) {
		if(*p != 0xFF)
			return false;
		p++;
	}
	return true;
	*/

	//Perform a blank verify check on the storage area
	EFlashReturn returnCode = _flash->FlashVerifySectors(_storeAddr, (_storeSize / FLASH_SECTOR_SIZE), FLASH_MARGIN_NORMAL);
	return (returnCode == FLASH_OK);
}

/*!-----------------------------------------------------------------------------
Function that finds the start of the active data record within the allocated
storage memory.
This procedure traverses the data storage area like a Linked list, looking for the
first Active data record in encounders.
@param[out] dataAddr Pointer to where the address of the active data (or free) record should be stored.
@result The address of the starting data record header for the active (or free) location
*/
EFlashDataReturn CFlashData::FindActiveRecord(puint32 dataAddr)
{
	uint32 addr, addrEnd;
	//puint32 state;
	PFlashDataHeader header;

	//Set the address to the start of storage
	addr = _storeAddr;
	addrEnd = _storeAddr + _storeSize;

	//Set the initial contents of the data address result to 0
	*dataAddr = 0;

	//Start loop traversing memory
	while(addr < addrEnd) {
		//Setup a pointer to the current header struct being pointed at...
		header = (PFlashDataHeader)addr;

		if(header->State == FDATA_STATE_FREE) {
			//If the header indicates this data record is FREE (unprogrammed), then
			//we have no programmed data record, so return with the free address an error
			*dataAddr = addr;
			return FDATA_ERR_EMPTY;
		}
		else if(header->State == FDATA_STATE_ACTIVE) {
			//If we have an active data block, then check that data lies within
			//the valid storage area
			uint32 dataStart = addr + sizeof(TFlashDataHeader);
			uint32 dataEnd = dataStart + header->Length;
			uint16 csum = CCrc16::Calc((puint8)dataStart, 0, header->Length);

			if(dataEnd >= addrEnd) {
				//Data lies outside the valid storage area, so return an error
				return FDATA_ERR_RANGE;
			}
			else if(csum != header->Checksum) {
				//The stored checksum does not match the data, so fail
				return FDATA_ERR_CHECKSUM;
			}
			else {
				//Data is valid and lies within the allowed storage area so return the address
				*dataAddr = addr;
				return FDATA_OK;
			}
		}
		else if(header->State == FDATA_STATE_IGNORE) {
			//If the header indicates this is an old record, now obsoleted, and should
			//be ignored, then skip over it.
			//addr += sizeof(TFlashDataHeader);
			//addr += header->Length;
			addr = header->NextHeader;
		}
		else {
			//If the header state is unknown, then we have corrupt data, so abort
			return FDATA_ERR_CORRUPT;
		}
	}

	//If we reach here, the current address lies outside allowed storage, so return an error
	return FDATA_ERR_RANGE;
}

/*!-----------------------------------------------------------------------------
Function that returns the length of the active data record available for reading.
@result The length of available data, Zero indicates that data is not available.
*/
uint16 CFlashData::GetReadLength()
{
	return _readLength;
}

/*!-----------------------------------------------------------------------------
Function that copies the current active data record into a user specified memory
location.
@param dest	Pointer to where the read data should be stored to.
@param maxLength Optional maximum number of bytes to copy, or 0 for all bytes
@result The number of bytes read, 0 indicates a fail
*/
uint16 CFlashData::Read(pointer destData, uint16 maxLength)
{
	puint8 pSrc, pDest;
	uint16 length;
	uint16 cnt;

	pSrc = (puint8)_readAddr;
	pDest = (puint8)destData;

	if((_readLength == 0) || (_readAddr == 0)) {
		//If max length isn't 0, then blank this number of bytes at the dest Data
		for(uint16 i = 0; i < maxLength; i++) {
			*pDest = 0;
			pDest++;
		}

		//Return fail if no data is available to read.
		return 0;
	}
	else {
		//If auto is specified, set the number of bytes to read
		if(maxLength == 0)
			maxLength = _readLength;

		//Check we're reading a valid number of bytes
		if(maxLength > _readLength)
			length = _readLength;
		else
			length = maxLength;

		//Copy the data out
		cnt = 0;
		for(uint16 i = 0; i < length; i++) {
			*pDest = *pSrc;
			pDest++;
			pSrc++;
			cnt++;
		}

		//If maxLength specified more bytes than are available, fill 0's
		//into the remaining destData bytes
		for(uint16 i = length; i < maxLength; i++) {
			*pDest = 0;
			pDest++;
		}

		//Return the number of copied bytes
		return cnt;
	}
}

/*!-----------------------------------------------------------------------------
Function that finds the start of the valid data block within the allocated storage
memory that can be read.
This procedure traverses the data storage area like a Linked list, looking for the
first Active data record in encounters, then sets up the classes read pointers.
*/
EFlashDataReturn CFlashData::ReadFind()
{
	uint32 dataAddr;
	PFlashDataHeader header;
	EFlashDataReturn returnCode;

	//Attempt to find the active data record within the flash storage area
	returnCode = this->FindActiveRecord(&dataAddr);

	if(returnCode == FDATA_OK) {
		//We have an active record, so setup pointers
		header = (PFlashDataHeader)dataAddr;
		_readAddr = dataAddr + sizeof(TFlashDataHeader);
		_readLength = header->Length;
	}
	else {
		//Indicate we have no valid data record
		_readAddr = 0;
		_readLength = 0;
	}

	//Return the status code
	return returnCode;
}

/*!-----------------------------------------------------------------------------
Function that writes a new data record into the next free locations of the data
storage area, and marks any previous ACTIVE data record as INACTIVE, so it
will be ignored on the next read.
If the data storage area becomes full, it is erased, and the new record written
at its start.
This function uses a state machine to control the operations.
*/
bool CFlashData::Write(pointer srcData, uint16 length)
{
	//Initialise variables.
	uint32 writeAddr = 0;
	bool writeStateEn = true;
	EFlashDataWrState writeState = WR_FIND_ACTIVE;
	bool writeSuccess = false;
	uint8 eraseCnt = 0;

	//Abort if the write length exceeds the storage size
	if((length + sizeof(TFlashDataHeader)) > _storeSize) {
		return false;
	}

	//Build a state machine to perform the write
	while(writeStateEn) {
		switch(writeState) {
			case WR_FIND_ACTIVE : {
				//Traverse the flash storage area to find any current active record
				EFlashDataReturn returnCode = this->FindActiveRecord(&writeAddr);

				if(returnCode == FDATA_OK) {
					//If we have an active record, then determine if it needs modification
					writeState = WR_MODIFY;
				}
				else if(returnCode == FDATA_ERR_EMPTY) {
					//If we have an un-programmed area, then just write into it
					writeState = WR_WRITE;
				}
				else {
					//For all other errors, we should erase the flash and try again
					writeState = WR_ERASE;
				}
				break;
			}

			case WR_ERASE : {
				//In this state, erase the flash storage
				bool success;

				//If we've not exceed the erase limit, then erase
				if(eraseCnt < 5) {
					success = this->Erase();
					eraseCnt++;
				}
				else
					success = false;

				if(success) {
					//If data was erased, then write the data to the start of storage
					//Reset writeAddr to the start of storage
					writeAddr = _storeAddr;
					writeState = WR_WRITE;
				}
				else {
					//If erase failed (or attempt limit exceeded), then we're screwed and can't write data, so abort with failure
					writeStateEn = false;	//Prevent further FSM execution
					writeSuccess = false;	//Set return code for failure.
					writeState = WR_DONE;
				}
				break;
			}

			case WR_MODIFY : {
				//Determine if the current settings need modifying - if they're
				//the same then don't do anything, and save some flash wear.
				bool match;
				PFlashDataHeader header;
				uint16 csum;
				puint8 psrc, pflash;

				//Get a pointer to the header structure
				header = (PFlashDataHeader)writeAddr;

				//Check the lengths match
				match = (length == header->Length);

				//Check the checksums match
				csum = CCrc16::Calc((puint8)srcData, 0, length);
				match &= (csum == header->Checksum);

				//Check the contents match
				psrc = (puint8)srcData;
				pflash = (puint8)(writeAddr + sizeof(TFlashDataHeader));
				for(uint16 i = 0; i < length; i++) {
					match &= (*psrc == *pflash);
					psrc++;
					pflash++;
				}

				if(match) {
					//Settings are the same, so don't change them.
					//Exit with success
					writeStateEn = false;	//Prevent further FSM execution
					writeSuccess = true;	//Set return code for success.
					writeState = WR_DONE;
				}
				else {
					//Settings are different, so start the update procedure
					writeState = WR_INVALIDATE;
				}

				break;
			}

			case WR_INVALIDATE : {
				//We have a current active record that's now obsolete, so mark it as invalid
				EFlashReturn flashCode;
				TFlashDataHeader header;

				//Read the current header from the writeAddr - this must be 8-byte aligned
				header = *((PFlashDataHeader)writeAddr);
				header.State = FDATA_STATE_IGNORE;

				//Over program the first phrase (8-bytes) with the INVALID code
				flashCode = _flash->FlashProgramPhrase(writeAddr, (puint8)&header);

				if(flashCode == FLASH_OK) {
					//If the record was marked as invalid, then proceed to program the new record
					//Move the write pointer to the new location
					//writeAddr += (sizeof(TFlashDataHeader) + header.Length);
					writeAddr = header.NextHeader;
					writeState = WR_WRITE;
				}
				else {
					//If over-programming failed, then we've no choice but to erase
					//the flash storage, and try again.
					writeState = WR_ERASE;
				}
				break;
			}

			case WR_WRITE : {
				//We now are ready to program a new data record, and writeAddr should
				//point to where the header should be written.
				EFlashReturn flashCode;
				TFlashDataHeader header;
				uint16 blockLength;
				uint32 writeAddrEnd;

				//If write exceeds storage length, then erase and start again
				writeAddrEnd = writeAddr + sizeof(TFlashDataHeader) + length;
				if(writeAddrEnd >= (_storeAddr + _storeSize)) {
					//Store a range error, so the code below fails, and causes an erase cycle
					flashCode = FLASH_ERR_RANGE;
				}
				else {
					//Compute the record block length (length rounded up to the nearest 8 bytes)
					blockLength = length & ~(0x7);	//Mask the lower 3 bits to 0
					if((length % 8) > 0)
						blockLength += 8;

					//Make up a new header
					header.State = FDATA_STATE_ACTIVE;
					header.NextHeader = writeAddr + sizeof(TFlashDataHeader) + blockLength;
					header.Length = length;
					header.Checksum = CCrc16::Calc((puint8)srcData, 0, length);

					//Program the header
					flashCode = _flash->FlashProgram(writeAddr, (puint8)&header, sizeof(TFlashDataHeader), NULL);

					if(flashCode == FLASH_OK) {
						//Header programming ok, so program data
						writeAddr += sizeof(TFlashDataHeader);
						flashCode = _flash->FlashProgram(writeAddr, (puint8)srcData, length, NULL);
					}
				}

				if(flashCode == FLASH_OK) {
					//If data programmed OK, then setup the read pointers to the new data
					_readAddr = writeAddr;
					_readLength = length;

					//Exit with success
					writeStateEn = false;	//Prevent further FSM execution
					writeSuccess = true;	//Set return code for success.
					writeState = WR_DONE;
				}
				else {
					//If header programming fails, then erase flash and try again
					writeState = WR_ERASE;
				}

				break;
			}

			case WR_DONE : {
				//This should never run, but is a catch state for the done condition.
				writeStateEn = false;	//Prevent further FSM execution
				break;
			}

			default : {
				//For error conditions, start the process.
				writeState = WR_FIND_ACTIVE;
				break;
			}
		}
	}

	//Return the success code
	return writeSuccess;
}

//==============================================================================
