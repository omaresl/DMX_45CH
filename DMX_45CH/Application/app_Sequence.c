/*
 * app_Sequence.c
 *
 *  Created on: Jan 10, 2024
 *      Author: Omare
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "app_WS2811.h"
#include "app_Sequence.h"
#include "app_DMXCore.h"

bool rb_EnableSequenceFlag = false;
bool rb_SequenceTick = false;
uint32_t rul_SeqIndex;

#define SEQUENCE_FADEUP_CH(channel,data)\
		{SEQ_SELECT_CH, 	channel		}, 	/* SelectCH */\
		{SEQ_FADEUP, 		data		}

#define SEQUENCE_FADEDOWN_CH(channel,data)\
		{SEQ_SELECT_CH, 	channel		}, 	/* SelectCH */\
		{SEQ_FADEDOWN, 		data		}

#define SEQUENCE_WAIT(time)\
		{SEQ_SET_TIME, 		time	}, 	/* Set Time */\
		{SEQ_TIME_WAIT, 	0u		} 	/* Wait Time */

/* AC Sequence Config (DMX channels 0..7, index 8 unused) */
#define AC_CASCADE_STAGGER		8u
#define AC_CASCADE_FADE_TICK	2u
#define AC_CASCADE_HOLD			30u

#define AC_CASCADE_RISE(ch)\
		SEQUENCE_WAIT(AC_CASCADE_STAGGER),\
		{SEQ_INIT_LOOP, SEQUENCE_DMX_MAX_VALUE},\
		SEQUENCE_FADEUP_CH(ch,1u),\
		SEQUENCE_WAIT(AC_CASCADE_FADE_TICK),\
		{SEQ_CHECK_LOOP, 0u}

#define AC_CASCADE_FALL(ch)\
		SEQUENCE_WAIT(AC_CASCADE_STAGGER),\
		{SEQ_INIT_LOOP, SEQUENCE_DMX_MAX_VALUE},\
		SEQUENCE_FADEDOWN_CH(ch,1u),\
		SEQUENCE_WAIT(AC_CASCADE_FADE_TICK),\
		{SEQ_CHECK_LOOP, 0u}

T_Sequence ACSequence[] = {
		/* Cascade UP CH0..CH7 */
		AC_CASCADE_RISE(0),
		AC_CASCADE_RISE(1),
		AC_CASCADE_RISE(2),
		AC_CASCADE_RISE(3),
		AC_CASCADE_RISE(4),
		AC_CASCADE_RISE(5),
		AC_CASCADE_RISE(6),
		AC_CASCADE_RISE(7),
		SEQUENCE_WAIT(AC_CASCADE_HOLD),
		/* Cascade DOWN CH7..CH0 */
		AC_CASCADE_FALL(7),
		AC_CASCADE_FALL(6),
		AC_CASCADE_FALL(5),
		AC_CASCADE_FALL(4),
		AC_CASCADE_FALL(3),
		AC_CASCADE_FALL(2),
		AC_CASCADE_FALL(1),
		AC_CASCADE_FALL(0),
		SEQUENCE_WAIT(AC_CASCADE_HOLD),
		{SEQ_JUMP, 			0u		},	/* Start Over Again */
};

/* LED Rainbow Sequence (DMX channels 9..53 = 15 RGB pixels) */
#define RAINBOW_LED_PIXELS		((N_Channels - WS2811_DMX_OFFSET) / 3u)
#define RAINBOW_HUE_STEP		17u
#define RAINBOW_HUE_SPEED		1u
#define RAINBOW_FRAME_DIV		8u

static T_SequenceState ACSequence_State;
static uint8_t Rainbow_BaseHue;
static uint16_t Rainbow_FrameCounter;

