/*
 * app_LIN.c
 *
 *  Created on: Jun 3, 2026
 *      Author: OmarSevilla
 */
#include "main.h"

void HAL_LIN_WaitBreak_IT(UART_HandleTypeDef *huart)
{
	/* Enable the UART LIN Break Interrupt */
	ATOMIC_SET_BIT(huart->Instance->CR2, USART_CR2_LBDIE);
}
