#include "flash_prog.hpp"

//==============================================================================
//Class Implementation...
//==============================================================================
//CFlashProg
//==============================================================================
/*!-----------------------------------------------------------------------------
@param flash		Pointer to the flash driver to use to access memory
@param cmdProc		Pointer to the command processor object to receive and send commands through
@param infoStart	Flash block start address when the ProgramInfo non-voltaile data should be stored
@param infoSize		Size of memory area where ProgramInfo non-volatile data should be store (in multiplies of 4kb block lengths)
*/
CFlashProg::CFlashProg(PFlash flash, uint32 infoStart, uint32 infoSize)
{
	//Initialise device access pointers
	_flash = flash;		//CPlatform::DevFlash;

	//Initialise the non-volatile Program Information storage
	_info = new CFlashData(_flash, infoStart, infoSize);

	//Initially indicate we have no hardware information available
	_hardware = NULL;

	//Initialise flash programming variables
	this->ProgReset();
}

/*!-----------------------------------------------------------------------------
*/
CFlashProg::~CFlashProg()
{
	//Unregister commands
	//###

	//Tidy up
	delete _info;
}

/*!-----------------------------------------------------------------------------
Function that raises an OnAction event.
*/
void CFlashProg::DoAction(EFlashProgAction action)
{
	TFlashProgActionParams params;
	params.Action = action;
	this->OnAction.Call(&params);
}

