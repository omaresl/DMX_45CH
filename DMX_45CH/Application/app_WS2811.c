/*
 * app_WS2811.c
 *
 *  Created on: Jun 2, 2026
 *      Author: OmarSevilla
 */

#include <stdint.h>
#include <string.h>
#include "app_WS2811.h"
#include "app_DMXCore.h"

uint8_t WS2811_Data[L_DATA_SIZE];

void app_WS2811_Init(void)
{
	memset(WS2811_Data,TRUE_DATA,WS2811_CH);
	memset(&WS2811_Data[WS2811_CH],0x00,WS2811_BREAK);
}

void app_WS2811_ConvertDMXData(void)
{
	uint8_t lub_BitCount;
	uint16_t WS2811_Data_IDX;
	uint16_t DMX_Data_IDX;

	WS2811_Data_IDX = 0;

	for(DMX_Data_IDX = 0; DMX_Data_IDX < N_Channels; DMX_Data_IDX++)
	{
		/* Convert 1 byte to 8bits */
		for(lub_BitCount = 0; lub_BitCount < 8u; lub_BitCount++)
		{
			if(((0xFF - DMX_Channels[DMX_Data_IDX]) & (1 << (7u - lub_BitCount))) != 0)
			{
				WS2811_Data[WS2811_Data_IDX] = TRUE_DATA;
			}
			else
			{
				WS2811_Data[WS2811_Data_IDX] = FALSE_DATA;
			}
			WS2811_Data_IDX++;
		}
	}
}
