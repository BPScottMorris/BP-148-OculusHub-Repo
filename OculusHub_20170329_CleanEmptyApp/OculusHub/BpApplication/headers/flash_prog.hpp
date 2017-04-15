/*==============================================================================
Module that uses the low level FTFL device to impliment a In-Circuit application
programmer.
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef FLASH_PROG_HPP
#define FLASH_PROG_HPP

//Include system libraries
#include <stdio.h>		//For snprintf function

//Include common type definitions and macros
#include "common.h"

//Include helper classes
#include "maths.hpp"
#include "crc32.hpp"
#include "callback.hpp"
#include "sha1.hpp"
#include "tea.hpp"
#include "serialize.hpp"

//Include the Flash access device
#include "flash.hpp"
#include "flash_data.hpp"

//Include the platform definitions to read flash and firmware address settings from
//#include "platform.h"

//------------------------------------------------------------------------------
#ifndef FIRMWARE_SECTION
	PRAGMA_ERROR("The FIRMWARE_SECTION definition must specify which firmware section this program resides in.")
#endif

#ifndef FIRMWARE_PARTNUMBER
	PRAGMA_ERROR("The FIRMWARE_PARTNUMBER definition must specify the firmware's part number for ID purposes.")
#endif

#ifndef FIRMWARE_VERSION_MAJOR
	PRAGMA_ERROR("The FIRMWARE_VERSION_MAJOR definition must specify the firmware's major version number.")
#endif

#ifndef FIRMWARE_VERSION_MINOR
	PRAGMA_ERROR("The FIRMWARE_VERSION_MINOR definition must specify the firmware's minor version number.")
#endif

#ifndef FIRMWARE_VERSION_BUILD
	PRAGMA_ERROR("The FIRMWARE_VERSION_BUILD definition must specify the firmware's build number.")
#endif

#ifndef FLASH_HASH_KEY
	PRAGMA_ERROR("The FLASH_HASH_KEY definition should contain a security key used when generating Firmware hashes.")
#endif

//==============================================================================
//General Definitions and Types
//==============================================================================
/*! Enumeration that describes how data will be presented when blocks are received */
enum EFlashProgDataFormat {
	FPROG_DATA_BINARY = 0x00,		/*!< Data is in its raw binary form */
	FPROG_DATA_COMPRESSED = 0x01,	/*!< Data is compressed */
	FPROG_DATA_ENCRYPTED = 0x02,	/*!< Data is encrypted, using the XXTEA algorithm */
	FPROG_DATA_ENCCOMP = 0x03		/*!< Data is encrypted and compressed */
};

//------------------------------------------------------------------------------
/*! Record that defines the identity information stored about hardware
*/
struct TFlashProgHardwareInfo {
	uint16		PartNumber;
	uint8		PartRevision;
	uint32		SerialNumber;
	uint16		FlagsSys;				//System only configurable flags (cannot be modified without engineering unlock) - Not used for anything at present, but could include flags to indicate hardware capabilities
	uint16		FlagsUser;				//User configurable flags for hardware settings

	/*! Function the deserialises an object into the struct */
	bool Deserialize(PSerialize serialize) {
		bool success;
		success = serialize->ReadUint16(&this->PartNumber, 0);
		success &= serialize->ReadUint8(&this->PartRevision, 0);
		success &= serialize->ReadUint32(&this->SerialNumber, 0);
		success &= serialize->ReadUint16(&this->FlagsSys, 0);
		success &= serialize->ReadUint16(&this->FlagsUser, 0);
		return success;
	}

	/*! Function that serializes the struct */
	bool Serialize(PSerialize serialize) {
		bool success;
		success = serialize->AddUint16(this->PartNumber);
		success &= serialize->AddUint8(this->PartRevision);
		success &= serialize->AddUint32(this->SerialNumber);
		success &= serialize->AddUint16(this->FlagsSys);
		success &= serialize->AddUint16(this->FlagsUser);
		return success;
	}
};

/*! Define a pointer to a HardwareInfo struct */
typedef TFlashProgHardwareInfo* PFlashProgHardwareInfo;

//------------------------------------------------------------------------------
/*! Record that is stored into non-volatile flash and also used by the flash
programmer to track values used for the firmware update process. This record
describes how the source (usually Scratch memory) should be copied into its
final location. */
struct TFlashProgUpdateInfo {
	bool Update;
	uint32 SrcAddr;		//The address where source data starts
	uint32 SrcLength;	//The length of the source data to copy to the destination
	uint32 SrcChecksum;	//The checksum of the source data
	uint8 DestSection;	//The destination section being programmed
	uint32 DestAddr;	//The address where data should be copied to
	uint32 DestSize;	//The size of the destination area to erase
};

