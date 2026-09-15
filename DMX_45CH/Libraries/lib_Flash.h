/*
 * lib_Flash.h
 *
 *  Created on: Dec 16, 2023
 *      Author: OmarSevilla
 */

#ifndef LIB_FLASH_H_
#define LIB_FLASH_H_

#include "stm32f3xx.h"

extern uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords);
extern void Flash_Read_Data (uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords);

#endif /* LIB_FLASH_H_ */
