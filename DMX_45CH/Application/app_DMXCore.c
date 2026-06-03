/*
 * app_DMXCore.c
 *
 *  Created on: Dec 11, 2023
 *      Author: OmarSevilla
 */
#include "app_DMXCore.h"
#include "app_Sequence.h"
#include "lib_Flash.h"
#include "app_CMD.h"
#include "main.h"

extern UART_HandleTypeDef huart1;

uint8_t raw_DMX_Channels[N_RawChannels];
uint8_t DMX_Channels[N_Channels];
uint8_t DMX_CMD_Buffer[DMX_CMD_BUFFER_SIZE];
uint8_t DMX_CMD_BufferIDX = 0u;
uint32_t DMXtimeoutCounter;
static uint16_t DMX_ChannelIDX;
static uint16_t DMX_ChannelSkipCounter;
T_DMXState DMX_State = DMX_State_BREAK;
uint16_t DMX_StartAddress = DMX_INIT_START_ADDRESS;
bool DMX2DMATransferFlag = false;

void app_DMX_DMATransfer(void)
{
	uint16_t l_DMX_ChannelIDX;
	if(DMX2DMATransferFlag != false)
	{
		DMX2DMATransferFlag = false;
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

static void app_DMX_StoreData(uint8_t data, uint32_t errorflags);

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

void app_DMX_RX_ITHandler(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   = READ_REG(huart->Instance->ISR);
	uint8_t RX_Data;
	uint32_t errorflags;

	HAL_NVIC_DisableIRQ(USART1_IRQn);
	HAL_NVIC_DisableIRQ(TIM2_IRQn);
	RX_Data = (uint8_t)huart->Instance->RDR;
	DMXtimeoutCounter = 0;
	rb_EnableSequenceFlag = false;
	switch(DMX_State)
	{
	case DMX_State_BREAK:
	{
		errorflags = huart->ErrorCode;
		if(((errorflags & HAL_UART_ERROR_FE) == HAL_UART_ERROR_FE) &&
				(RX_Data == 0u))
		{
			DMX_State = DMX_State_START;
		}
	}break;
	case DMX_State_START:
	{
		/* If no error occurs */
		errorflags = 0;//(isrflags & (uint32_t)(/*USART_ISR_IDLE |*/ USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE | USART_ISR_RTOF));
		if(errorflags == 0)/* Valid Data Incoming */
		{
			if(RX_Data == 0)/* Valid DMX Data */
			{
				DMX_ChannelIDX = 0;
				DMX_ChannelSkipCounter = 0;
				if(DMX_StartAddress <= (DMX_ChannelIDX + 1u))
				{
					DMX_State = DMX_State_RX;
				}
				else
				{
					DMX_State = DMX_State_SKIP;
				}
			}
			else /* No DMX Data */
			{
				/* Store Incoming Data */
				DMX_CMD_BufferIDX = 0;
				DMX_CMD_Buffer[DMX_CMD_BufferIDX] = RX_Data;
				if(DMX_CMD_Buffer[DMX_CMD_BufferIDX] == str_SETDMXADR[DMX_CMD_BufferIDX])
				{
				/* Receive CMD State */
					DMX_State = DMX_State_RXCMD;
					DMX_CMD_BufferIDX++;
				}
				else
				{
					DMX_State = DMX_State_BREAK;
				}
			}
		}
		else
		{/* Wait for IDLE */
			DMX_State = DMX_State_BREAK;
		}
	}break;
	case DMX_State_RXCMD:
	{
		/* If no error occurs */
		errorflags = 0;//(isrflags & (uint32_t)USART_ISR_FE);
		if(errorflags == 0)/* Valid Data Incoming */
		{
			/* Store Incoming Data */
			DMX_CMD_Buffer[DMX_CMD_BufferIDX] = RX_Data;
			if(DMX_CMD_BufferIDX < DMX_CMD_BUFFER_SIZE)
			{
				DMX_CMD_BufferIDX++;
			}
			else
			{
				DMX_State = DMX_State_BREAK;
			}
		}
		else
		{/* Wait for IDLE */
			DMX_State = DMX_State_BREAK;
		}
	}break;
	case DMX_State_EXECMD:
	{
		/* Do Nothing */
	}break;
	case DMX_State_SKIP:
	{
		/* If no error occurs */
		errorflags = 0;//(isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE | USART_ISR_RTOF));
		if (errorflags == 0U)
		{
			DMX_ChannelSkipCounter++;
			if(DMX_StartAddress <= (DMX_ChannelSkipCounter + 1u))
			{
				DMX_State = DMX_State_RX;
			}
			else
			{
				DMX_State = DMX_State_SKIP;
			}
		}
		else
		{/* Wait for IDLE */
			DMX_State = DMX_State_BREAK;
		}
	}break;
	case DMX_State_RX:
	{
		/* If no error occurs */
		errorflags = (isrflags & USART_ISR_FE);

		/* Start Reception */
		app_DMX_StoreData(RX_Data,errorflags);

	}break;
	case DMX_State_EE:
	{
		EEPROM_BlockTable[0] = DMX_StartAddress;
		Flash_Write_Data(EEPROM_START_ADDRESS, &EEPROM_BlockTable[0], 1u);
		DMX_State = DMX_State_BREAK;
	}break;
	case DMX_State_IDLE:
	case DMX_State_N:
	default:
	{/* Wait for IDLE */
		DMX_State = DMX_State_BREAK;
	}break;
	}

	/* Clear All Flags */
	__HAL_UART_CLEAR_FLAG(huart,0xFFFFFFFFu);

	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

static void app_DMX_StoreData(uint8_t data, uint32_t errorflags)
{
	if(DMX_ChannelIDX < N_Channels)
	{
		if(errorflags == 0)
		{
			DMX_Channels[DMX_ChannelIDX] = data;
		}
		DMX_ChannelIDX++;
	}
	else
	{
		DMX_State = DMX_State_BREAK;
	}
}