//------------------------------------------------------------------------------
/*! Record that defines the identity information stored about firmware.
As this struct is stored in non-volatile memory, and may in the future
require additional fields adding without corrupting the memory arrangement or
require reporgramming, a Union is used to ensure the struct reserves 32 bytes
of memory
*/
struct TFlashProgFirmwareInfo {
	bool	Valid;
	uint16	PartNumber;
	uint8	VersionMaj;
	uint8	VersionMin;
	uint16	VersionBuild;
	uint32	Checksum;

	/*! Function the deserialises an object into the struct */
	bool Deserialize(PSerialize serialize) {
		bool success;
		success = serialize->ReadBool(&this->Valid, 0);
		success = serialize->ReadUint16(&this->PartNumber, 0);
		success &= serialize->ReadUint8(&this->VersionMaj, 0);
		success &= serialize->ReadUint8(&this->VersionMin, 0);
		success &= serialize->ReadUint16(&this->VersionBuild, 0);
		success &= serialize->ReadUint32(&this->Checksum, 0);
		return success;
	}

	/*! Function that serializes the struct */
	bool Serialize(PSerialize serialize) {
		bool success;
		success = serialize->AddBool(this->Valid);
		success &= serialize->AddUint16(this->PartNumber);
		success &= serialize->AddUint8(this->VersionMaj);
		success &= serialize->AddUint8(this->VersionMin);
		success &= serialize->AddUint16(this->VersionBuild);
		success &= serialize->AddUint32(this->Checksum);
		return success;
	}
};

/*!
Record that defines the identity information stored about the current
firmware configuration.
This status is typically stored in the upper sector of Flash and used by the
firmware programming system.
*/
struct TFlashProgInfo {
	TFlashProgUpdateInfo FlashUpdate;		//Flash updater information
	//TFlashProgHardwareInfo Hardware;		//Hardware information
	TFlashProgFirmwareInfo Firmware[2];		//Firmware Sections
};

/*! Define a pointer for a Flash Program Device Info record */
typedef TFlashProgInfo* PFlashProgInfo;

//------------------------------------------------------------------------------
/*! Structure used to pass initialisation details into the programming system
for the section to be programmed */
struct TFlashProgInit {
	uint8		Section;
	uint16		PartNumber;
	uint8		PartRevMin;
	uint8		PartRevMax;
	uint32		SerialNumber;
	EFlashProgDataFormat DataFormat;
	uint32		Length;
	uint32		Checksum;
	uint8		Hash[20];
};

typedef TFlashProgInit* PFlashProgInit;

//------------------------------------------------------------------------------
/*! Enumeration of action codes passed through the OnAction event callback */
enum EFlashProgAction {
	FPROG_ACTION_PROG_INIT,
	FPROG_ACTION_PROG_UPDATE,
	FPROG_ACTION_UPDATE_START,
	FPROG_ACTION_UPDATE_DONE,
	FPROG_ACTION_UPDATE_ERROR
};

/*! Record that is passed as part of the FlashProg Action event */
struct TFlashProgActionParams {
	EFlashProgAction Action;
};

typedef TFlashProgActionParams* PFlashProgActionParams;

typedef CCallback1<void, PFlashProgActionParams> CFlashProgActionCallback;

//------------------------------------------------------------------------------
/*! Enumeration used to return status codes from Flash Programmer routines */
enum EFlashProgReturn {
	FPROG_OK,
	FPROG_FLASH_ERROR,
	FPROG_INIT_ERROR,
	FPROG_SECTION_ERROR,
	FPROG_LENGTH_ERROR,
	FPROG_CHECKSUM_ERROR,
	FPROG_HASH_ERROR,
	FPROG_COPY_NOW,
	FPROG_REBOOT_NOW
};

//==============================================================================
//Class Definition...
//==============================================================================
/*! Define a class that implements in-system programming
*/
class CFlashProg {
	private:
		PFlash		_flash;
		PFlashData	_info;
		uint16		_blockCnt;
		EFlashProgDataFormat _blockFormat;
		TFlashProgUpdateInfo _update;
		PFlashProgHardwareInfo _hardware;
		uint32		_scratchAddr;
		uint32		_scratchLength;
		uint32		_scratchChecksum;

		//Private Methods
		void DoAction(EFlashProgAction action);

	public:
		//Construction and Disposal
		CFlashProg(PFlash flash, uint32 infoStart, uint32 infoSize);
		~CFlashProg();

		//Methods
		EFlashProgReturn ProgInit(PFlashProgInit init);
		void ProgReset();
		EFlashProgReturn ProgScratch(puint8 data, uint16 length);
		EFlashProgReturn ProgUpdate();
		void SetHardwareInfo(PFlashProgHardwareInfo value);
		bool ReadInfo(PFlashProgInfo info);
		bool WriteInfo(PFlashProgInfo info);
		bool UnsecureBackdoor();
		bool UpdateClear();
		bool UpdateCopy();
		bool UpdateFirmwareInfo();

		//Event Callback
		CFlashProgActionCallback OnAction;
};

/*! Define a pointer to a flash programmer object */
typedef CFlashProg* PFlashProg;

//==============================================================================
#endif
