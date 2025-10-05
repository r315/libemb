#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef struct stimer {
    uint32_t interval;          // Timeout in STIMER_Handler call unit, generally 1ms
    int32_t count;              // Internal private counter
    uint32_t (*callback)(struct stimer *timer);
    void *data;                 // User data
    struct stimer *next;        // Next timer in list
}stimer_t;

void STIMER_Config(stimer_t *timer, uint32_t interval, uint32_t (*callback)(stimer_t *timer));
void STIMER_Cancel(stimer_t *timer);
void STIMER_Start(stimer_t *timer);
void STIMER_Stop(stimer_t *timer);
void STIMER_Reset(stimer_t *timer);
void STIMER_SetInterval(stimer_t *timer, uint32_t interval);
uint32_t STIMER_IsActive(stimer_t *timer);
void STIMER_Handler(void);
void STIMER_Tick(uint32_t tick);

#ifdef __cplusplus
}
#endif

#endif