/*!-----------------------------------------------------------------------------
Function called to initialise the programming parameters for a section.
@param init		Pointer to the struct with the programming initialisation parameters
@result			Return code indicating if the programming system was initialised.
*/
EFlashProgReturn CFlashProg::ProgInit(PFlashProgInit init)
{
	TFlashProgInfo info;
	EFlashReturn flashReturn;
	uint32 sectionStart;
	uint32 sectionSize;
	uint8 hashStr[256];
	uint32 hashStrLen;
	uint8 hashVal[20];

	//Reset programming vairables to default values.
	this->ProgReset();

	//Read the device program status information from Flash
	this->ReadInfo(&info);

	//Recreate the validation SHA-1 hash
	hashStrLen = snprintf((pchar)hashStr, 256, "%s-%.5u-%u-%u-%.6u-%u-%u-%.8X",
		FLASH_HASH_KEY,
		init->PartNumber,
		init->PartRevMin,
		init->PartRevMax,
		init->SerialNumber,
		init->DataFormat,
		init->Length,
		init->Checksum
	);
	//Check the hash string isn't too large
	if(hashStrLen >= 256)
		return FPROG_HASH_ERROR;
	//Compute the hash from the hash string
	CSha1::Calc(hashStr, hashStrLen, hashVal);
	//Check the hash matches what we received
	for(uint8 i = 0; i < 20; i++) {
		if(init->Hash[i] != hashVal[i])
			return FPROG_HASH_ERROR;
	}

	//If the HardwareInfo pointer is defined, then validate hardware information
	if(_hardware) {
		if((init->PartNumber > 0) && (init->PartNumber != _hardware->PartNumber)) {
			//Fail if we're trying to program the wrong part number
			//return FPROG_BAD_PARTNUMBER;
			return FPROG_INIT_ERROR;
		}

		if((init->PartRevMin > 0) && (init->PartRevMin > _hardware->PartRevision)) {
			//Fail if that hardware part revision is less than the specified minimum
			//return FPROG_BAD_REVISION;
			return FPROG_INIT_ERROR;
		}

		if((init->PartRevMax > 0) && (init->PartRevMax < _hardware->PartRevision)) {
			//Fail if that hardware part revision is greater than the specified maximum
			//return FPROG_BAD_REVISION;
			return FPROG_INIT_ERROR;
		}

		if((init->SerialNumber > 0) && (init->SerialNumber != _hardware->SerialNumber)) {
			//Fail if we're trying to program the wrong serial number
			//return FPROG_BAD_SERIALNUMBER;
			return FPROG_INIT_ERROR;
		}
	}

	//Check for supported data types
	switch(init->DataFormat) {
		case FPROG_DATA_BINARY :
		case FPROG_DATA_ENCRYPTED :
		{
			//Supported data formats, so skip check
			break;
		}

		case FPROG_DATA_COMPRESSED :
		case FPROG_DATA_ENCCOMP :
		default : {
			//Fail if we have an invalid data format
			return FPROG_INIT_ERROR;
		}
	}

	//Fail the the length for the specified section is wrong
	switch(init->Section) {
		case FLASH_SECTION_BOOT : {
			#if (FIRMWARE_SECTION == FLASH_SECTION_BOOT)
				//If this firmware is compiled as the bootloader, then abort as we CAN'T
				//reprogram the bootloader from the bootloader.
				return FPROG_SECTION_ERROR;
			#else
				sectionStart = FLASH_BOOT_START;
				sectionSize = FLASH_BOOT_SIZE;
				break;
			#endif
		}

		case FLASH_SECTION_MAIN : {
			sectionStart = FLASH_MAIN_START;
			sectionSize = FLASH_MAIN_SIZE;
			break;
		}

		default : {
			//Fail as we have an unidentified section
			return FPROG_SECTION_ERROR;
		}
	}

	//Fail the the length for the specified section is wrong
	if((init->Length > sectionSize) || (init->Length > FLASH_SCRATCH_SIZE)) {
		return FPROG_LENGTH_ERROR;
	}

	//Erase the scratch memory ready for a new program
	flashReturn = _flash->FlashEraseRange(FLASH_SCRATCH_START, FLASH_SCRATCH_SIZE);
	if(flashReturn != FLASH_OK) {
		//Fail as Scratch memory couldn't be erased
		return FPROG_FLASH_ERROR;
	}

	//Indicate we're ready to receive data
	_update.Update = true;
	_update.SrcAddr = FLASH_SCRATCH_START;
	_update.SrcLength = init->Length;
	_update.SrcChecksum = init->Checksum;
	_update.DestSection = init->Section;
	_update.DestAddr = sectionStart;
	_update.DestSize = sectionSize;

	_scratchAddr = FLASH_SCRATCH_START;
	_scratchLength = 0;
	_scratchChecksum = 0;

	_blockCnt = 0;
	_blockFormat = init->DataFormat;

	//Raise an action event
	this->DoAction(FPROG_ACTION_PROG_INIT);

	//Indicate initialisation success
	return FPROG_OK;
}

/*!-----------------------------------------------------------------------------
Function that resets the programming state
*/
void CFlashProg::ProgReset()
{
	//Reset programming vairables to default values.
	_update.Update = false;
	_update.SrcAddr = 0;
	_update.SrcLength = 0;
	_update.SrcChecksum = 0;
	_update.DestSection = 0;
	_update.DestAddr = 0;
	_update.DestSize = 0;

	_scratchAddr = 0;
	_scratchLength = 0;
	_scratchChecksum = 0;

	_blockCnt = 0;
	_blockFormat = FPROG_DATA_BINARY;
}

