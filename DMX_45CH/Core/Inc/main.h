/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define WAITCOUNTERMAX	15u
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define SYSTICK_LOAD (/*SystemCoreClock*/48000000U/1000000U)
#define SYSTICK_DELAY_CALIB (SYSTICK_LOAD >> 1)

#define DELAY_US(us) \
    do { \
         uint32_t start = SysTick->VAL; \
         uint32_t ticks = (us * SYSTICK_LOAD)-SYSTICK_DELAY_CALIB;  \
         while((start - SysTick->VAL) < ticks); \
    } while (0)
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern TIM_HandleTypeDef htim6;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_CONTROL_SIGNAL_Pin GPIO_PIN_2
#define LED_CONTROL_SIGNAL_GPIO_Port GPIOA
#define TRX_IN_DE_Pin GPIO_PIN_3
#define TRX_IN_DE_GPIO_Port GPIOA
#define TRX_IN_RE_Pin GPIO_PIN_4
#define TRX_IN_RE_GPIO_Port GPIOF
#define ZERO_CROSS_Pin GPIO_PIN_12
#define ZERO_CROSS_GPIO_Port GPIOB
#define ZERO_CROSS_EXTI_IRQn EXTI15_10_IRQn
#define LAMP_8_Pin GPIO_PIN_10
#define LAMP_8_GPIO_Port GPIOC
#define LAMP_7_Pin GPIO_PIN_11
#define LAMP_7_GPIO_Port GPIOC
#define LAMP_6_Pin GPIO_PIN_12
#define LAMP_6_GPIO_Port GPIOC
#define LAMP_5_Pin GPIO_PIN_2
#define LAMP_5_GPIO_Port GPIOD
#define LAMP_4_Pin GPIO_PIN_5
#define LAMP_4_GPIO_Port GPIOB
#define LAMP_3_Pin GPIO_PIN_6
#define LAMP_3_GPIO_Port GPIOB
#define LAMP_2_Pin GPIO_PIN_7
#define LAMP_2_GPIO_Port GPIOB
#define LAMP_1_Pin GPIO_PIN_8
#define LAMP_1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
