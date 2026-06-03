/*
 * app_LIN.c
 *
 *  Created on: Jun 3, 2026
 *      Author: OmarSevilla
 */
#include "main.h"

static void UART_RxISR_8BIT_LIN(UART_HandleTypeDef *huart);

void HAL_LIN_WaitBreak_IT(UART_HandleTypeDef *huart)
{
//	/* Check that a Rx process is not already ongoing */
//	if (huart->RxState == HAL_UART_STATE_READY)
//	{
//		/* Set Reception type to reception till IDLE Event*/
//		huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
//		huart->RxEventType = HAL_UART_RXEVENT_TC;
//	}
//
//	huart->ErrorCode = HAL_UART_ERROR_NONE;
//	huart->RxState = HAL_UART_STATE_BUSY_RX;

	/* Enable the UART LIN Break Interrupt */
	ATOMIC_SET_BIT(huart->Instance->CR2, USART_CR2_LBDIE);

	/* Set the Rx ISR function pointer according to the data word length */
//	huart->RxISR = UART_RxISR_8BIT_LIN;
}

static void UART_RxISR_8BIT_LIN(UART_HandleTypeDef *huart)
{

}
