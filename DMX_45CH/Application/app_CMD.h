/*
 * app_CMD.h
 *
 *  Created on: Mar 5, 2024
 *      Author: OmarSevilla
 */

#ifndef APP_CMD_H_
#define APP_CMD_H_

#include <stddef.h>
#include <stdint.h>
#include "stm32f3xx_hal.h"
#include <stdbool.h>

typedef enum
{
	CMD_ID_REQ,
	CMD_SET_ADR,
	CMD_NOT_FOUND,
	CMD_N
}T_CMD;

extern char str_DMXIDREQ[];
extern char str_SETDMXADR[];

extern T_CMD app_CMD_GetCMD(uint8_t* RxBuffer, uint8_t BufferSize);
extern bool app_CMD_ExecCMD(T_CMD l_CMD, uint8_t* RxBuffer, uint8_t BufferSize);

#endif /* APP_CMD_H_ */
