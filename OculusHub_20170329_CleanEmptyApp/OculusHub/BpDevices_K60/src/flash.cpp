#include "flash.hpp"

#include "com.hpp"

//==============================================================================
//Class Implementation...
//==============================================================================
//CFlash
//==============================================================================
/*!-----------------------------------------------------------------------------
*/
CFlash::CFlash()
{
	//Initialise the pointer to the flash object
	_fmc = FMC;
	_flash = FTFE;

	//Indicate the config section of flash is locked to prevent programming access
	_cfgLock = true;

	//Disable Data Cache in Flash memory Controller Module
	CLR_BITS(_fmc->PFB01CR, FMC_PFB01CR_B01DCE_MASK);
    CLR_BITS(_fmc->PFB23CR, FMC_PFB23CR_B23DCE_MASK);
	//CLR_BITS(_fmc->PFB0CR, FMC_PFB0CR_B0DCE_MASK);
	//CLR_BITS(_fmc->PFB1CR, FMC_PFB1CR_B1DCE_MASK);
}

/*!-----------------------------------------------------------------------------
*/
CFlash::~CFlash()
{

}

/*!-----------------------------------------------------------------------------
Function that checks a range of addresses are valid and lie within the allowable
limits of what can be programmed.
The 'addrStart' parameter is modified to make it a suitable FTFL internal memory address
@param addrStart	The starting address of the range
@param addrRange	The number of address locations in the range (must be >= 1)
@result True if the address range is valid, otherwise false.
*/
bool CFlash::CheckAddress(uint32& addrStart, uint32 addrRange)
{
	//Check for target addresses
    //if((addrStart < FLASH_BASE) || ((addrStart + addrRange - 1) >= FLASH_SIZE)) {
	if((addrStart + addrRange - 1) >= FLASH_SIZE) {
		//Return an error code FTFx_ERR_RANGE
		return false;
    }
    else {
        //Convert System memory address to FTFx internal memory address
        //addrStart -= FLASH_BASE;
		return true;
    }
}

/*!-----------------------------------------------------------------------------
Function that writes the configuration struct into the Flash configuration
registers located at address FLASH_CNFG_START_ADDRESS.
NB: Before programming, the flash sector (or 4kb) must first be erased to 1's.
*/
EFlashReturn CFlash::ConfigProgram(PFlashConfig cfg)
{
	EFlashReturn returnCode;

	//Allow our programmer to write to config memory
	bool tmp = _cfgLock;
	_cfgLock = false;

	//Write the config code
	returnCode = this->FlashProgram(FLASH_CNFG_START_ADDRESS, (puint8)cfg, FLASH_CNFG_SIZE, NULL);

	//Restore our lock status
	_cfgLock = tmp;

	return returnCode;
}

/*!-----------------------------------------------------------------------------
Function that reads the contents of the flash configuration registers
into the specified flash structure.
*/
void CFlash::ConfigRead(PFlashConfig cfg)
{
	//Create a pointer to the flash source
	PFlashConfig src = (PFlashConfig)FLASH_CNFG_START_ADDRESS;
	//Copy the flash into the config structure
	*cfg = *src;
}

