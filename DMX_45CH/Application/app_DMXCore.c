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
	Flash_Read_Data(EEPROM_START_ADDRESS, EEPROM_BlockTable, 1u);
	if(EEPROM_BlockTable[0] > (N_RawChannels - N_Channels))
	{
		DMX_StartAddress = 1u;
		EEPROM_BlockTable[0] = DMX_StartAddress;
		Flash_Write_Data(EEPROM_START_ADDRESS, &EEPROM_BlockTable[0], 1u);
	}
	else
	{
		DMX_StartAddress = (uint16_t)EEPROM_BlockTable[0];
	}
}