/*!-----------------------------------------------------------------------------
Function that programs a block into the scratch memory at the next free area
and updates the scratch programming variables.
NB: The length must be a multiple of 4-bytes by definition for XXTEA decryption
with a minimum of 8 bytes.
*/
EFlashProgReturn CFlashProg::ProgScratch(puint8 data, uint16 length)
{
	EFlashReturn flashReturn;
	uint8 dataTmp[FLASH_PHRASE_SIZE];
	puint8 ptr;
	uint16 progLen;
	bool decrypt;
	uint8 decryptKey[16];
	bool decompress;


	//Abort if we're not initialised to receive data
	if(!_update.Update) {
		return FPROG_INIT_ERROR;
	}

	//Abort if we don't have at least 2 unit32's to program,
	//of the length isn't a multiple of 4 bytes (a uint32).
	if((length < 8) || IS_BITS_SET(length, 0x03)) {
		this->ProgReset();
		return FPROG_LENGTH_ERROR;
	}

	//Decode the data format
	switch(_blockFormat) {
		case FPROG_DATA_BINARY : { decompress = false; decrypt = false; break; }
		case FPROG_DATA_COMPRESSED : { decompress = true; decrypt = false; break; }
		case FPROG_DATA_ENCRYPTED : { decompress = false; decrypt = true; break; }
		case FPROG_DATA_ENCCOMP : { decompress = true; decrypt = true; break; }
		default : { return FPROG_INIT_ERROR; } 	//This should never run, as PROG_INIT filters the allowed data types.
	}

	//If required, decrypt the data first (using the XXTEA algorithm)...
	if(decrypt) {
		//Copy the first 14 chararacters from the key into the decryption key array
		//And zeroes any further bytes upto the 16 char length.
		int len = snprintf((pchar)(&decryptKey[0]), 14+1, FLASH_HASH_KEY);
		for(int idx = len; idx < 14; idx++) {
			decryptKey[idx] = 0;
		}
		//Include the block counter value in the last two bytes of the key
		decryptKey[14] = (uint8)(_blockCnt & 0xFF);
		decryptKey[15] = (uint8)((_blockCnt >> 8) & 0xFF);

		//Perform the decryption in place, using the XXTEA algorithm
		//To function, this required a minimum of 8 bytes (64 bits) or the data doesn't decode correctly.
		CXxTea::Decrypt((puint32)data, length / 4, (puint32)decryptKey);
	}

	//If required decompress the data...
	if(decompress) {
		//##TODO
	}

	//Compute the checksum of the current block
	_scratchChecksum = CCrc32::CalcBuffer(data, length, CRC32_GEN_POLY, _scratchChecksum);

	//Loop program Flash phrases from the block.
	while((length > 0) && (_scratchLength < _update.DestSize) && (_scratchLength < FLASH_SCRATCH_SIZE)) {
		//Program the next 8-byte phrase of data
		if(length >= FLASH_PHRASE_SIZE) {
			progLen = FLASH_PHRASE_SIZE;
			ptr = data;
			data += FLASH_PHRASE_SIZE;
		}
		else {
			progLen = length;
			ptr = dataTmp;

			//Copy the remaining bytes into a temporary '0xFF' padded buffer
			//(as 0xFF is unprogrammed memory)
			for(uint8 idx = 0; idx < progLen; idx++) {
				dataTmp[idx] = *data;
				data++;
			}
			for(uint8 idx = progLen; idx < FLASH_PHRASE_SIZE; idx++) {
				//dataTmp[idx] = 0;
				dataTmp[idx] = 0xFF;
			}
		}

		//Compute the CRC32 for the scratch area
		//_scratchChecksum = CCrc32::CalcBuffer(ptr, progLen, CRC32_GEN_POLY, _scratchChecksum);

		//Program the data into the next available scratchpad memory
		flashReturn = _flash->FlashProgramPhrase(_scratchAddr, ptr);

		//If the flash failed to program, then abort programming
		if(flashReturn != FLASH_OK) {
			this->ProgReset();
			return FPROG_FLASH_ERROR;
		}

		//Update control variables
		length -= progLen;
		_scratchAddr += progLen;
		_scratchLength += progLen;
	}

	//Increase the block counter - used for decryption key generation
	_blockCnt++;

	//If the loop finished with data left to program, as we filled scratch memory
	//the report a length error
	if(length > 0) {
		this->ProgReset();
		return FPROG_LENGTH_ERROR;
	}

	//Indicate programming success
	return FPROG_OK;
}

