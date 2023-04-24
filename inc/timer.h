#ifndef __timer_h__
#define __timer_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint32_t TIMER_SetInterval(void (*func)(void), uint32_t interval);
uint32_t TIMER_SetTimeout(void (*func)(void), uint32_t interval);
void TIMER_ClearInterval(uint32_t tid);

#ifdef __cplusplus
}
#endif

#endif
