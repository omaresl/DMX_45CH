/*
 * app_CMD.c
 *
 * Binary UART command protocol (vendor DMX start code 0xEE).
 * Frame: 0xEE | CMD (1B) | LEN (1B) | payload (LEN) | checksum (1B),
 * checksum = uint8 sum of CMD + LEN + payload.
 * Responses go through the RS-485 transceiver (DE direction pin,
 * TX pin already inverted by USART config), blocking TX.
 */
#include <string.h>
#include "main.h"
#include "app_CMD.h"
#include "app_CmdBlink.h"
#include "app_DMXCore.h"
#include "app_Sequence.h"
#include "app_ACControl.h"
#include "app_WS2811.h"
#include "lib_Flash.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;
extern void HAL_LIN_WaitBreak_IT(UART_HandleTypeDef *huart);

void app_CMD_Respond(const uint8_t* l_Data, uint16_t l_Length)
{
	if((l_Data != NULL) && (l_Length > 0u))
	{
		/* Transceiver to TX */
		HAL_GPIO_WritePin(TRX_IN_DE_GPIO_Port, TRX_IN_DE_Pin, GPIO_PIN_SET);
		/* Driver enable settling (YD3082EESA: 2500 ns max) before first bit */
		DELAY_US(10);
		(void)HAL_UART_Transmit(&huart1, (uint8_t*)l_Data, l_Length, CMD_TX_TIMEOUT_MS);
		/* Back to RX (RE low = receiver enabled) and re-arm break detection */
		HAL_GPIO_WritePin(TRX_IN_DE_GPIO_Port, TRX_IN_DE_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(TRX_IN_RE_GPIO_Port, TRX_IN_RE_Pin, GPIO_PIN_RESET);
		HAL_LIN_WaitBreak_IT(&huart1);
	}
}

static void app_CMD_GetInfo(void)
{
	uint8_t l_Response[CMD_GET_INFO_LENGTH];

	/* UID 12 B | FW version 2 B | model 2 B | start address u16 (big-endian) */
	(void)memcpy(&l_Response[0u], (const uint8_t*)UID_BASE, UID_LENGTH);
	l_Response[12u] = (uint8_t)FW_VERSION_MAJOR;
	l_Response[13u] = (uint8_t)FW_VERSION_MINOR;
	l_Response[14u] = (uint8_t)((MODEL_ID >> 8u) & 0xFFu);
	l_Response[15u] = (uint8_t)(MODEL_ID & 0xFFu);
	l_Response[16u] = (uint8_t)((DMX_StartAddress >> 8u) & 0xFFu);
	l_Response[17u] = (uint8_t)(DMX_StartAddress & 0xFFu);

	app_CMD_Respond(l_Response, (uint16_t)CMD_GET_INFO_LENGTH);
}

static void app_CMD_Discover(uint8_t l_Seed)
{
	uint8_t l_IDX;
	uint8_t l_Hash = 0u;
	uint8_t l_Slot;

	/* Decorrelated backoff: UID hash ^ free-running TIM2 ^ per-round seed */
	for(l_IDX = 0u; l_IDX < UID_LENGTH; l_IDX++)
	{
		l_Hash = (uint8_t)(l_Hash ^ ((const uint8_t*)UID_BASE)[l_IDX]);
	}
	l_Hash = (uint8_t)(l_Hash ^ (uint8_t)__HAL_TIM_GET_COUNTER(&htim2) ^ l_Seed);
	l_Slot = (uint8_t)(l_Hash % CMD_DISCOVER_SLOTS);

	/* Worst case 15 * 5 ms = 75 ms + TX, well under the ~410 ms IWDG */
	if(l_Slot > 0u)
	{
		HAL_Delay((uint32_t)l_Slot * (uint32_t)CMD_DISCOVER_SLOT_MS);
	}
	app_CMD_GetInfo();
}

/* RAM selection: no selection active at boot (= broadcast writes).
 * Any SELECT activates selection bus-wide (all receivers see it);
 * only the UID match stays selected. DESELECT clears everywhere. */
static bool rb_CMD_Selected = false;
static bool rb_CMD_SelectionActive = false;

static void app_CMD_Select(void)
{
	uint8_t l_IDX;
	bool l_Match = true;

	for(l_IDX = 0u; l_IDX < UID_LENGTH; l_IDX++)
	{
		if(raw_DMX_Channels[3u + l_IDX] != ((const uint8_t*)UID_BASE)[l_IDX])
		{
			l_Match = false;
			break;
		}
	}
	rb_CMD_SelectionActive = true;
	rb_CMD_Selected = l_Match;
	if(l_Match != false)
	{
		/* Only the match responds (avoids N-1 NACK collision) */
		uint8_t l_Ack[CMD_ACK_LENGTH] = {CMD_ACK_BYTE, CMD_ACK_OK};
		app_CMD_Respond(l_Ack, (uint16_t)CMD_ACK_LENGTH);
	}
}

static void app_CMD_Deselect(void)
{
	rb_CMD_SelectionActive = false;
	rb_CMD_Selected = false;
}

/* EEPROM slots: [1] sequence limit, [2] AC limit (u8 in u32, [3] reserved) */
static void app_CMD_SetPowerLimit(uint8_t l_SeqLimit, uint8_t l_AcLimit, uint8_t l_LedLimit)
{
	uint32_t l_Block[EEPROM_N_SLOTS];
	uint8_t l_Ack[CMD_ACK_LENGTH];

	/* Not selected while a selection is active: stay silent (no NACK storm) */
	if(app_CMD_WriteAllowed() == false)
	{
		return;
	}
	l_Ack[0u] = CMD_ACK_BYTE;
	if((l_SeqLimit > SEQUENCE_DMX_LIMIT_MAX) || (l_AcLimit > ACCONTROL_VALUE_LIMIT_MAX) ||
		(l_LedLimit > LED_DMX_LIMIT_MAX))
	{
		l_Ack[1u] = CMD_NACK_INVALID;
		app_CmdBlink_Trigger(255u, 0u, 0u); /* red */
		app_CMD_Respond(l_Ack, (uint16_t)CMD_ACK_LENGTH);
		return;
	}
	/* Read-modify-write: Flash_Write_Data erases the whole page */
	Flash_Read_Data(EEPROM_START_ADDRESS, l_Block, EEPROM_N_SLOTS);
	l_Block[EEPROM_SLOT_SEQ_LIMIT] = (uint32_t)l_SeqLimit;
	l_Block[EEPROM_SLOT_AC_LIMIT] = (uint32_t)l_AcLimit;
	l_Block[EEPROM_SLOT_LED_LIMIT] = (uint32_t)l_LedLimit;
	if(Flash_Write_Data(EEPROM_START_ADDRESS, l_Block, EEPROM_N_SLOTS) != 0u)
	{
		l_Ack[1u] = CMD_NACK_INVALID;
		app_CmdBlink_Trigger(255u, 0u, 0u); /* red */
	}
	else
	{
		Sequence_DMX_MaxValue = l_SeqLimit;
		ACControl_ValueMax = l_AcLimit;
		Led_DMX_MaxValue = l_LedLimit;
		l_Ack[1u] = CMD_ACK_OK;
		app_CmdBlink_Trigger(255u, 165u, 0u); /* orange */
	}
	app_CMD_Respond(l_Ack, (uint16_t)CMD_ACK_LENGTH);
}

static void app_CMD_SetDmxAddr(void)
{	uint16_t l_Address;
	uint8_t l_Ack[CMD_ACK_LENGTH];

	/* Not selected while a selection is active: stay silent (no NACK storm) */
	if(app_CMD_WriteAllowed() == false)
	{
		return;
	}
	l_Address = (uint16_t)(((uint16_t)raw_DMX_Channels[3u] << 8u) | raw_DMX_Channels[4u]);
	l_Ack[0u] = CMD_ACK_BYTE;
	if(app_DMXCore_SetStartAddress(l_Address) != false)
	{
		l_Ack[1u] = CMD_ACK_OK;
		app_CmdBlink_Trigger(255u, 255u, 0u); /* yellow */
	}
	else
	{
		l_Ack[1u] = CMD_NACK_INVALID;
		app_CmdBlink_Trigger(255u, 0u, 0u); /* red */
	}
	app_CMD_Respond(l_Ack, (uint16_t)CMD_ACK_LENGTH);
}

bool app_CMD_WriteAllowed(void)
{
	return ((rb_CMD_SelectionActive == false) || (rb_CMD_Selected != false));
}

void app_CMD_Exec(void)
{
	uint8_t l_CMD;
	uint8_t l_LEN;
	uint8_t l_IDX;
	uint8_t l_Checksum;
	uint16_t l_EndIDX;

	/* Minimum frame: CMD + LEN + checksum */
	l_CMD = raw_DMX_Channels[1u];
	l_LEN = raw_DMX_Channels[2u];
	l_EndIDX = (uint16_t)(3u + l_LEN);
	if(l_EndIDX >= (uint16_t)N_RawChannels)
	{
		return;
	}

	l_Checksum = (uint8_t)(l_CMD + l_LEN);
	for(l_IDX = 0u; l_IDX < l_LEN; l_IDX++)
	{
		l_Checksum = (uint8_t)(l_Checksum + raw_DMX_Channels[3u + l_IDX]);
	}
	if(l_Checksum != raw_DMX_Channels[l_EndIDX])
	{
		return;
	}

	switch(l_CMD)
	{
	case CMD_GET_INFO:
		app_CmdBlink_Trigger(0u, 255u, 255u); /* cyan */
		app_CMD_GetInfo();
		break;
	case CMD_DISCOVER:
		if(l_LEN == 1u)
		{
			app_CmdBlink_Trigger(255u, 0u, 255u); /* magenta */
			app_CMD_Discover(raw_DMX_Channels[3u]);
		}
		break;
	case CMD_SELECT_UID:
		if(l_LEN == UID_LENGTH)
		{
			app_CMD_Select();
			if(rb_CMD_Selected != false)
			{
				app_CmdBlink_Trigger(0u, 255u, 0u); /* green, match only */
			}
		}
		break;
	case CMD_DESELECT:
		if(l_LEN == 0u)
		{
			app_CmdBlink_Trigger(0u, 0u, 255u); /* blue */
			app_CMD_Deselect();
		}
		break;
	case CMD_SET_DMX_ADDR:
		if(l_LEN == 2u)
		{
			app_CMD_SetDmxAddr();
		}
		break;
	case CMD_SET_POWER_LIMIT:
		if(l_LEN == 3u)
		{
			app_CMD_SetPowerLimit(raw_DMX_Channels[3u], raw_DMX_Channels[4u], raw_DMX_Channels[5u]);
		}
		break;
	case CMD_GET_STATUS:
	default:
		/* Not implemented yet: silently ignore */
		break;
	}
}
