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

extern uint8_t raw_DMX_Channels[N_RawChannels];
extern uint8_t DMX_Channels[N_Channels];
extern uint16_t	DMX_StartAddress;
extern bool DMX2DMATransferFlag;

#define EEPROM_START_ADDRESS 0x08007000u
extern uint32_t EEPROM_BlockTable[4u];

extern void app_DMXCore_Init(void);
extern void app_DMX_DMATransfer(void);

#endif /* APP_DMXCORE_H_ */
