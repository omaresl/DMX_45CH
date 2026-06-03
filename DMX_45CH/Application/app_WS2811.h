/*
 * app_WS2811.h
 *
 *  Created on: Jun 2, 2026
 *      Author: OmarSevilla
 */

#ifndef APP_WS2811_H_
#define APP_WS2811_H_

#define WS2811_CH		(8*45)
#define WS2811_BREAK	(160)
#define L_DATA_SIZE	(WS2811_CH + WS2811_BREAK)
#define TRUE_DATA	0xFC
#define FALSE_DATA	0xC0

extern uint8_t WS2811_Data[L_DATA_SIZE];

extern void app_WS2811_Init(void);
extern void app_WS2811_ConvertDMXData(void);

#endif /* APP_WS2811_H_ */