/*!-----------------------------------------------------------------------------
Function that prints a string representing the specified return code
*/
void CFlash::DebugReturnCode(EFlashReturn returnCode)
{
    switch(returnCode) {
		case FLASH_OK : { COM_PRINT("No Error: OK\r\n"); break; }
		case FLASH_ERR_SIZE : { COM_PRINT("Error: ERR_SIZE \r\n"); break; }
		case FLASH_ERR_ADDR : { COM_PRINT("Error: ERR_ADDR \r\n"); break; }
		case FLASH_ERR_RANGE : { COM_PRINT("Error: ERR_RANGE \r\n"); break; }
		case FLASH_ERR_ACCERR : { COM_PRINT("Error: ERR_ACCERR \r\n"); break; }
		case FLASH_ERR_PVIOL : { COM_PRINT("Error: ERR_PVIOL \r\n"); break; }
		case FLASH_ERR_MGSTAT0 : { COM_PRINT("Error: ERR_MGSTAT0 \r\n"); break; }
		case FLASH_ERR_PARAM : { COM_PRINT("Error: ERR_PARAM \r\n"); break; }
		default : { COM_PRINT("Unknown Error.\r\n"); break; }
	}

	/*
	if(failedReason == OK)
		printf("No Error: OK \r\n");
	else {
		if(IS_BITS_SET(failedReason, ERR_SIZE))
			printf("Error: ERR_SIZE \r\n");
		if(IS_BITS_SET(failedReason, ERR_RANGE))
			printf("Error: ERR_RANGE \r\n");
		if(IS_BITS_SET(failedReason, ERR_ACCERR))
			printf("Error: ERR_ACCERR \r\n");
		if(IS_BITS_SET(failedReason, ERR_PVIOL))
			printf("Error: ERR_PVIOL \r\n");
		if(IS_BITS_SET(failedReason, ERR_MGSTAT0))
			printf("Error: ERR_MGSTAT0 \r\n");
		if(IS_BITS_SET(failedReason, ERR_CHANGEPROT))
			printf("Error: ERR_CHANGEPROT \r\n");
		if(IS_BITS_SET(failedReason, ERR_EEESIZE))
			printf("Error: ERR_EEESIZE \r\n");
		if(IS_BITS_SET(failedReason, ERR_EFLASHSIZE))
			printf("Error: ERR_EFLASHSIZE \r\n");
		if(IS_BITS_SET(failedReason, ERR_ADDR))
			printf("Error: ERR_ADDR \r\n");
		if(IS_BITS_SET(failedReason, ERR_NOEEE))
			printf("Error: ERR_NOEEE \r\n");
		if(IS_BITS_SET(failedReason, ERR_EFLASHONLY))
			printf("Error: ERR_EFLASHONLY \r\n");
		if(IS_BITS_SET(failedReason, ERR_DFLASHONLY))
			printf("Error: ERR_DFLASHONLY \r\n");
		if(IS_BITS_SET(failedReason, ERR_RDCOLERR))
			printf("Error: ERR_RDCOLERR \r\n");
		if(IS_BITS_SET(failedReason, ERR_RAMRDY))
			printf("Error: ERR_RAMRDY \r\n");
		if(IS_BITS_SET(failedReason, ERR_MARGIN))
			printf("Error: ERR_MARGIN \r\n");
	}
	*/
}

/*!-----------------------------------------------------------------------------
Function that is called to execute a Flash command sequence.
NB: This routine MUST be placed in RAM to prevent Flash memory contentions.
@param cmdSize	The number of bytes defining the command and its parameters
@param cmdData	Pointer to an array of bytes defining the command and its parameters to execute
@result The return status of the command after execution.
*/
//
EFlashReturn CFlash::ExecuteCmd(uint8 cmdSize, puint8 cmdData)
{
    uint8 fstat;
	uint8 i;

	//Check CCIF bit of the flash status register is set, indicating no command in progress
	while(IS_BITS_CLR(_flash->FSTAT, FTFE_FSTAT_CCIF_MASK)) {};

    //Do NOT clear RDCOLERR & ACCERR & FPVIOL flags here, this seems to cause
	//problems with timing that either hang the program, disconnect the debugger
	//or cause the programming to fail, unless single-stepped through!
	//SET_BITS(_flash->FSTAT, (uint8)(FTFE_FSTAT_RDCOLERR_MASK | FTFE_FSTAT_ACCERR_MASK | FTFE_FSTAT_FPVIOL_MASK));

    //Load FCCOB registers
    for(i = 0; i < cmdSize; i++) {
        switch(i) {
			case 0: { _flash->FCCOB0 = cmdData[i]; break; }
			case 1: { _flash->FCCOB1 = cmdData[i]; break; }
			case 2: { _flash->FCCOB2 = cmdData[i]; break; }
			case 3: { _flash->FCCOB3 = cmdData[i]; break; }
			case 4: { _flash->FCCOB4 = cmdData[i]; break; }
			case 5: { _flash->FCCOB5 = cmdData[i]; break; }
			case 6: { _flash->FCCOB6 = cmdData[i]; break; }
			case 7: { _flash->FCCOB7 = cmdData[i]; break; }
			case 8: { _flash->FCCOB8 = cmdData[i]; break; }
			case 9: { _flash->FCCOB9 = cmdData[i]; break; }
			case 10: { _flash->FCCOBA = cmdData[i]; break; }
			case 11: { _flash->FCCOBB = cmdData[i]; break; }
            default: break;
        }
    }

    IRQ_DISABLE;

	//Perform the programming
	CFlash::ExecuteCmdRam(_flash);

	IRQ_ENABLE;

    //Check for errors
	fstat = _flash->FSTAT;
    if(IS_BIT_SET(fstat, FTFE_FSTAT_ACCERR_SHIFT)) {
        //An access error occurred
        return FLASH_ERR_ACCERR;
    }
    else if(IS_BIT_SET(fstat, FTFE_FSTAT_FPVIOL_SHIFT)) {
        //A protection error occurred
        return FLASH_ERR_PVIOL;
    }
    else if(IS_BIT_SET(fstat, FTFE_FSTAT_MGSTAT0_SHIFT)) {
        //A MGSTAT0 non-correctable error occurred
        return FLASH_ERR_MGSTAT0;
    }
	else {
		//Return the return code
		return FLASH_OK;
	}
}

