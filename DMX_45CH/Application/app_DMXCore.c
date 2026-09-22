/*
 * app_DMXCore.c
 *
 *  Created on: Dec 11, 2023
 *      Author: OmarSevilla
 */
#include "app_DMXCore.h"
#include "app_CMD.h"
#include "app_Sequence.h"
#include "app_ACControl.h"
#include "app_WS2811.h"
#include "lib_Flash.h"
#include "main.h"

uint8_t raw_DMX_Channels[N_RawChannels];
uint8_t DMX_Channels[N_Channels];
uint16_t DMX_StartAddress = DMX_INIT_START_ADDRESS;
bool DMX2DMATransferFlag = false;
uint32_t EEPROM_BlockTable[4u];

void app_DMX_DMATransfer(void)
{
	uint16_t l_DMX_ChannelIDX;
	if(DMX2DMATransferFlag != false)
	{
		DMX2DMATransferFlag = false;
		if(raw_DMX_Channels[0u] == CMD_START_CODE)
		{
			/* Command frame: process instead of channel copy */
			app_CMD_Exec();
		}
		else
		{
			for(l_DMX_ChannelIDX = 0; l_DMX_ChannelIDX < N_Channels;l_DMX_ChannelIDX++)
			{
				if((DMX_StartAddress + l_DMX_ChannelIDX) <= N_RawChannels)
				{
					DMX_Channels[l_DMX_ChannelIDX] = raw_DMX_Channels[DMX_StartAddress + l_DMX_ChannelIDX + 1u];
				}
				else
				{
					DMX_Channels[l_DMX_ChannelIDX] = 0;
				}
			}
		}
	}
}

void app_DMXCore_Init(void)
{
	bool l_Dirty = false;

	Flash_Read_Data(EEPROM_START_ADDRESS, EEPROM_BlockTable, EEPROM_N_SLOTS);
	if(EEPROM_BlockTable[EEPROM_SLOT_DMX_START] > (uint32_t)DMX_MAX_START_ADDRESS)
	{
		DMX_StartAddress = 1u;
		EEPROM_BlockTable[EEPROM_SLOT_DMX_START] = DMX_StartAddress;
		l_Dirty = true;
	}
	else
	{
		DMX_StartAddress = (uint16_t)EEPROM_BlockTable[EEPROM_SLOT_DMX_START];
	}
	/* Virgin (0xFFFFFFFF) or garbage slots take defaults and persist them */
	if((EEPROM_BlockTable[EEPROM_SLOT_SEQ_LIMIT] == 0xFFFFFFFFu) ||
		((uint8_t)EEPROM_BlockTable[EEPROM_SLOT_SEQ_LIMIT] > SEQUENCE_DMX_LIMIT_MAX))
	{
		EEPROM_BlockTable[EEPROM_SLOT_SEQ_LIMIT] = (uint32_t)SEQUENCE_DMX_MAX_VALUE;
		l_Dirty = true;
	}
	if((EEPROM_BlockTable[EEPROM_SLOT_AC_LIMIT] == 0xFFFFFFFFu) ||
		((uint8_t)EEPROM_BlockTable[EEPROM_SLOT_AC_LIMIT] > ACCONTROL_VALUE_LIMIT_MAX))
	{
		EEPROM_BlockTable[EEPROM_SLOT_AC_LIMIT] = (uint32_t)ACCONTROL_VALUE_MAX;
		l_Dirty = true;
	}
	if((EEPROM_BlockTable[EEPROM_SLOT_LED_LIMIT] == 0xFFFFFFFFu) ||
		((uint8_t)EEPROM_BlockTable[EEPROM_SLOT_LED_LIMIT] > LED_DMX_LIMIT_MAX))
	{
		EEPROM_BlockTable[EEPROM_SLOT_LED_LIMIT] = (uint32_t)LED_DMX_DEFAULT;
		l_Dirty = true;
	}
	if(l_Dirty != false)
	{
		/* Single page erase, first boot only */
		Flash_Write_Data(EEPROM_START_ADDRESS, EEPROM_BlockTable, EEPROM_N_SLOTS);
	}
}

bool app_DMXCore_SetStartAddress(uint16_t l_Address)
{
	if((l_Address < 1u) || (l_Address > DMX_MAX_START_ADDRESS))
	{
		return false;
	}
	/* Read-modify-write: Flash_Write_Data erases the whole page */
	Flash_Read_Data(EEPROM_START_ADDRESS, EEPROM_BlockTable, EEPROM_N_SLOTS);
	EEPROM_BlockTable[EEPROM_SLOT_DMX_START] = (uint32_t)l_Address;
	if(Flash_Write_Data(EEPROM_START_ADDRESS, EEPROM_BlockTable, EEPROM_N_SLOTS) != 0u)
	{
		return false;
	}
	DMX_StartAddress = l_Address;
	return true;
}