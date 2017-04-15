/*==============================================================================
File that contains definitions defining the version information
==============================================================================*/
#ifndef CMD_DEFS_H
#define CMD_DEFS_H

//------------------------------------------------------------------------------
//Define Command Processor ID Codes (CID)
//(in addition to those in cmd_proc.hpp)
#define CID_SYS_ALIVE							0x01	/*!< Command sent to receive a simple alive message from the beacon */
#define CID_SYS_INFO							0x02	/*!< Command sent to receive hardware/firmware identification message from the beacon */
#define CID_SYS_REBOOT							0x03	/*!< Command sent to reboot the device */
#define CID_PROG_INIT							0x0D	/*!< Command sent to initialise a flash programming sequence */
#define CID_PROG_BLOCK							0x0E	/*!< Command sent to transfer a flash programming block */
#define CID_PROG_UPDATE							0x0F	/*!< Command sent to update the firmware once program transfer has completed */

//------------------------------------------------------------------------------
//Define Command Processor Status Codes (CST)
//(in addition to those in cmd_proc.hpp)
#define CST_PROG_FLASH_ERROR					0x0A	/*!< Raised if an error occurs while writing flash data */
#define CST_PROG_FIRMWARE_ERROR					0x0B
#define CST_PROG_SECTION_ERROR					0x0C
#define CST_PROG_LENGTH_ERROR					0x0D
#define CST_PROG_DATA_ERROR						0x0E
#define CST_PROG_CHECKSUM_ERROR					0x0F

//==============================================================================
#endif
