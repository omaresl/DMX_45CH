/*
 * app_CmdBlink.h
 *
 * Visual ACK: 5 Hz strobe (color vs DMX state) on the LED channels for
 * accepted commands. Non-blocking (HAL_GetTick); DMX_Channels untouched,
 * output restores alone.
 */

#ifndef APP_CMDBLINK_H_
#define APP_CMDBLINK_H_

#include <stdint.h>
#include <stdbool.h>

#define CMD_BLINK_MS	(1000u)
#define CMD_STROBE_HALF_MS	(100u)

extern void app_CmdBlink_Trigger(uint8_t l_R, uint8_t l_G, uint8_t l_B);
/* Returns true while blink active (caller must skip normal conversion) */
extern bool app_CmdBlink_Apply(void);

#endif /* APP_CMDBLINK_H_ */
