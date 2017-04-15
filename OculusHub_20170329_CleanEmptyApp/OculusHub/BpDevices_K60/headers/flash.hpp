/*==============================================================================
Module that provides definitions and implementations for managing the FTFL
Flash Memory module - allowing reading, reprogramming
==============================================================================*/
//Prevent multiple inclusions of this file
#ifndef FLASH_HPP
#define FLASH_HPP

//Include system libraries

//Include common type definitions and macros
#include "common.h"

//Include the processor platform
#include "processor.h"

//Include helper classes
//#include "crc16.hpp"
//#include "callbacks.hpp"

//==============================================================================
//General Definitions and Types
//==============================================================================
#ifndef FLASH_SIZE
	PRAGMA_ERROR("FLASH_SIZE constant not defined")
#endif

#ifndef FLASH_SECTOR_SIZE
	PRAGMA_ERROR("FLASH_SECTOR_SIZE constant not defined")
#endif

#ifndef FLASH_BLOCK_SIZE
	PRAGMA_ERROR("FLASH_BLOCK_SIZE constant not defined")
#endif

//------------------------------------------------------------------------------
/*! Base address of Flash area */
#define FLASH_BASE						0x00000000

//Base addresses of program flash blocks
//#define FLASH_BLOCK0_BASE				FLASH_BASE
//#define FLASH_BLOCK1_BASE				(FLASH_BLOCK0_BASE + FLASH_BLOCK_SIZE)
//#define FLASH_BLOCK2_BASE				(FLASH_BLOCK1_BASE + FLASH_BLOCK_SIZE)
//#define FLASH_BLOCK3_BASE				(FLASH_BLOCK2_BASE + FLASH_BLOCK_SIZE)

//For PFlash Swapping, two halves are used - define size and base addresses of halves
//#define FLASH_HALF_SIZE				(FLASH_BLOCK_SIZE * 2)	/*!< Program Flash block size, 512kB */
//#define FLASH_HALF0_BASE				FLASH_BLOCK0_BASE
//#define FLASH_HALF1_BASE				FLASH_BLOCK2_BASE

/* Address offset (compare to start addr of P/D flash) and size of PFlash IFR and DFlash IFR */
#define FLASH_IFR_OFFSET				(FLASH_BASE)		 								/* 0x00000000*/
#define FLASH_IFR_SIZE					0x00000400
#define FLASH_IFR_START_ADDRESS			(FLASH_IFR_OFFSET) 									/* 0x00000000 */
#define FLASH_IFR_END_ADDRESS			(FLASH_IFR_START_ADDRESS + FLASH_IFR_SIZE - 1)		/* 0x000003FF */

//Addresses of Protected Configuration memory
#define FLASH_CNFG_OFFSET				(FLASH_IFR_OFFSET + FLASH_IFR_SIZE)					/* 0x00000400 */
#define FLASH_CNFG_SIZE					0x00000010
#define FLASH_CNFG_START_ADDRESS		(FLASH_CNFG_OFFSET)									/* 0x00000400 */
#define FLASH_CNFG_END_ADDRESS			(FLASH_CNFG_START_ADDRESS + FLASH_CNFG_SIZE - 1)	/* 0x0000040F */

//Addresses of executable program
#define FLASH_APP_OFFSET				(FLASH_CNFG_OFFSET + FLASH_CNFG_SIZE)
#define FLASH_APP_START_ADDRESS			(FLASH_APP_OFFSET)

/*! Safety code to use when calling the PFlashEraseAll command */
#define FLASH_ERASE_SAFETY				0xAA55CC33

//------------------------------------------------------------------------------
//Element sizes for use by the flash programming system...
#define FLASH_WORD_SIZE					0x0002		/*!< Word size - 2 bytes, 16 bits */
#define FLASH_LONGWORD_SIZE				0x0004		/*!< Longword size - 4 bytes, 32 bits */
#define FLASH_PHRASE_SIZE				0x0008		/*!< Phrase size - 8 bytes, 64 bits */
#define FLASH_DPHRASE_SIZE				0x0010		/*!< Double-phrase size - 16 bytes, 128 bits */