static void Sequence_RainbowWheelRGB(uint8_t l_RainbowHue,
									 uint8_t* l_RainbowR,
									 uint8_t* l_RainbowG,
									 uint8_t* l_RainbowB)
{
	uint8_t l_RainbowSeg;
	uint8_t l_RainbowPos;
	uint8_t l_RainbowValue;

	l_RainbowSeg   = l_RainbowHue / 43u;
	l_RainbowPos   = l_RainbowHue % 43u;
	l_RainbowValue = (uint8_t)(((uint32_t)l_RainbowPos * 255u) / 42u);

	switch(l_RainbowSeg)
	{
	case 0u:
		*l_RainbowR = 255u;
		*l_RainbowG = l_RainbowValue;
		*l_RainbowB = 0u;
		break;
	case 1u:
		*l_RainbowR = (uint8_t)(255u - l_RainbowValue);
		*l_RainbowG = 255u;
		*l_RainbowB = 0u;
		break;
	case 2u:
		*l_RainbowR = 0u;
		*l_RainbowG = 255u;
		*l_RainbowB = l_RainbowValue;
		break;
	case 3u:
		*l_RainbowR = 0u;
		*l_RainbowG = (uint8_t)(255u - l_RainbowValue);
		*l_RainbowB = 255u;
		break;
	case 4u:
		*l_RainbowR = l_RainbowValue;
		*l_RainbowG = 0u;
		*l_RainbowB = 255u;
		break;
	default:
		*l_RainbowR = 255u;
		*l_RainbowG = 0u;
		*l_RainbowB = (uint8_t)(255u - l_RainbowValue);
		break;
	}
}

