#ifndef PROCESSOR_H
#define PROCESSOR_H

//Include common type definitions and macros
#include "common.h"

//Include the CMSIS Framework for the hardware
#include "cmsis/MK60F15.h"

//Define possible processor types
//#define MCU_MK22FX512AVLH12		1		/*!< SRS Thruster board mk1 */
//#define MCU_MK22FX512AVLK12		2		/*!< SRS BattDist board */
//#define MCU_MK22FN1M0VLH22		3	 	/*!< SRS DevDist board */
//#define MCU_MK22FN1M0AVLH12		4		/*!< SRS Thruster board mk2 */

#define MCU_MK60FN1M0VLQ15 			5		/*!< Oculus Hub board */

#include "platform.h"

#endif
