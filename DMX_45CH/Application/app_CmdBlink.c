/*
 * app_CmdBlink.c
 *
 * Visual ACK: solid RGB blink on the LED channels for accepted commands.
 * Fills WS2811_Data with the color bit encoding (0xFC/0xC0, same convention
 * as app_WS2811_ConvertDMXData) while active; DMX_Channels untouched.
 */
#include "main.h"
#include "app_CmdBlink.h"
#include "app_WS2811.h"

static bool rb_BlinkActive = false;
static uint32_t rul_BlinkExpiry = 0u;
static uint8_t rub_BlinkRGB[3u];

void app_CmdBlink_Trigger(uint8_t l_R, uint8_t l_G, uint8_t l_B)
{
	rub_BlinkRGB[0u] = l_R;
	rub_BlinkRGB[1u] = l_G;
	rub_BlinkRGB[2u] = l_B;
	rul_BlinkExpiry = HAL_GetTick() + (uint32_t)CMD_BLINK_MS;
	rb_BlinkActive = true;
}

bool app_CmdBlink_Apply(void)
{
	uint16_t l_IDX;
	uint8_t l_Bit;
	uint8_t l_Value;

	if(rb_BlinkActive == false)
	{
		return false;
	}
	/* int32_t subtraction handles HAL_GetTick wraparound */
	if((int32_t)(HAL_GetTick() - rul_BlinkExpiry) >= 0)
	{
		rb_BlinkActive = false;
		return false;
	}
	for(l_IDX = 0u; l_IDX < (uint16_t)(N_Channels - WS2811_DMX_OFFSET); l_IDX++)
	{
		l_Value = rub_BlinkRGB[l_IDX % 3u];
		for(l_Bit = 0u; l_Bit < 8u; l_Bit++)
		{
			if(((0xFFu - (uint16_t)l_Value) & (1u << (7u - l_Bit))) != 0u)
			{
				WS2811_Data[(uint16_t)((uint16_t)l_IDX * 8u) + l_Bit] = TRUE_DATA;
			}
			else
			{
				WS2811_Data[(uint16_t)((uint16_t)l_IDX * 8u) + l_Bit] = FALSE_DATA;
			}
		}
	}
	return true;
}
