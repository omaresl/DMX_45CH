/*
 * app_CMD.c
 *
 *  Created on: Mar 5, 2024
 *      Author: OmarSevilla
 */
#include "app_CMD.h"
#include "app_DMXCore.h"
#include "main.h"
#include "lib_Flash.h"
#include "app_Sequence.h"

extern UART_HandleTypeDef huart1;
extern T_Sequence IndicatorSequence[];


uint32_t EEPROM_BlockTable[4u];

#define N_RECEIVE_COMMANDS	2
/* Receive Commands */
char str_DMXIDREQ[] = {"\f\022DMX ID REQUEST\n\t00"};
char str_SETDMXADR[] = {"\4\x1D""COMMAND SET DMX ADDRESS\n\t00"};

typedef struct
{
	T_CMD	CommandAssigned;
	char* 	CommandTxtPtr;
	uint8_t CommandSize;
}T_COMMAND_TXT;

T_COMMAND_TXT CommandTable[N_RECEIVE_COMMANDS] =
{
		{CMD_ID_REQ, str_DMXIDREQ,sizeof(str_DMXIDREQ)- 1},
		{CMD_SET_ADR, str_SETDMXADR,sizeof(str_SETDMXADR) - 3},
};
/* Responses */
char str_DMXIDREQ_RES[] = {"\r\023DMX ID RESPONSE"};

/***********************************
 * Name: app_CMD_GetCMD
 * Description: Get The CMD based on Buffer Data
 * ********************************/
T_CMD app_CMD_GetCMD(uint8_t* RxBuffer, uint8_t BufferSize)
{
	T_CMD l_RetVal = CMD_NOT_FOUND;
	uint8_t l_CompareIDX;
	uint8_t l_CommandTableIDX = 0;

	str_DMXIDREQ[sizeof(str_DMXIDREQ) - 3u] = 0;
	str_DMXIDREQ[sizeof(str_DMXIDREQ) - 2u] = 0;

	l_CommandTableIDX = 1u;

	/* Compare String Received */
	for(
			l_CompareIDX = 0;
			l_CompareIDX < BufferSize;
			l_CompareIDX++
	)
	{
		if(((char)RxBuffer[l_CompareIDX] != CommandTable[l_CommandTableIDX].CommandTxtPtr[l_CompareIDX]))
		{
			break;
		}
		else
		{
			/* Do Nothing */
		}
	}

	/* Command Found */
	if(l_CompareIDX == (uint8_t)CommandTable[l_CommandTableIDX].CommandSize)
	{
		l_RetVal = CommandTable[l_CommandTableIDX].CommandAssigned;
	}
	/* Command Not Found */
	else
	{
		l_RetVal = CMD_NOT_FOUND;
	}

	return l_RetVal;
}

/***********************************
 * Name: app_CMD_ExecCMD
 * Description: Execute The CMD based on Buffer Data
 * ********************************/
bool app_CMD_ExecCMD(T_CMD l_CMD, uint8_t* RxBuffer, uint8_t BufferSize)
{
	bool lb_Result;

	lb_Result = false;
	switch(l_CMD)
	{
	case CMD_SET_ADR:
	{
		uint16_t NewDMXAdress;
		/* Get Address */
		NewDMXAdress = (uint16_t)(RxBuffer[CommandTable[CMD_SET_ADR].CommandSize + 2u] << 8u) | (uint16_t)RxBuffer[CommandTable[CMD_SET_ADR].CommandSize + 3u];
		if((NewDMXAdress != 0) &&
				(NewDMXAdress != DMX_StartAddress))
		{
			DMX_StartAddress = NewDMXAdress;
			EEPROM_BlockTable[0] = DMX_StartAddress;
			Flash_Write_Data(EEPROM_START_ADDRESS, &EEPROM_BlockTable[0], 1u);
			app_SequenceCommandExec(IndicatorSequence);
			lb_Result = true;
		}
		/* Set DMX Address */
	}break;
	case CMD_ID_REQ:
	{
		uint16_t l_ID;

		/* Get ID Requested*/
		l_ID = (uint16_t)(((uint16_t)RxBuffer[BufferSize - 2u] << 8u )|
				((uint16_t)RxBuffer[BufferSize - 1u]));
		/* Compare ID with uC ID */
		if(1)//200 == l_ID)
		{/* ID Found */
			uint8_t CMD_ID_REQ_TX_Buffer[4];

			CMD_ID_REQ_TX_Buffer[1] = (uint8_t)(l_ID & 0x00FF);
			CMD_ID_REQ_TX_Buffer[0] = (uint8_t)((l_ID & 0xFF00) >> 8u);
			CMD_ID_REQ_TX_Buffer[3] = (uint8_t)(DMX_StartAddress & 0x00FF);
			CMD_ID_REQ_TX_Buffer[2] = (uint8_t)((DMX_StartAddress & 0xFF00) >> 8u);

			/* Set Transmission CFG */
			HAL_GPIO_WritePin(TRX_IN_DE_GPIO_Port, TRX_IN_DE_Pin, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(TRX_IN_RE_GPIO_Port, TRX_IN_RE_Pin, GPIO_PIN_SET);
			huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_TXINVERT_INIT | UART_ADVFEATURE_RXOVERRUNDISABLE_INIT|UART_ADVFEATURE_DMADISABLEONERROR_INIT;
			huart1.AdvancedInit.TxPinLevelInvert = UART_ADVFEATURE_TXINV_ENABLE;
			__HAL_UART_DISABLE(&huart1);
			UART_AdvFeatureConfig(&huart1);
			__HAL_UART_ENABLE(&huart1);
			/* Transmit Response */
			DELAY_US(150);

			huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_TXINVERT_INIT | UART_ADVFEATURE_RXOVERRUNDISABLE_INIT|UART_ADVFEATURE_DMADISABLEONERROR_INIT;
			huart1.AdvancedInit.TxPinLevelInvert = UART_ADVFEATURE_TXINV_DISABLE;
			__HAL_UART_DISABLE(&huart1);
			UART_AdvFeatureConfig(&huart1);
			__HAL_UART_ENABLE(&huart1);
			HAL_UART_Transmit(&huart1, (uint8_t*)str_DMXIDREQ_RES, (sizeof(str_DMXIDREQ_RES) - 1u),HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart1, (uint8_t*)CMD_ID_REQ_TX_Buffer, sizeof(CMD_ID_REQ_TX_Buffer),HAL_MAX_DELAY);

			/* Set Receive CFG */
			HAL_GPIO_WritePin(TRX_IN_DE_GPIO_Port, TRX_IN_DE_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(TRX_IN_RE_GPIO_Port, TRX_IN_RE_Pin, GPIO_PIN_RESET);

			HAL_UART_Receive_IT(&huart1, DMX_Channels, N_Channels);

			DMX_State = DMX_State_BREAK;
		}
		else
		{/* Wrong ID */
			/* Do Nothing */
		}

	}break;
	case CMD_NOT_FOUND:
	case CMD_N:
	default:
	{
		/* Do nothing */
	}break;
	}

	return lb_Result;
}
