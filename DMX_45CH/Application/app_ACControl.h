/*
 * app_ACControl.h
 *
 *  Created on: Dec 16, 2023
 *      Author: OmarSevilla
 */

#ifndef SRC_APP_ACCONTROL_H_
#define SRC_APP_ACCONTROL_H_
#include "stm32f3xx_hal.h"
#include <stdbool.h>

#define ACCONTROL_BASECNTR_MAX			255u
#define ACCONTROL_DMXCNTR_MAX			255u
#define ACCONTROL_VALUE_MAX				200u
#define ACCONTROL_VALUE_LIMIT_MAX		200u
#define ACCONTROL_ONCOUNTER_MAX			100u
#define ACCONTROL_DMX_START_CHANNEL		1u

#define	ACCONTROL_N_CHANNELS			8u

#define ACCONTROL_WAVETIME_BY_4			130u /*4166 us @32us/sample*/
#define ACCONTROL_WAVETIME_BY_2			260u /*8332 us @32us/sample*/

#define ACCONTRO_DEBOUNCE_THRHLD		15u

#if(ACCONTROL_N_CHANNELS > 0 )
	#define ACCONTROL_N_CHANNELS_CFG \
			{\
			{GPIOB,	GPIO_PIN_8},  /* PB8  LAMP_1 */\
			{GPIOB,	GPIO_PIN_7},  /* PB7  LAMP_2 */\
			{GPIOB,	GPIO_PIN_6},  /* PB6  LAMP_3 */\
			{GPIOB,	GPIO_PIN_5},  /* PB5  LAMP_4 */\
			{GPIOD,	GPIO_PIN_2},  /* PD2  LAMP_5 */\
			{GPIOC,	GPIO_PIN_12}, /* PC12 LAMP_6 */\
			{GPIOC,	GPIO_PIN_11}, /* PC11 LAMP_7 */\
			{GPIOC,	GPIO_PIN_10}, /* PC10 LAMP_8 */\
			}
#endif

#define ACControl_CHX_ON(IDX)					HAL_GPIO_WritePin(ACControl_GPIO_CH[IDX].GPIOx, ACControl_GPIO_CH[IDX].GPIO_Pin, GPIO_PIN_RESET)
#define ACControl_CHX_OFF(IDX)					HAL_GPIO_WritePin(ACControl_GPIO_CH[IDX].GPIOx, ACControl_GPIO_CH[IDX].GPIO_Pin, GPIO_PIN_SET)
#define ACControl_GetZeroCrossValue()			HAL_GPIO_ReadPin(ZERO_CROSS_GPIO_Port, ZERO_CROSS_Pin)



 	extern volatile uint16_t ACControl_MainTimeCounter;
 	extern volatile bool ACControl_ZeroCrossFound;

	/* Runtime power limit (EEPROM slot 2), clamps phase values */
	extern uint8_t ACControl_ValueMax;
	extern void app_ACControl_Init(void);

	extern void app_ACControl_ITTimeBaseHandler(void);
	extern void app_ACControl_ITZeroCrossDetection(void);
	extern void app_ACControl_MainFunction(void);


#endif /* SRC_APP_ACCONTROL_H_ */