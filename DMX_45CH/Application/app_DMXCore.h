/*
 * app_DMXCore.h
 *
 *  Created on: Dec 11, 2023
 *      Author: OmarSevilla
 */

#ifndef APP_DMXCORE_H_
#define APP_DMXCORE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "app_CFG.h"
#include "stm32f3xx_hal.h"

/* Channels Supported */

#define N_Channels	54u

#define N_RawChannels 	513u /* 512 + 1 (Start) */

#define DMX_DEFAULT_START_ADDRESS	1u
#define DMX_INIT_START_ADDRESS	1u//DMX_DEFAULT_START_ADDRESS

/* Highest valid start address: last window of N_Channels in the frame */
#define DMX_MAX_START_ADDRESS	((uint16_t)(N_RawChannels - N_Channels))

extern uint8_t raw_DMX_Channels[N_RawChannels];
extern uint8_t DMX_Channels[N_Channels];
extern uint16_t	DMX_StartAddress;
extern bool DMX2DMATransferFlag;

#define EEPROM_START_ADDRESS 0x08007000u
/* EEPROM slot map (u32 each) */
#define EEPROM_SLOT_DMX_START	0u
#define EEPROM_SLOT_SEQ_LIMIT	1u
#define EEPROM_SLOT_AC_LIMIT	2u
#define EEPROM_SLOT_LED_LIMIT	3u
#define EEPROM_N_SLOTS			4u
#define EEPROM_SLOT_ADDR(slot)	(EEPROM_START_ADDRESS + ((uint32_t)(slot) * 4u))
extern uint32_t EEPROM_BlockTable[4u];

extern void app_DMXCore_Init(void);
extern void app_DMX_DMATransfer(void);
/* Validated setter: persists to EEPROM slot 0 (other slots preserved) */
extern bool app_DMXCore_SetStartAddress(uint16_t l_Address);

#endif /* APP_DMXCORE_H_ */