//------------------------------------------------------------------------------
//Alignment sizes for various programming operations
#define FLASH_ERSBLK_ALIGN_SIZE			FLASH_DPHRASE_SIZE		/* Check align of erase block function */
#define FLASH_PGMCHK_ALIGN_SIZE			FLASH_LONGWORD_SIZE		/* Check align of program check function */
//#define FLASH_PPGMSEC_ALIGN_SIZE		FLASH_DPHRASE_SIZE		/* Check align of program section function */
//#define FLASH_DPGMSEC_ALIGN_SIZE		FLASH_DPHRASE_SIZE		/* Check align of program section function */
#define FLASH_VERBLK_ALIGN_SIZE			FLASH_DPHRASE_SIZE		/* Check align of verify block function */
//#define FLASH_PRD1SEC_ALIGN_SIZE		FLASH_DPHRASE_SIZE		/* Check align of verify section function */
//#define FLASH_DRD1SEC_ALIGN_SIZE		FLASH_DPHRASE_SIZE		/* Check align of verify section function */
#define FLASH_SWAP_ALIGN_SIZE			FLASH_DPHRASE_SIZE		/* Check align of swap function*/
//#define FLASH_RDRSRC_ALIGN_SIZE		FLASH_PHRASE_SIZE		/* Check align of read resource function */
//#define FLASH_RDONCE_INDEX_MAX		0x7						/* Maximum index in read once command */

//------------------------------------------------------------------------------
//Command codes for programming the flash
#define FLASH_CMD_VERIFY_BLOCK			0x00
#define FLASH_CMD_VERIFY_SECTION		0x01
#define FLASH_CMD_PROGRAM_CHECK			0x02
//#define FLASH_CMD_READ_RESOURCE		0x03
//#define FLASH_CMD_PROGRAM_LONGWORD	0x06
#define FLASH_CMD_PROGRAM_PHRASE		0x07
#define FLASH_CMD_ERASE_BLOCK			0x08
#define FLASH_CMD_ERASE_SECTOR			0x09
//#define FLASH_CMD_PROGRAM_SECTION		0x0B
//#define FLASH_CMD_VERIFY_ALL_BLOCK	0x40
//#define FLASH_CMD_READ_ONCE			0x41
//#define FLASH_CMD_PROGRAM_ONCE		0x43
#define FLASH_CMD_ERASE_ALL_BLOCK		0x44
#define FLASH_CMD_SECURITY_BY_PASS		0x45
//#define FLASH_CMD_PFLASH_SWAP			0x46
//#define FLASH_CMD_PROGRAM_PARTITION	0x80
//#define FLASH_CMD_SET_EERAM			0x81

#define FLASH_RAM_PROG_SIZE				30

//------------------------------------------------------------------------------
/*! Structure that maps onto the flash configuration memory at address FLASH_CNFG_OFFSET
*/
struct TFlashConfig {
	uint8 Key[8];			//Back-door comparison key
	uint32 FProt;			//Flash Protection bytes
	uint8 FSec;				//Flash security bytes
	uint8 FOpt;				//Flash option register
	uint8 FEProt;			//EEProm protection byte - FlexNVM devices only
	uint8 FDProt;			//Data protection byte - FlexNVM devices only
};

typedef TFlashConfig* PFlashConfig;

//------------------------------------------------------------------------------
/*! Enumeration specifying the Flash Command return codes */
enum EFlashReturn {
	FLASH_OK = 0,
	FLASH_ERR_SIZE = 1,
	FLASH_ERR_ADDR = 2,
	FLASH_ERR_RANGE = 3,
	FLASH_ERR_ACCERR = 4,
	FLASH_ERR_PVIOL = 5,
	FLASH_ERR_MGSTAT0 = 6,
	FLASH_ERR_PARAM = 7
	//FLASH_ERR_CHANGEPROT = 0x0020,
	//FLASH_ERR_EEESIZE = 0x0040,
	//FLASH_ERR_EFLASHSIZE = 0x0080,
	//FLASH_ERR_NOEEE = 0x0200,
	//FLASH_ERR_EFLASHONLY = 0x0400,
	//FLASH_ERR_DFLASHONLY = 0x0800,
	//FLASH_ERR_RDCOLERR = 0x1000,
	//FLASH_ERR_RAMRDY = 0x2000,
};

//------------------------------------------------------------------------------
/*! Enumeration specifying the Flash Command Read Margin parameter codes */
enum EFlashReadMargin {
	FLASH_MARGIN_NORMAL = 0x00,
	FLASH_MARGIN_USER = 0x01,
	FLASH_MARGIN_FACTORY = 0x02
};

//------------------------------------------------------------------------------
/*! Control codes for a FLASH_CMD_PFLASH_SWAP command
enum EFlashSwapCmd {
	FLASH_SWAP_SET_INDICATOR_ADDR = 0x01,
	FLASH_SWAP_SET_IN_PREPARE = 0x02,
	FLASH_SWAP_SET_IN_COMPLETE = 0x04,
	FLASH_SWAP_REPORT_STATUS = 0x08
};
*/

