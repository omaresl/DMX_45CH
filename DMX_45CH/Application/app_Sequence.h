/*
 * app_Sequence.h
 *
 *  Created on: Jan 10, 2024
 *      Author: Omare
 */

#ifndef APP_SEQUENCE_H_
#define APP_SEQUENCE_H_
#include <stdbool.h>

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
extern void app_Sequence_Main(void);
extern void app_SequenceCommandExec(T_Sequence* l_Sequence);

#endif /* APP_SEQUENCE_H_ */
