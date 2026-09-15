/*
 * app_WS2811.h
 *
 *  Created on: Jun 2, 2026
 *      Author: OmarSevilla
 */

#ifndef APP_WS2811_H_
#define APP_WS2811_H_

#include "app_DMXCore.h"

#define WS2811_DMX_OFFSET	(9u) /* DMX channels 1..8: AC lamps, channel 9 unused, LEDs from 10 */
#define WS2811_CH		(8u*(N_Channels-WS2811_DMX_OFFSET))
#define WS2811_BREAK	(160)
#define L_DATA_SIZE	(WS2811_CH + WS2811_BREAK)
#define TRUE_DATA	0xFC
#define FALSE_DATA	0xC0

extern uint8_t WS2811_Data[L_DATA_SIZE];

extern void app_WS2811_Init(void);
extern void app_WS2811_ConvertDMXData(void);

#endif /* APP_WS2811_H_ */
