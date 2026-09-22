/*
 * app_WS2811.c
 *
 *  Created on: Jun 2, 2026
 *      Author: OmarSevilla
 */

#include <stdint.h>
#include <string.h>
#include "app_WS2811.h"
#include "app_CmdBlink.h"
#include "app_DMXCore.h"
#include "app_Sequence.h"
#include "lib_Flash.h"

uint8_t WS2811_Data[L_DATA_SIZE];
uint8_t Led_DMX_MaxValue = LED_DMX_DEFAULT;

void app_WS2811_Init(void)
{
	uint32_t l_Slot;

	memset(WS2811_Data,TRUE_DATA,WS2811_CH);
	memset(&WS2811_Data[WS2811_CH],0x00,WS2811_BREAK);
	Flash_Read_Data(EEPROM_SLOT_ADDR(EEPROM_SLOT_LED_LIMIT), &l_Slot, 1u);
	if((l_Slot == 0xFFFFFFFFu) || ((uint8_t)l_Slot > LED_DMX_LIMIT_MAX))
	{
		Led_DMX_MaxValue = LED_DMX_DEFAULT;
	}
	else
	{
		Led_DMX_MaxValue = (uint8_t)l_Slot;
	}
}

void app_WS2811_ConvertDMXData(void)
{
	uint8_t lub_BitCount;
	uint16_t WS2811_Data_IDX;
	uint16_t DMX_Data_IDX;
	uint8_t lub_ChannelValue;

	WS2811_Data_IDX = 0;

	/* Visual command ACK overrides LED output while active */
	if(app_CmdBlink_Apply() != false)
	{
		return;
	}

	for(DMX_Data_IDX = WS2811_DMX_OFFSET; DMX_Data_IDX < N_Channels; DMX_Data_IDX++)
	{
		/* LED power limit: also clamps live DMX values, DMX_Channels untouched */
		lub_ChannelValue = DMX_Channels[DMX_Data_IDX];
		if(lub_ChannelValue > Led_DMX_MaxValue)
		{
			lub_ChannelValue = Led_DMX_MaxValue;
		}
		/* Convert 1 byte to 8bits */
		for(lub_BitCount = 0; lub_BitCount < 8u; lub_BitCount++)
		{
			if(((0xFF - lub_ChannelValue) & (1 << (7u - lub_BitCount))) != 0)
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