/*! Status codes for the current swap state
enum EFlashSwapState {
	FLASH_SWAP_UNINIT = 0x00,
	FLASH_SWAP_READY = 0x01,
	FLASH_SWAP_UPDATE = 0x02,
	FLASH_SWAP_UPDATE_ERASED = 0x03,
	FLASH_SWAP_COMPLETE = 0x04
};
*/

/*! Status codes for the Current and Next swap states
enum EFlashSwapBlockStatus {
	FLASH_BLOCK_0_ACTIVE = 0x00,
	FLASH_BLOCK_1_ACTIVE = 0x01
};
*/

/*! Struct holding the return codes from a SwapGetStatus command
struct TFlashSwapState {
	EFlashSwapState CurrentState;
	EFlashSwapBlockStatus CurrentBlock;
	EFlashSwapBlockStatus NextBlock;
};
*/

/*! Define a pointer to a TFlashSwapState structure
typedef TFlashSwapState* PFlashSwapState;
*/

//------------------------------------------------------------------------------
/*! Define a struct for params passed in an CAcoXcvrRxErrorCallback event
struct TFlashSwapParams {
	EFlashSwapState State;
	bool Continue;
};
*/

/*! Define a pointer to a TAcoXcvrActionParams structure
typedef TFlashSwapParams* PFlashSwapParams;
*/

/*! Define a callback for a swap event
typedef CCallback1<void, PFlashSwapParams>	CFlashSwapCallback;
*/

//==============================================================================
//Class Definition...
//==============================================================================
/*!As different Kinetis devices use FTFL or FTFE types, map required memory
access struct to a uniform type
*/
typedef FTFE_Type FLASH_Type;

/*!
Class that provides access to the internal FTFL/FTFE flash programming and management
module, allowing self reprogramming and data storage operations.
*/
class CFlash {
	private:
		//Private fields
		FMC_Type*	_fmc;
		FLASH_Type*	_flash;
		bool		_cfgLock;

		//Private Methods
		bool CheckAddress(uint32& addrStart, uint32 addrRange);
		EFlashReturn ExecuteCmd(uint8 cmdSize, puint8 cmdData);
		EFlashReturn FlashCheckLWord(uint32 destAddr, puint8 verifyData, EFlashReadMargin marginLevel);
		//EFlashReturn CmdSwapExecute(uint32 addr, EFlashSwapCmd swapCmd);

		//Static methods
		static void ExecuteCmdRam(FLASH_Type* flash);		/*!< Function programmed into RAM to execute a programming command */

	public:
		//Construction and Disposal
		CFlash();
		virtual ~CFlash();

		//Methods
		EFlashReturn ConfigProgram(PFlashConfig cfg);
		void ConfigRead(PFlashConfig cfg);
		void DebugReturnCode(EFlashReturn returnCode);
		bool GetConfigLock();
		uint8 GetFlashActiveBlock();
		EFlashReturn FlashBackdoor(puint8 key);
		EFlashReturn FlashCheck(uint32 destAddr, puint8 verifyData, uint32 size, EFlashReadMargin marginLevel, puint32 failAddr = NULL);
		EFlashReturn FlashEraseAll(uint32 confirm);
		EFlashReturn FlashEraseBlock(uint32 addr);
		EFlashReturn FlashEraseRange(uint32 addr, uint32 size);
		EFlashReturn FlashEraseSector(uint32 addr);
		EFlashReturn FlashEraseSectors(uint32 addr, uint16 sectors);
		EFlashReturn FlashProgram(uint32 destAddr, puint8 srcData, uint32 size, puint32 failAddr = NULL);
		EFlashReturn FlashProgramPhrase(uint32 destAddr, puint8 srcData);
		EFlashReturn FlashVerifyBlock(uint32 addr, EFlashReadMargin marginLevel);
		EFlashReturn FlashVerifySector(uint32 addr, EFlashReadMargin marginLevel);
		EFlashReturn FlashVerifySectors(uint32 addr, uint16 sectors, EFlashReadMargin marginLevel);
		//EFlashReturn CmdSwap(uint32 flashAddr);
		//EFlashReturn CmdSwapGetStatus(uint32 flashAddr, PFlashSwapState status);
		void SetConfigLock(bool value);

		//Event Callbacks
		//CFlashSwapCallback OnSwapStatus;

		//Class functions
		static void FlashDump(uint32 addr, uint32 length, uint8 rowLen = 16);
};

/*! Define a pointer to a Flash class */
typedef CFlash* PFlash;

//==============================================================================
#endif