/*!-----------------------------------------------------------------------------
Function that is called to start the programming update sequence once the
scratch memory contains the new program.
@result Returns eitehr COPY or REBOOT status codes indication if the UpdateCopy
	routine should be run immediatly, or the device should be rebooted for the bootloader
	to then perform the copy.
*/
EFlashProgReturn CFlashProg::ProgUpdate()
{
	TFlashProgInfo info;
	bool success;
	uint8 section;

	//Abort if we're not initialised to update data
	if(!_update.Update) {
		return FPROG_INIT_ERROR;
	}

	//Abort if the scratch memory length is less than the program length sent
	//If length is larger, for encryption we may have had to send some padding bytes.
	if(_scratchLength < _update.SrcLength) {
		this->ProgReset();
		return FPROG_LENGTH_ERROR;
	}

	//Abort if the scratch memory checksum doesn't match the program checksum sent
	if(_scratchChecksum != _update.SrcChecksum) {
		this->ProgReset();
		return FPROG_CHECKSUM_ERROR;
	}

	#if (FIRMWARE_SECTION == FLASH_SECTION_BOOT)
	//Abort if we're trying to program the bootloader section when we're executing as the bootloader.
	//This isn't allowed, as it will lead to a flash conflict and errors...
	if(_update.DestSection == FLASH_SECTION_BOOT) {
		this->ProgReset();
		return FPROG_SECTION_ERROR;
	}
	#endif

	//Take action depending on which section is required
	if((_update.DestSection != FLASH_SECTION_BOOT) && (_update.DestSection != FLASH_SECTION_MAIN)) {
		return FPROG_SECTION_ERROR;
	}

	//Read the device program status information from Flash
	success = this->ReadInfo(&info);
	//###Take action here if info can't be read

	//Copy across the information the flash updater needs into the Info Structure
	info.FlashUpdate = _update;

	//Invalid the target firmware section being updated
	info.Firmware[_update.DestSection].Valid = false;
	info.Firmware[_update.DestSection].PartNumber = 0;
	info.Firmware[_update.DestSection].VersionMaj = 0;
	info.Firmware[_update.DestSection].VersionMin = 0;
	info.Firmware[_update.DestSection].VersionBuild = 0;

	//Write the new information back into flash storage
	success = this->WriteInfo(&info);

	//Reset the programming system, so program needs to be reloaded again.
	section = _update.DestSection;	//Memorise the section before we clear the programming variables.
	this->ProgReset();

	//If we couldn't write the info structure, then indicate a flash error occured
	if(!success) {
		return FPROG_FLASH_ERROR;
	}

	//Raise an action event
	this->DoAction(FPROG_ACTION_PROG_UPDATE);

	//Determine the action to now take based on what Firmware mode we're executing in,
	//and what flash section we're trying to program...
	#if (FIRMWARE_SECTION == FLASH_SECTION_BOOT)
		//*** Executing as a BOOTLOADER application ***

		//For programing other sections (as we've already ensured we can't program the
		//bootloader while running as a bootloader), do the reprogramming now as we're not executing
		//in that memory space, so conflict errors arn't a problem, then reboot to complete the update...
		//return FPROG_COPY_NOW;
		return FPROG_REBOOT_NOW;

	#else
		//*** Executing as a NON-BOOTLOADER (MAIN) application ***
		if(section == FLASH_SECTION_BOOT) {
			//If we're trying to program the bootloader section when we're executing as the main program,
			//perform the update now as we're not in that memory space, so conflict errors arn't a problem...
			//But don't reboot incase it screws up - this is for safey to allow another reprogramming.
			return FPROG_COPY_NOW;
		}
		else {
			//If we're NOT trying to program the bootloader section when we're executing as the main program,
			//we must reboot and allow the booloader to perform the program update otherwise
			//we risk a flash conflict and errors...
			return FPROG_REBOOT_NOW;
		}

	#endif
}

/*!-----------------------------------------------------------------------------
Function that reads the device programming information from non-volatile memory
@param info	Pointer to where the read information should be stored
@result True if the information was read (and fitted the struct) otherwise false.
*/
bool CFlashProg::ReadInfo(PFlashProgInfo info)
{
	int32 bytes = _info->ReadType(info);
	return (bytes == sizeof(TFlashProgInfo));
}