/*!-----------------------------------------------------------------------------
Function that is programmed into RAM, and called to actually execute the
Flash programming command.
This is a static function to save on RAM usage.
*/
FUNC_RAM_OPTIMIZE_OFF CFlash::ExecuteCmdRam(FLASH_Type* flash)
{
	//Launch command (and clear down error flags)
	//SET_BITS(flash->FSTAT, FTFE_FSTAT_CCIF_MASK);
	SET_BITS(flash->FSTAT, FTFE_FSTAT_CCIF_MASK | FTFE_FSTAT_RDCOLERR_MASK | FTFE_FSTAT_ACCERR_MASK | FTFE_FSTAT_FPVIOL_MASK);

	//Wait for command completion
	while(IS_BITS_CLR(flash->FSTAT, FTFE_FSTAT_CCIF_MASK)) {} ;
}

/*!-----------------------------------------------------------------------------
Function that returns the status of the config-lock flag, that prevents
the config part of flash (Address 0x00000400 to 0x0000040F) from being
programmed, affecting the security settings
*/
bool CFlash::GetConfigLock()
{
	return _cfgLock;
}

/*!-----------------------------------------------------------------------------
Function that returns which is the active program flash black that is locative
at relative address 0x0000.
Using the program flash SWAP command will change this.
*/
uint8 CFlash::GetFlashActiveBlock()
{
	if(IS_BIT_SET(_flash->FCNFG, FTFE_FCNFG_SWAP_SHIFT))
		return 1;
	else
		return 0;
}

/*!-----------------------------------------------------------------------------
Function that attempts to ulock the device using the backdoor access key
@param key Pointer to 8-bytes of the backdoor access key
*/
EFlashReturn CFlash::FlashBackdoor(puint8 key)
{
	uint8 cmdData[12];

	//Prepare parameters to execute the command
	cmdData[0] = FLASH_CMD_SECURITY_BY_PASS;
	cmdData[1] = 0;	//Unused
	cmdData[2] = 0;	//Unused
	cmdData[3] = 0;	//Unused

	//Copy across the key bytes
	for(uint8 idx = 0; idx < 8; idx++) {
		cmdData[4 + idx] = *key;
		key++;
	}

	//Execute the command
	return this->ExecuteCmd(12, cmdData);
}

/*!-----------------------------------------------------------------------------
The Program Check command tests a previously programmed program flash location
to see if it reads correctly at the specified margin level.
This function will accept any non-aligned destination address.
@param destAddr		The address from which to read data, can be any address
@param verifyData	Pointer to where the validating data to check against should be found
@param size			The number of bytes to check
@param marginLevel	The margin level to test at
@param failAddr		Optional pointer to where the address of the failing locaiton should be stored if found (NULL if not required)
@result Success or error code from the operation
*/
EFlashReturn CFlash::FlashCheck(uint32 destAddr, puint8 verifyData, uint32 size, EFlashReadMargin marginLevel, puint32 failAddr)
{
	uint8 destOffset;
	uint8 i;
	uint8 buf[FLASH_PGMCHK_ALIGN_SIZE];
	puint8 destPtr;
	EFlashReturn returnCode;

	//Return a OK if no bytes are specified
	if(size == 0)
		return FLASH_OK;

	//Check target addresses lie within memory
	if(!this->CheckAddress(destAddr, size))
		return FLASH_ERR_RANGE;

	while(size > 0) {
		//Determine if the destAddr lies on an 4-byte boundry, or how far away it lies
		destOffset = destAddr % FLASH_PGMCHK_ALIGN_SIZE;

		if((destOffset > 0) || (size < FLASH_PGMCHK_ALIGN_SIZE)) {
			//Handle the starting and ending condition, where...
			//a) DestAddress doesn't lie on an 4-byte boundry (starting case)
			//b) Size is less than 4-bytes (ending, or if less than 4 bytes are to be programmed)

			//Ensure we have a long-word address, by masking the lower address bits to zero
			CLR_BITS(destAddr, (FLASH_PGMCHK_ALIGN_SIZE - 1));

			//Read in the current destinations bytes existing contents
			destPtr = (puint8)destAddr;
			for(i = 0; i < FLASH_PGMCHK_ALIGN_SIZE; i++) {
				if(i < destOffset) {
					//Copy any existing destination bytes prior to the new bytes
					buf[i] = *destPtr;
				}
				else if(size > 0) {
					//Copy in verify bytes to the buffer
					buf[i] = *verifyData;
					verifyData++;
					size--;
				}
				else {
					//Fill remaing bytes with existing destinaiton if size is less than FLASH_PGMCHK_ALIGN_SIZE
					buf[i] = *destPtr;
				}
				destPtr++;
			}

			//Check the Flash against the buffered long word
			returnCode = this->FlashCheckLWord(destAddr, buf, marginLevel);
		}
		else {
			//Check the Flash long word sequence
			returnCode = this->FlashCheckLWord(destAddr, verifyData, marginLevel);

			//Update variables
			verifyData += FLASH_PGMCHK_ALIGN_SIZE;
			size -= FLASH_PGMCHK_ALIGN_SIZE;
		}

		//Abort if an error occured
		if(returnCode != FLASH_OK) {
			//If assigned, store the failing Flash Address
			if(failAddr)
				*failAddr = destAddr;
			//Return the error code
			return returnCode;
		}

		//Update the destination address
		destAddr += FLASH_PGMCHK_ALIGN_SIZE;
	}

	//Return sccuess
	return FLASH_OK;
}

