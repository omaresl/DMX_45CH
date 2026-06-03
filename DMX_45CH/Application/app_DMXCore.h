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

typedef enum
{
	DMX_State_IDLE,
	DMX_State_BREAK,
	DMX_State_START,
	DMX_State_SKIP,
	DMX_State_RX,
	DMX_State_EE,
	DMX_State_RXCMD,
	DMX_State_EXECMD,
	DMX_State_N
}T_DMXState;

/* Channels Supported */

#define N_Channels	45u

#define N_RawChannels 	513u /* 512 + 1 (Start) */

#define DMX_DEFAULT_START_ADDRESS	1u
#define DMX_INIT_START_ADDRESS	1u//DMX_DEFAULT_START_ADDRESS
#define DMX_TIMEOUT_COUNT	5000u
#define DMX_CMD_BUFFER_SIZE	100u

extern uint8_t raw_DMX_Channels[N_RawChannels];
extern uint8_t DMX_Channels[N_Channels];
extern uint32_t DMXtimeoutCounter;
extern uint16_t	DMX_StartAddress;
extern T_DMXState DMX_State;
extern uint8_t DMX_CMD_Buffer[DMX_CMD_BUFFER_SIZE];
extern uint8_t DMX_CMD_BufferIDX;
extern bool DMX2DMATransferFlag;

#define EEPROM_START_ADDRESS 0x08007000u
extern uint32_t EEPROM_BlockTable[4u];

extern void app_DMX_RX_ITHandler(UART_HandleTypeDef *l_UARTInstance);
extern void app_DMXCore_Init(void);
extern void app_DMX_DMATransfer(void);

#endif /* APP_DMXCORE_H_ */
