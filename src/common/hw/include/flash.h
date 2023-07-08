/*
 * flash.h
 *
 *  Created on: 2020. 12. 14.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_FLASH_H_
#define SRC_COMMON_HW_INCLUDE_FLASH_H_

#include "hw_def.h"


#ifdef _USE_HW_FLASH


bool flashInit(void);
bool flashErase(uint32_t addr, uint32_t length);
bool flashWrite(uint32_t addr, uint8_t *p_data, uint32_t length);
bool flashRead(uint32_t addr, uint8_t *p_data, uint32_t length);

FLASH_Status FLASH_EraseSector(uint32_t FLASH_Sector, uint8_t VoltageRange);


#endif


#endif /* SRC_COMMON_HW_INCLUDE_FLASH_H_ */
