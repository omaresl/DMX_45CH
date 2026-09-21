/*
 * app_SafeState.h
 *
 * Fail-safe output shutdown, called from fault handlers and Error_Handler
 * before NVIC_SystemReset. Direct register writes (no HAL calls) so it
 * works even with a corrupted stack.
 */

#ifndef APP_SAFESTATE_H_
#define APP_SAFESTATE_H_

extern void app_SafeState_OutputsOff(void);

#endif /* APP_SAFESTATE_H_ */
