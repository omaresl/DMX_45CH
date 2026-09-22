/*
 * app_Sequence.h
 *
 *  Created on: Jan 10, 2024
 *      Author: Omare
 */

#ifndef APP_SEQUENCE_H_
#define APP_SEQUENCE_H_
#include <stdbool.h>
#include <stdint.h>

/* Default when EEPROM slot holds 0xFFFFFFFF or garbage (>200) */
#define SEQUENCE_DMX_MAX_VALUE		(100u)
#define SEQUENCE_DMX_LIMIT_MAX		(200u)

typedef struct
{
	uint8_t  lub_CHIndex;
	uint32_t lul_SeqStep;
	uint32_t lul_TimeCounter;
	uint32_t lul_LoopCounter;
	uint32_t lul_FirstLoopStep;
}T_SequenceState;

typedef enum
{
	SEQ_INIT_FRAME,
	SEQ_SELECT_CH,
	SEQ_SET_CH_DATA,
	SEQ_SET_TIME,
	SEQ_TIME_WAIT,
	SEQ_END_FRAME,
	SEQ_JUMP,
	SEQ_FADEUP,
	SEQ_FADEDOWN,
	SEQ_INIT_LOOP,
	SEQ_CHECK_LOOP
}T_SequenceCmd;

typedef struct
{
	T_SequenceCmd cmd; /* Command */
	uint32_t 	param;
}T_Sequence;

extern bool rb_EnableSequenceFlag;
extern bool rb_SequenceTick;
/* Runtime power limit (EEPROM slot 1), applied to cascade, rainbow, clamp */
extern uint8_t Sequence_DMX_MaxValue;
extern void app_Sequence_Init(void);
extern void app_Sequence_Main(void);
extern void app_SequenceCommandExec(T_Sequence* l_Sequence, T_SequenceState* l_State);

#endif /* APP_SEQUENCE_H_ */