/*!-----------------------------------------------------------------------------
The Program Check command tests a previously programmed program flash location (long word)
to see if it reads correctly at the specified margin level.
@param destAddr		The address from which to read data, must be long word aligned
@param verifyData	Pointer to where the validating data to check against should be found
@param size			The number of bytes to check
@param marginLevel	The margin level to test at
@result Success or error code from the operation
*/
EFlashReturn CFlash::FlashCheckLWord(uint32 destAddr, puint8 verifyData, EFlashReadMargin marginLevel)
{
	uint8 cmdData[12];

	//Ensure we have a LongWord address, by masking the lower address bits to zero
	CLR_BITS(destAddr, (FLASH_PGMCHK_ALIGN_SIZE - 1));

	//Prepare parameters to execute the command
	cmdData[0] = FLASH_CMD_PROGRAM_CHECK;
	cmdData[1] = (uint8)((destAddr >> 16) & 0xFF);
	cmdData[2] = (uint8)((destAddr >> 8) & 0xFF);
	cmdData[3] = (uint8)(destAddr & 0xFF);
	cmdData[4] = (uint8)marginLevel;
	//For Little Endian access
	cmdData[8] = *(verifyData + 3);
	cmdData[9] = *(verifyData + 2);
	cmdData[10] = *(verifyData + 1);
	cmdData[11] = *(verifyData + 0);

	//Execute the command
	return this->ExecuteCmd(12, cmdData);
}

/*!-----------------------------------------------------------------------------
Function that dumps the specified area of Flash to the PrintF command
*/
void CFlash::FlashDump(uint32 addr, uint32 bytes, uint8 rowLen)
{
	puint8 p = (puint8)addr;
	uint8 r = 0;
	for(uint32 idx = 0; idx < bytes; idx++) {
		if(r == 0) {
			COM_PRINT("Addr %.8X: ", addr + idx);
		}
		else if((r % 4) == 0) {
			COM_PRINT("    ");
		}
		else {
			COM_PRINT(" ");
		}
		uint8 data = *p;
		COM_PRINT("%.2x", data);
		p++;
		r++;
		if(r >= rowLen) {
			printf("\r\n");
			r = 0;
		}
	}
}

/*!-----------------------------------------------------------------------------
This function issues an "EraseAll" Flash command.
The Erase All Blocks operation erases program flash memory, the program flash
IFR containing the swap indicator address, data flash memory, data flash IFR
space, EEPROM backup memory, and FlexRAM, then verifies that all are erased.
@param confirm As a safety check, write the walue 0xAA55CC33 to this to start the command
*/
EFlashReturn CFlash::FlashEraseAll(uint32 confirm)
{
	EFlashReturn returnCode;
	uint8 cmdData[1];

	if(confirm != FLASH_ERASE_SAFETY)
		return FLASH_ERR_PARAM;

	//Preparing command paramters to erase a flash block
    cmdData[0] = FLASH_CMD_ERASE_ALL_BLOCK;

    //Calling flash command sequence function to execute the command
    returnCode = this->ExecuteCmd(1, cmdData);

	//This line should never be reached, as the program should crash before here!!!
	//while(true) {};

	return returnCode;
}

/*!-----------------------------------------------------------------------------
The Erase Flash Block operation erases all addresses in a single program flash or data
flash block that contains the specified address.
@param addr The address in the program flash block to erase (to 1's)
@result Success or error code from the operation
*/
EFlashReturn CFlash::FlashEraseBlock(uint32 addr)
{
	uint8 cmdData[4];

	//Ensure we have a block address, by masking the lower address bits to zero
	CLR_BITS(addr, (FLASH_BLOCK_SIZE - 1));

    //Preparing command paramters to erase a flash block
    cmdData[0] = FLASH_CMD_ERASE_BLOCK;
    cmdData[1] = (uint8)((addr >> 16) & 0xFF);
    cmdData[2] = (uint8)((addr >> 8) & 0xFF);
    cmdData[3] = (uint8)(addr & 0xFF);

    //Calling flash command sequence function to execute the command
    return this->ExecuteCmd(4, cmdData);
}