/*!-----------------------------------------------------------------------------
Function that sets the pointer to the hardware information to use when validating
weather to accept firmware.
*/
void CFlashProg::SetHardwareInfo(PFlashProgHardwareInfo value)
{
	_hardware = value;
}

/*!-----------------------------------------------------------------------------
Function that write new device programming information to non-volatile memory
@param info	Pointer to where the read information should be programmed from
@result True if the data was written successfully.
*/
bool CFlashProg::WriteInfo(PFlashProgInfo info)
{
	return _info->WriteType(info);
}

/*!-----------------------------------------------------------------------------
Function that attempts to temporarily unsecure the device using the backdoor access key
*/
bool CFlashProg::UnsecureBackdoor()
{
	EFlashReturn flashReturn;

	//Create the key string
	string key = FLASH_HASH_KEY;
	key += "00000000";
	puint8 pkey = (puint8)key.c_str();

	//Attempt the unlock
	flashReturn = _flash->FlashBackdoor(pkey);

	//Return success
	return (flashReturn == FLASH_OK);
}

/*!-----------------------------------------------------------------------------
Function that can be called to force the Update informaiton stored in non-volatile
flash to be cleared. This will prevent a possible firware update/copy when the
device boots.
*/
bool CFlashProg::UpdateClear()
{
	TFlashProgInfo info;
	bool success;

	//Read the device program status information from Flash
	success = this->ReadInfo(&info);

	//Clear down the update params in flash
	if(info.FlashUpdate.Update) {
		info.FlashUpdate.Update = false;
		info.FlashUpdate.SrcAddr = 0;
		info.FlashUpdate.SrcLength = 0;
		info.FlashUpdate.SrcChecksum = 0;
		info.FlashUpdate.DestSection = 0;
		info.FlashUpdate.DestAddr = 0;
		info.FlashUpdate.DestSize = 0;

		//Write the new info back into Flash
		success = this->WriteInfo(&info);
	}

	return success;
}

