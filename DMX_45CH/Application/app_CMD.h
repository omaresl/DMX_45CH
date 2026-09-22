/*
 * app_CMD.h
 *
 * Binary UART command protocol (vendor DMX start code 0xEE).
 * See Doc/uart_commands.md and DMX_Tester/docs/protocol.md.
 */

#ifndef APP_CMD_H_
#define APP_CMD_H_

#include <stdint.h>
#include <stdbool.h>

#define CMD_START_CODE			(0xEEu)

#define CMD_GET_INFO			(0x01u)
#define CMD_SET_DMX_ADDR		(0x02u)
#define CMD_SET_POWER_LIMIT		(0x03u)
#define CMD_GET_STATUS			(0x04u)
#define CMD_DISCOVER			(0x05u)
#define CMD_SELECT_UID			(0x06u)
#define CMD_DESELECT			(0x07u)

#define CMD_ACK_BYTE			(0xA5u)
#define CMD_ACK_OK				(0x00u)
#define CMD_NACK_INVALID		(0x01u)

/* Firmware identity reported by CMD_GET_INFO */
#define FW_VERSION_MAJOR		(0u)
#define FW_VERSION_MINOR		(3u)
#define MODEL_ID				(0x0045u)

/* STM32F302 unique device ID (96-bit); base address from CMSIS (stm32f302xc.h) */
#define UID_LENGTH				(12u)

/* Fixed response lengths (frozen protocol) */
#define CMD_ACK_LENGTH			(2u)
#define CMD_GET_INFO_LENGTH		(18u)
#define CMD_GET_STATUS_LENGTH	(6u)

/* TX timeout for blocking responses (18 B @250 kbps ~= 1 ms) */
#define CMD_TX_TIMEOUT_MS		(100u)

/* Discovery slot grid (frozen protocol) */
#define CMD_DISCOVER_SLOTS		(16u)
#define CMD_DISCOVER_SLOT_MS	(5u)

extern void app_CMD_Exec(void);
extern void app_CMD_Respond(const uint8_t* l_Data, uint16_t l_Length);
/* Write gate: true when no selection active (broadcast) or this unit selected */
extern bool app_CMD_WriteAllowed(void);

#endif /* APP_CMD_H_ */