/*!-----------------------------------------------------------------------------
Function that erases a the specified number of addresses from the starting
address with the following caviats...
Addr must lie on a 4kb (4096 bytes) address and Size must be a multiple of 4096.
The lower bits of these values are masked to 0's so will always be rounded down
*/
EFlashReturn CFlash::FlashEraseRange(uint32 addr, uint32 size)
{
	EFlashReturn returnCode;

	//Ensure we have a sector address, by masking the lower address bits to zero
	CLR_BITS(addr, (FLASH_SECTOR_SIZE - 1));

	//Ensure the size is a multiple of sectors by masking the lower address bits to zero
	CLR_BITS(size, (FLASH_SECTOR_SIZE - 1));

	//Check target addresses lie within memory
	if(!this->CheckAddress(addr, size))
		return FLASH_ERR_RANGE;

	//Erase sectors
	while(size > 0) {
		//Erase the sector
		returnCode = this->FlashEraseSector(addr);
		if(returnCode != FLASH_OK) {
			return returnCode;
		}

		//Move to the next sector
		addr += FLASH_SECTOR_SIZE;

		//Decrease bytes remaining
		size -= FLASH_SECTOR_SIZE;
	}

	//Return success;
	return FLASH_OK;
}

/*!-----------------------------------------------------------------------------
Function that erases a single program flash sector (i.e. 4kb chunk) back to all 1's.
@param addr	Address at the start of the sector to erase.
@result Success or error code from the operation
*/
EFlashReturn CFlash::FlashEraseSector(uint32 addr)
{
	uint8 cmdData[4];

	//Ensure we have a sector address, by masking the lower address bits to zero
	CLR_BITS(addr, (FLASH_SECTOR_SIZE - 1));

	//Preparing passing parameter to erase a flash block
	cmdData[0] = FLASH_CMD_ERASE_SECTOR;
	cmdData[1] = (uint8)((addr >> 16) & 0xFF);
	cmdData[2] = (uint8)((addr >> 8) & 0xFF);
	cmdData[3] = (uint8)(addr & 0xFF);

	//Calling flash command sequence function to execute the command
	return this->ExecuteCmd(4, cmdData);
}

/*!-----------------------------------------------------------------------------
Function that erases a sequential range of program flash sectors (i.e. 4kb chunks)
back to all 1's.
@param addr	Address at the start of the sector to erase.
@param sectors The number of sequential sectors to erase
@result Success or error code from the operation
*/
EFlashReturn CFlash::FlashEraseSectors(uint32 addr, uint16 sectors)
{
	return this->FlashEraseRange(addr, sectors * FLASH_SECTOR_SIZE);
}

/*!-----------------------------------------------------------------------------
Function that programs any number of bytes from any address to any address.
The function internally takes care of word/phrase boundries, and lengths length
than 8-byte multiples.
NB: Flash can only be programmed from a all 1's state to a value, attempts
to reprogram already programmed non 1's areas will fail.
The flash program mechanism includes a self verify, and will flag an error (MGSTAT)
if an attempt to program an already programmed byte is made.
@param destAddr		Any address in Flash where programming should start (does NOT have to be phrase aligned)
@param srcData		Pointer to the data to program into the memory location
@param size			The number of bytes to program
@param failAddr		Optional address where a verify fail error address should be stored. Use NULL if not required
@result The return code indicating the success of the operation.
*/
EFlashReturn CFlash::FlashProgram(uint32 destAddr, puint8 srcData, uint32 size, puint32 failAddr)
{
	uint8 destOffset;
	uint8 i;
	uint8 buf[FLASH_PHRASE_SIZE];
	puint8 destPtr;
	EFlashReturn returnCode;

	//Return a OK if no bytes are specified
	if(size == 0)
		return FLASH_OK;

	//Check target addresses lie within memory
	if(!this->CheckAddress(destAddr, size))
		return FLASH_ERR_RANGE;

	while(size > 0) {
		//Determine if the destAddr lies on an 8-byte boundry, or how far away it lies
		destOffset = destAddr % FLASH_PHRASE_SIZE;

		if((destOffset > 0) || (size < FLASH_PHRASE_SIZE)) {
			//Handle the starting and ending condition, where...
			//a) DestAddress doesn't lie on an 8-byte boundry (starting case)
			//b) Size is less than 8-bytes (ending, or if less than 8 bytes are to be programmed)

			//Ensure we have a Phrase address, by masking the lower address bits to zero
			CLR_BITS(destAddr, (FLASH_PHRASE_SIZE - 1));

			//Read in the current destinations bytes existing contents
			destPtr = (puint8)destAddr;
			for(i = 0; i < FLASH_PHRASE_SIZE; i++) {
				if(i < destOffset) {
					//Copy any existing destination bytes prior to the new bytes
					buf[i] = *destPtr;
				}
				else if(size > 0) {
					//Copy in new bytes to program to the destination
					buf[i] = *srcData;
					srcData++;
					size--;
				}
				else {
					//Fill remaing bytes with existing destinaiton if size is less than FLASH_PHRASE_SIZE
					buf[i] = *destPtr;
				}
				destPtr++;
			}

			//Program the phrase
			returnCode = this->FlashProgramPhrase(destAddr, buf);
		}
		else {
			//Program the next 8-byte sequence
			returnCode = this->FlashProgramPhrase(destAddr, srcData);

			//Update variables
			srcData += FLASH_PHRASE_SIZE;
			size -= FLASH_PHRASE_SIZE;
		}

		//Abort if an error occured
		if(returnCode != FLASH_OK) {
			//If assigned, store the failing Flash Address
			if(failAddr)
				*failAddr = destAddr;
			//Return the error code
			return returnCode;
		}

		//Update the destination address
		destAddr += FLASH_PHRASE_SIZE;
	}

	//Return sccuess
	return FLASH_OK;
}

