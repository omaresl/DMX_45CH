/*
 * app_DMXCore.c
 *
 *  Created on: Dec 11, 2023
 *      Author: OmarSevilla
 */
#include "app_DMXCore.h"
#include "app_CMD.h"
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
	Flash_Read_Data(EEPROM_SLOT_ADDR(EEPROM_SLOT_DMX_START), EEPROM_BlockTable, 1u);
	if(EEPROM_BlockTable[EEPROM_SLOT_DMX_START] > (N_RawChannels - N_Channels))
	{
		DMX_StartAddress = 1u;
		EEPROM_BlockTable[EEPROM_SLOT_DMX_START] = DMX_StartAddress;
		Flash_Write_Data(EEPROM_SLOT_ADDR(EEPROM_SLOT_DMX_START), &EEPROM_BlockTable[EEPROM_SLOT_DMX_START], 1u);
	}
	else
	{
		DMX_StartAddress = (uint16_t)EEPROM_BlockTable[EEPROM_SLOT_DMX_START];
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