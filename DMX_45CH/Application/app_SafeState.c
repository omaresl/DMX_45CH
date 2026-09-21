/*
 * app_SafeState.c
 *
 * Drives all power outputs to their safe (OFF) state:
 * LAMP_1..8 are open-drain, OFF = SET; LED_CONTROL_SIGNAL (PA2)
 * is LOW on HW Con Resistencia, HIGH only on HW Sin Resistencia.
 * Uses BSRR/BRR single writes: atomic and HAL-independent.
 */
#include "main.h"
#include "app_SafeState.h"

void app_SafeState_OutputsOff(void)
{
	/* LAMP_1..4 (PB8/PB7/PB6/PB5), LAMP_5 (PD2), LAMP_6..8 (PC12/PC11/PC10): OFF = SET */
	GPIOB->BSRR = (uint32_t)(LAMP_4_Pin | LAMP_3_Pin | LAMP_2_Pin | LAMP_1_Pin);
	GPIOD->BSRR = (uint32_t)LAMP_5_Pin;
	GPIOC->BSRR = (uint32_t)(LAMP_8_Pin | LAMP_7_Pin | LAMP_6_Pin);
#ifdef HW_SIN_RESISTENCIA
	GPIOA->BSRR = (uint32_t)LED_CONTROL_SIGNAL_Pin;
#else
	GPIOA->BRR = (uint32_t)LED_CONTROL_SIGNAL_Pin;
#endif
}