/*!-----------------------------------------------------------------------------
Function that programs a Phrase (8 bytes - 64 bits) into the specified memory location.
NB: The bytes to be programmed in memory must have been erased first, and the address
MUST lie on an 8-byte boundry (and will be forced to do so through masking).
The flash program mechanism includes a self verify, and will flag an error (MGSTAT)
if an attempt to program an already programmed byte is made.
@param destAddr		An 8-byte (Phrase) aligned address in Flash where programming should start
@param srcData		Pointer to the data to program into the memory location
@result The return code indicating the success of the operation.
*/
EFlashReturn CFlash::FlashProgramPhrase(uint32 destAddr, puint8 srcData)
{
	uint8 cmdData[12];

	//Ensure we have a Phrase address, by masking the lower address bits to zero
	CLR_BITS(destAddr, (FLASH_PHRASE_SIZE - 1));

	//Check target addresses lie within memory
	if(!this->CheckAddress(destAddr, 8))
		return FLASH_ERR_RANGE;

	//Abort if we're trying to program the CONFIG area of flash, with the security settings in.
	//We should not change this as it may lock the device.
	if(_cfgLock && (destAddr >= FLASH_CNFG_START_ADDRESS) && (destAddr <= FLASH_CNFG_END_ADDRESS)) {
		//Return OK, as we don't want to abort any block programming operations.
		return FLASH_OK;
	}

	//Prepare the ProgramLongword command to send
	cmdData[0] = FLASH_CMD_PROGRAM_PHRASE;
	cmdData[1] = (uint8)((destAddr >> 16) & 0xFF);
	cmdData[2] = (uint8)((destAddr >> 8) & 0xFF);
	cmdData[3] = (uint8)(destAddr & 0xFF);

	//For Little Endian access
	//First Word
	cmdData[4] = *(srcData + 3);
	cmdData[5] = *(srcData + 2);
	cmdData[6] = *(srcData + 1);
	cmdData[7] = *(srcData + 0);
	//Second Word
	cmdData[8] = *(srcData + 7);
	cmdData[9] = *(srcData + 6);
	cmdData[10] = *(srcData + 5);
	cmdData[11] = *(srcData + 4);

	//Execute the ProgramPhrase command
	return this->ExecuteCmd(12, cmdData);
}
/*!-----------------------------------------------------------------------------
The Verify (Read 1s) Block command checks to see if an entire program flash or data flash block
has been erased to the specified margin level.
@result Success or error code from the operation
*/
EFlashReturn CFlash::FlashVerifyBlock(uint32 addr, EFlashReadMargin marginLevel)
{
	uint8 cmdData[5];

    //Ensure we have a block address, by masking the lower address bits to zero
	CLR_BITS(addr, (FLASH_BLOCK_SIZE - 1));

    //Prepare parameters to verify block command
    cmdData[0] = FLASH_CMD_VERIFY_BLOCK;
    cmdData[1] = (uint8)((addr >> 16) & 0xFF);
    cmdData[2] = (uint8)((addr >> 8) & 0xFF);
    cmdData[3] = (uint8)(addr & 0xFF);
    cmdData[4] = (uint8)marginLevel;

    //Calling flash command sequence function to execute the command
    return this->ExecuteCmd(5, cmdData);
}

/*!-----------------------------------------------------------------------------
Function that verifies the erase of a single program flash sector was correctly
performed at the specified margin level, and is ready for programming.
@param addr	Address at the start of the sector to verify.
@param marginLevel The margin level to verify the sector contents at.
@result Success or error code from the operation
*/
EFlashReturn CFlash::FlashVerifySector(uint32 addr, EFlashReadMargin marginLevel)
{
	return this->FlashVerifySectors(addr, 1, marginLevel);
}

