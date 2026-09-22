/*
 * app_ACControl.c
 *
 *  Created on: Dec 16, 2023
 *      Author: OmarSevilla
 */
#include "main.h"
#include "app_ACControl.h"
#include "app_DMXCore.h"
#include "lib_Flash.h"

extern TIM_HandleTypeDef htim2;

volatile uint16_t ACControl_MainTimeCounter = 0;
volatile bool ACControl_ZeroCrossFound = false;
volatile uint8_t ACControl_DebounceWaveCnt = 0;
uint8_t ACControl_ValueMax = ACCONTROL_VALUE_MAX;

void app_ACControl_Init(void)
{
	uint32_t l_Slot;

	Flash_Read_Data(EEPROM_SLOT_ADDR(EEPROM_SLOT_AC_LIMIT), &l_Slot, 1u);
	if((l_Slot == 0xFFFFFFFFu) || ((uint8_t)l_Slot > ACCONTROL_VALUE_LIMIT_MAX))
	{
		ACControl_ValueMax = ACCONTROL_VALUE_MAX;
	}
	else
	{
		ACControl_ValueMax = (uint8_t)l_Slot;
	}
}
static uint16_t ACControl_OnCounter[ACCONTROL_N_CHANNELS];
static bool ACControl_Tick = false;

#if(ACCONTROL_N_CHANNELS > 0)
typedef struct
{
	GPIO_TypeDef* 	GPIOx;
	uint16_t		GPIO_Pin;
}T_GPIO_CH;

T_GPIO_CH ACControl_GPIO_CH[ACCONTROL_N_CHANNELS] = ACCONTROL_N_CHANNELS_CFG;
#endif

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#if(ACCONTROL_N_CHANNELS > 0)
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  /* NOTE: This function should not be modified, when the callback is needed,
            the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */

  	uint8_t ACControl_IDX_Selector;

  	HAL_TIM_Base_Stop(&htim2);
  	__HAL_TIM_SET_COUNTER(&htim2,0);
  	HAL_TIM_Base_Start(&htim2);
	ACControl_ZeroCrossFound = true;
	for(ACControl_IDX_Selector = 0;ACControl_IDX_Selector<ACCONTROL_N_CHANNELS;ACControl_IDX_Selector++)
	{
		ACControl_OnCounter[ACControl_IDX_Selector] = 0;
	}
#endif
}

void app_ACControl_ITTimeBaseHandler(void)
{
#if(ACCONTROL_N_CHANNELS > 0)
	if(false != ACControl_ZeroCrossFound)
	{
		if(ACControl_MainTimeCounter >= ACCONTROL_BASECNTR_MAX)
		{/* Reset Counter */
			ACControl_MainTimeCounter = 0u;
			ACControl_ZeroCrossFound = false;
		}
		else
		{
			ACControl_MainTimeCounter++;
		}
	}

	ACControl_Tick = true;
#endif
	return;
}

void app_ACControl_ITZeroCrossDetection(void)
{
#if(ACCONTROL_N_CHANNELS > 0)
	static bool l_ZeroFlag = false;
	uint8_t ACControl_IDX_Selector;
	if(HAL_GPIO_ReadPin(ZERO_CROSS_GPIO_Port, ZERO_CROSS_Pin) != false)
	{
		if(l_ZeroFlag == false)
		{
			l_ZeroFlag = true;
			ACControl_MainTimeCounter = 0u;
			ACControl_ZeroCrossFound = true;
			ACControl_DebounceWaveCnt = 0;
			for(ACControl_IDX_Selector = 0;ACControl_IDX_Selector<ACCONTROL_N_CHANNELS;ACControl_IDX_Selector++)
			{
				ACControl_OnCounter[ACControl_IDX_Selector] = 0;
			}
		}
		else
		{
			/* Do Nothing */
		}
	}
	else
	{
		if(ACControl_DebounceWaveCnt < ACCONTRO_DEBOUNCE_THRHLD)
		{
			ACControl_DebounceWaveCnt++;
		}
		else
		{
			l_ZeroFlag = false;
		}
	}
#endif
	return;
}

void app_ACControl_MainFunction(void)
{
#if(ACCONTROL_N_CHANNELS > 0)
	unsigned char ACControl_IDX_Selector;
	uint8_t ACControlTempValue;

uint32_t ACTime;
	if(1)//ACControl_Tick != false)
	{
		ACControl_Tick = false;
		for(ACControl_IDX_Selector = 0;ACControl_IDX_Selector<ACCONTROL_N_CHANNELS;ACControl_IDX_Selector++)
		{
			ACTime = __HAL_TIM_GET_COUNTER(&htim2);
			if(ACTime > ACCONTROL_DMXCNTR_MAX)
			{
				ACTime = ACCONTROL_DMXCNTR_MAX;
			}

			ACControlTempValue = DMX_Channels[(ACCONTROL_DMX_START_CHANNEL-1u)+ACControl_IDX_Selector];
			if(ACControlTempValue > ACControl_ValueMax)
			{
				ACControlTempValue = ACControl_ValueMax;
			}
			else{/* Do Nothing */}


			/* Check if Channel needs to be triggered */
			if((ACCONTROL_DMXCNTR_MAX - ACControlTempValue) < ACTime)
			{
				if(ACControl_OnCounter[ACControl_IDX_Selector] < ACCONTROL_ONCOUNTER_MAX)
				{
					ACControl_CHX_ON(ACControl_IDX_Selector);
					ACControl_OnCounter[ACControl_IDX_Selector]++;
				}
				else
				{
					ACControl_CHX_OFF(ACControl_IDX_Selector);
				}
			}
			else
			{
				ACControl_CHX_OFF(ACControl_IDX_Selector);
			}
		}
	}
#endif
}