static void Sequence_LED_RainbowUpdate(void)
{
	uint8_t l_RainbowPixel;
	uint8_t l_RainbowChannel;
	uint8_t l_RainbowRGB[3u];

	for(l_RainbowPixel = 0u; l_RainbowPixel < RAINBOW_LED_PIXELS; l_RainbowPixel++)
	{
		Sequence_RainbowWheelRGB((uint8_t)(Rainbow_BaseHue + (l_RainbowPixel * RAINBOW_HUE_STEP)),
								&l_RainbowRGB[0u],
								&l_RainbowRGB[1u],
								&l_RainbowRGB[2u]);

		l_RainbowChannel = WS2811_DMX_OFFSET + (l_RainbowPixel * 3u);
		DMX_Channels[l_RainbowChannel + 0u] = (uint8_t)(((uint32_t)l_RainbowRGB[0u] * SEQUENCE_DMX_MAX_VALUE) / 255u);
		DMX_Channels[l_RainbowChannel + 1u] = (uint8_t)(((uint32_t)l_RainbowRGB[1u] * SEQUENCE_DMX_MAX_VALUE) / 255u);
		DMX_Channels[l_RainbowChannel + 2u] = (uint8_t)(((uint32_t)l_RainbowRGB[2u] * SEQUENCE_DMX_MAX_VALUE) / 255u);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  if(rb_EnableSequenceFlag == false)
  {
	  memset(raw_DMX_Channels,0x00,N_RawChannels);
	  memset(DMX_Channels,0x00,N_Channels);
	  rb_EnableSequenceFlag = true;
	  rb_SequenceTick = true;
	  HAL_TIM_Base_Stop_IT(&htim6);
	  __HAL_TIM_SET_PRESCALER(&htim6,480);
	  __HAL_TIM_SET_AUTORELOAD(&htim6,(100));
	  HAL_TIM_Base_Start_IT(&htim6);
  }
  else
  {
	  rb_SequenceTick = true;
  }

}


static void app_Sequence_LimitChannels(void)
{
	uint8_t l_IDX;

	for(l_IDX = 0u; l_IDX < N_Channels; l_IDX++)
	{
		if(DMX_Channels[l_IDX] > SEQUENCE_DMX_MAX_VALUE)
		{
			DMX_Channels[l_IDX] = SEQUENCE_DMX_MAX_VALUE;
		}
	}
}

void app_Sequence_Main(void)
{
	if((false != rb_EnableSequenceFlag) &&
			(false != rb_SequenceTick))
	{
		rb_SequenceTick = false;
		app_SequenceCommandExec(ACSequence, &ACSequence_State);

		Rainbow_FrameCounter++;
		if(Rainbow_FrameCounter >= RAINBOW_FRAME_DIV)
		{
			Rainbow_FrameCounter = 0u;
			Rainbow_BaseHue = (uint8_t)(Rainbow_BaseHue + RAINBOW_HUE_SPEED);
		}
		Sequence_LED_RainbowUpdate();
		app_Sequence_LimitChannels();
	}
	else
	{
		/* Do Nothing */
	}
}


void app_SequenceCommandExec(T_Sequence* l_Sequence, T_SequenceState* l_State)
{
	bool l_ExecFlag;

	l_ExecFlag = true;
	do
	{
		switch(l_Sequence[l_State->lul_SeqStep].cmd)
		{
		case SEQ_INIT_FRAME:
		{
			l_ExecFlag = true;
			l_State->lul_SeqStep++;
		}break;
		case SEQ_END_FRAME:
		{
			l_ExecFlag = false;
			l_State->lul_SeqStep++;
		}break;
		case SEQ_SELECT_CH:
		{
			l_State->lub_CHIndex = (uint8_t)l_Sequence[l_State->lul_SeqStep].param;
			l_State->lul_SeqStep++;
		}break;
		case SEQ_SET_CH_DATA:
		{
			DMX_Channels[l_State->lub_CHIndex] = (uint8_t)l_Sequence[l_State->lul_SeqStep].param;
			l_State->lul_SeqStep++;
		}break;
		case SEQ_SET_TIME:
		{
			l_State->lul_TimeCounter = l_Sequence[l_State->lul_SeqStep].param;
			l_State->lul_SeqStep++;
		}break;
		case SEQ_TIME_WAIT:
		{
			if(l_State->lul_TimeCounter > 0u)
			{
				l_State->lul_TimeCounter--;
				l_ExecFlag=false;
			}
			else
			{
				l_State->lul_SeqStep++;
			}
		}break;
		case SEQ_JUMP:
		{
			l_State->lul_SeqStep = l_Sequence[l_State->lul_SeqStep].param;
		}break;
		case SEQ_FADEUP:
		{
			if(DMX_Channels[l_State->lub_CHIndex] < PWM_BASECNTR_MAX)
			{
				if((PWM_BASECNTR_MAX - DMX_Channels[l_State->lub_CHIndex]) >= l_Sequence[l_State->lul_SeqStep].param)
				{
					DMX_Channels[l_State->lub_CHIndex] += (uint8_t)l_Sequence[l_State->lul_SeqStep].param;
				}
				else
				{
					DMX_Channels[l_State->lub_CHIndex] = PWM_BASECNTR_MAX;
				}
			}
			l_State->lul_SeqStep++;
		}break;
		case SEQ_FADEDOWN:
		{
			if(DMX_Channels[l_State->lub_CHIndex] > PWM_BASECNTR_MIN)
			{
				if((DMX_Channels[l_State->lub_CHIndex] - PWM_BASECNTR_MIN) >= l_Sequence[l_State->lul_SeqStep].param)
				{
					DMX_Channels[l_State->lub_CHIndex] -= (uint8_t)l_Sequence[l_State->lul_SeqStep].param;
				}
				else
				{
					DMX_Channels[l_State->lub_CHIndex] = PWM_BASECNTR_MIN;
				}
			}
			l_State->lul_SeqStep++;
		}break;
		case SEQ_INIT_LOOP:
		{
			l_State->lul_LoopCounter = l_Sequence[l_State->lul_SeqStep].param;
			l_State->lul_SeqStep++;
			l_State->lul_FirstLoopStep = l_State->lul_SeqStep;
		}break;
		case SEQ_CHECK_LOOP:
		{
			if(l_State->lul_LoopCounter > 0u)
			{
				l_State->lul_LoopCounter--;
				l_State->lul_SeqStep = l_State->lul_FirstLoopStep;
			}
			else
			{
				l_State->lul_SeqStep++;
			}
		}break;
		default:
		{
			l_ExecFlag = false;
			l_State->lul_SeqStep = 0u;
		}break;
		}
	}while(l_ExecFlag);
}