/*!-----------------------------------------------------------------------------
Function that verifies the erase of a multiple program flash sectors was correctly
performed at the specified margin level, and is ready for programming.
@param addr	Address at the start of the sector to verify.
@param sectors The number of flash sectors to check
@param marginLevel The margin level to verify the sector contents at.
@result Success or error code from the operation
*/
EFlashReturn CFlash::FlashVerifySectors(uint32 addr, uint16 sectors, EFlashReadMargin marginLevel)
{
	uint8 cmdData[7];

	//Calcualte number of 128 byte sections to check for single flash sector
	uint16 size = (FLASH_SECTOR_SIZE / 128) * sectors;

	//Ensure we have a sector address, by masking the lower address bits to zero
	CLR_BITS(addr, (FLASH_SECTOR_SIZE - 1));

	//Preparing passing parameter to erase a flash block
	cmdData[0] = FLASH_CMD_VERIFY_SECTION;
	cmdData[1] = (uint8)((addr >> 16) & 0xFF);
	cmdData[2] = (uint8)((addr >> 8) & 0xFF);
	cmdData[3] = (uint8)(addr & 0xFF);
	cmdData[4] = (uint8)((size >> 8) & 0xFF);
	cmdData[5] = (uint8)(size & 0xFF);
	cmdData[6] = (uint8)marginLevel;

	//Calling flash command sequence function to execute the command
	return this->ExecuteCmd(7, cmdData);
}

/*!-----------------------------------------------------------------------------
Function that sets the status of the config-lock flag, that prevents
the config part of flash (Address 0x00000400 to 0x0000040F) from being
programmed, affecting the security settings
*/
void CFlash::SetConfigLock(bool value)
{
	_cfgLock = value;
}


/*!-----------------------------------------------------------------------------
Function that issues a Swap command.

EFlashReturn CFlash::CmdSwap(uint32 flashAddr)
{
    EFlashReturn returnCode;
    //UINT8 currentSwapMode = 0xFF;
	TFlashSwapState swapState;
	TFlashSwapParams params;
	params.Continue = true;

    //Set the default return code as FTFx_OK
    returnCode = OK;

    //Check if the destination is not Longword aligned
    if((flashAddr % FLASH_SWAP_ALIGN_SIZE) != 0) {
        //Return error code FTFx_ERR_ADDR
        return ERR_ADDR;
    }

	//Check if the flash address lies in the Active block (P-Flash block 0)
	if((flashAddr < FLASH_BASE) || (flashAddr >= (FLASH_SIZE + FLASH_HALF_SIZE))) {
        //Return error code FTFx_ERR_RANGE
        return ERR_RANGE;
    }

	//Check if the flash address is in the P-Flash block 0 but is in Flash Configuration Field
    if((flashAddr >= FLASH_CNFG_START_ADDRESS) && (flashAddr <= FLASH_CNFG_END_ADDRESS)) {
        //Return error code FTFx_ERR_RANGE
        return ERR_RANGE;
    }

	//Convert System memory address to FTFx internal memory address
    flashAddr -= FLASH_BASE;

    //Report current swap state
	returnCode = this->CmdSwapGetStatus(flashAddr, &swapState);
	if(returnCode == OK) {
		params.State = swapState.CurrentState;
		OnSwapStatus.Call(&params);
	}
    if((returnCode != OK) || !params.Continue) {
		return returnCode;
    }

    //If current swap mode is Uninitialized
    if(swapState.CurrentState == SWAP_UNINIT) {
        //Initialize Swap to Initialized/READY state
		returnCode = this->CmdSwapExecute(flashAddr, SWAP_SET_INDICATOR_ADDR);
        if(returnCode != OK) {
            return returnCode;
        }

        //Wait for state transition from Uninitialized to expected state
		while((EFlashSwapState)_flash->FCCOB5 == SWAP_UNINIT) {};

        //Report current swap state
		//Should be READY.
		returnCode = this->CmdSwapGetStatus(flashAddr, &swapState);
		if(returnCode == OK) {
			params.State = swapState.CurrentState;
			OnSwapStatus.Call(&params);
		}
		if((returnCode != OK) || !params.Continue) {
			return returnCode;
		}
    }

    //If current swap mode is Initialized/Ready
    if(swapState.CurrentState == SWAP_READY) {
        //Progress Swap to UPDATE state
		returnCode = this->CmdSwapExecute(flashAddr, SWAP_SET_IN_PREPARE);
        if(returnCode != OK) {
            return returnCode;
        }

        //Wait for state transition from Initialized/Ready to others
		while((EFlashSwapState)_flash->FCCOB5 == SWAP_READY) {};

        //Report current swap state
		//Should be UPDATE or UPDATE_ERASED.
		returnCode = this->CmdSwapGetStatus(flashAddr, &swapState);
		if(returnCode == OK) {
			params.State = swapState.CurrentState;
			OnSwapStatus.Call(&params);
		}
		if((returnCode != OK) || !params.Continue) {
			return returnCode;
		}
    }

    //If current swap mode is Update or Update-Erased
    if((swapState.CurrentState == SWAP_UPDATE) || (swapState.CurrentState == SWAP_UPDATE_ERASED)) {

		//If the current swap mode is not update-erase, erase swap indicator
		//in non-active block by erasing the sector contains the swap indicator
        if(swapState.CurrentState != SWAP_UPDATE_ERASED) {
			//Erase the swap indicator in the non-active block
			uint32 eraseAddr = flashAddr + FLASH_HALF_SIZE;
			returnCode = this->CmdEraseSector(eraseAddr, FLASH_SECTOR_SIZE);
            if(returnCode != OK) {
				return returnCode;
            }

            //Now the swap state must be Update-Erased, so report current swap state
			//Should be UPDATE_ERASED.
			returnCode = this->CmdSwapGetStatus(flashAddr, &swapState);
			if(returnCode == OK) {
				params.State = swapState.CurrentState;
				OnSwapStatus.Call(&params);
			}
			if((returnCode != OK) || !params.Continue) {
				return returnCode;
			}
        }

        //Progress Swap to COMPLETE State
		returnCode = this->CmdSwapExecute(flashAddr, SWAP_SET_IN_COMPLETE);
        if(returnCode != OK) {
        	return returnCode;
        }

        //Wait for state transition from Update-Erased to Complete
		while((EFlashSwapState)_flash->FCCOB5 != SWAP_COMPLETE) {};

		//Report current swap state
		//Should be COMPLETE.
		returnCode = this->CmdSwapGetStatus(flashAddr, &swapState);
		if(returnCode == OK) {
			params.State = swapState.CurrentState;
			OnSwapStatus.Call(&params);
		}
		if((returnCode != OK) || !params.Continue) {
			return returnCode;
		}
    }

    return returnCode;
}
*/