/*!-----------------------------------------------------------------------------
Function that is called  once a new program has been loaded into the Scratch
memory and the Info.Update structure configured for the copy accordingly.
When run, this function, if required, copies the specified source memory contens from
the Scratch memory area into the required destination position.
*/
bool CFlashProg::UpdateCopy()
{
	TFlashProgInfo info;
	TFlashProgUpdateInfo update;
	bool success;
	EFlashReturn flashReturn;

	//Read the device program status information from Flash
	success = this->ReadInfo(&info);
	update = info.FlashUpdate;

	//Abort if we don't have proper programming details
	if(!update.Update)
		return false;
	if(update.SrcAddr == 0)
		return false;
	if(update.SrcLength == 0)
		return false;

	//Abort if we're trying to update the section we're operating in
	if(update.DestSection == FIRMWARE_SECTION)
		return false;

	//Abort if the source memory checksum fails what we think is should be
	uint32 csum = CCrc32::CalcBuffer((puint8)(update.SrcAddr), update.SrcLength, CRC32_GEN_POLY, 0);
	if(update.SrcChecksum != csum)
		return false;

	//Raise an action event
	this->DoAction(FPROG_ACTION_UPDATE_START);

	//Disable Interrupts during programming
	IRQ_DISABLE;

	//Erase the destination memory prior to copying
	if(success) {
		flashReturn = _flash->FlashEraseRange(update.DestAddr, update.DestSize);
		success = (flashReturn == FLASH_OK);
	}

	//Copy the program memory
	if(success) {
		flashReturn = _flash->FlashProgram(update.DestAddr, (puint8)(update.SrcAddr), update.SrcLength, NULL);
		success = (flashReturn == FLASH_OK);
	}

	//Check the security settings, and resecure the device if required
	{
		//Read the current security config settings
		TFlashConfig cfg;
		_flash->ConfigRead(&cfg);

		//Only reset security if it currently isn't set
		if((cfg.FSec & FTFE_FSEC_SEC_MASK) != 0) {
			//If we're not debug firmware, then secure the device
			#ifndef DEBUG
				//Backdoor Enabled, Mass Erase Enabled, Failure Analysis Granted, Flash Security SECURE
				cfg.FSec = FTFE_FSEC_KEYEN(2) | FTFE_FSEC_MEEN(3) | FTFE_FSEC_FSLACC(3) | FTFE_FSEC_SEC(0);
			#endif

			//Set the backdoor key value
			string tmp = FLASH_HASH_KEY;
			tmp += "00000000";
			puint8 ptmp = (puint8)tmp.c_str();
			for(uint8 idx = 0; idx < 8; idx++) {
				cfg.Key[idx] = *ptmp;
				ptmp++;
			}

			//Reprogram the config memory
			_flash->ConfigProgram(&cfg);
		}
	}

	//Once programming is complete and verified, clear the the Update info
	if(success) {
		//Indicate the updated firmware section is now valid
		info.Firmware[update.DestSection].Valid = true;
		info.Firmware[update.DestSection].Checksum = info.FlashUpdate.SrcChecksum;

		//Clear down the update params in flash
		info.FlashUpdate.Update = false;
		info.FlashUpdate.SrcAddr = 0;
		info.FlashUpdate.SrcLength = 0;
		info.FlashUpdate.SrcChecksum = 0;
		info.FlashUpdate.DestSection = 0;
		info.FlashUpdate.DestAddr = 0;
		info.FlashUpdate.DestSize = 0;

		//Write the new info back into Flash
		success = this->WriteInfo(&info);
	}

	//Reenable Interrupts
	IRQ_ENABLE;

	//Raise an action event
	if(success)
		this->DoAction(FPROG_ACTION_UPDATE_DONE);
	else
		this->DoAction(FPROG_ACTION_UPDATE_ERROR);

	return success;
}


/*!-----------------------------------------------------------------------------
Function that is called when the device boots to check the current firmware version
information stored is correct and up to date. If not, the new values are written
into non-volatile memory.
@result True if the firmware is correct, or was written correctly.
*/
bool CFlashProg::UpdateFirmwareInfo()
{
	TFlashProgInfo info;
	bool success;

	//Read the device program status information from Flash
	success = this->ReadInfo(&info);

	//Determine if we need to change any of the values
	bool change = false;
	change |= (info.Firmware[FIRMWARE_SECTION].Valid == false);
	change |= (info.Firmware[FIRMWARE_SECTION].PartNumber != FIRMWARE_PARTNUMBER);
	change |= (info.Firmware[FIRMWARE_SECTION].VersionMaj != FIRMWARE_VERSION_MAJOR);
	change |= (info.Firmware[FIRMWARE_SECTION].VersionMin != FIRMWARE_VERSION_MINOR);
	change |= (info.Firmware[FIRMWARE_SECTION].VersionBuild != FIRMWARE_VERSION_BUILD);

	if(change) {
		//Update the info settings to match the current firmware values
		info.Firmware[FIRMWARE_SECTION].Valid = true;	//Ensure the firmware is reported as valid if we're executing this.
		info.Firmware[FIRMWARE_SECTION].PartNumber = FIRMWARE_PARTNUMBER;
		info.Firmware[FIRMWARE_SECTION].VersionMaj = FIRMWARE_VERSION_MAJOR;
		info.Firmware[FIRMWARE_SECTION].VersionMin = FIRMWARE_VERSION_MINOR;
		info.Firmware[FIRMWARE_SECTION].VersionBuild = FIRMWARE_VERSION_BUILD;

		//Write the new info back into Flash
		success = this->WriteInfo(&info);
	}

	return success;
}

//==============================================================================