/*!-----------------------------------------------------------------------------
Function that issues a Swap command.

EFlashReturn CFlash::CmdSwapExecute(uint32 addr, EFlashSwapCmd swapCmd)
{
    EFlashReturn returnCode;
	uint8 cmdData[8];

    //Prepare command
    cmdData[0] = FLASH_CMD_PFLASH_SWAP;
    cmdData[1] = (uint8)((addr >> 16) & 0xFF);
    cmdData[2] = (uint8)((addr >> 8) & 0xFF);
    cmdData[3] = (uint8)(addr & 0xFF);
    cmdData[4] = (uint8)swapCmd;
    cmdData[5] = 0xFF;
    cmdData[6] = 0xFF;
    cmdData[7] = 0xFF;

    //Call flash command sequence function to execute the command
    returnCode = this->ExecuteCmd(8, cmdData);

	return returnCode;
}
*/

/*!-----------------------------------------------------------------------------
Function that issues a Swap command to report on the current swap status, which
is populated into the status parameter.

EFlashReturn CFlash::CmdSwapGetStatus(uint32 flashAddr, PFlashSwapState status)
{
    EFlashReturn returnCode;

    //Set the default return code as FTFx_OK
    returnCode = OK;

    //Check if the destination is not Longword aligned
    if((flashAddr % FLASH_SWAP_ALIGN_SIZE) != 0) {
        //Return error code FTFx_ERR_ADDR
        return ERR_ADDR;
    }

    //Check if the flash address lies in the Active block (P-Flash block 0)
	if((flashAddr < FLASH_BASE) || (flashAddr >= (FLASH_SIZE + FLASH_HALF_SIZE))) {
        //Return error code FTFx_ERR_RANGE
        return ERR_RANGE;
    }

	//Check if the flash address is in the P-Flash block 0 but is in Flash Configuration Field
    if((flashAddr >= FLASH_CNFG_START_ADDRESS) && (flashAddr <= FLASH_CNFG_END_ADDRESS)) {
        //Return error code FTFx_ERR_RANGE
        return ERR_RANGE;
    }

	//Convert System memory address to FTFx internal memory address
    flashAddr -= FLASH_BASE;

	//Issue a Swap "Report Status" sub command
	returnCode = this->CmdSwapExecute(flashAddr, SWAP_REPORT_STATUS);

    //If the command executed, read out the returned status codes
    if(returnCode == OK) {
		status->CurrentState = (EFlashSwapState)_flash->FCCOB5;
		status->CurrentBlock = (EFlashSwapBlockStatus)_flash->FCCOB6;
		status->NextBlock = (EFlashSwapBlockStatus)_flash->FCCOB7;
    }

    return returnCode;
}
*/

//==============================================================================
