#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef struct simpletimer_s {
    uint32_t interval;          // Timeout in STIMER_Handler call unit, generally 1ms
    uint32_t countdown;         // Internal private counter
    uint32_t (*callback)(struct simpletimer_s *timer); // callback function
    struct simpletimer_s *next; // Next timer in list
}simpletimer_t;

void STIMER_Config(simpletimer_t *timer, uint32_t interval, uint32_t (*callback)(simpletimer_t *timer));
void STIMER_Remove(simpletimer_t *timer);
void STIMER_Start(simpletimer_t *timer);
void STIMER_Stop(simpletimer_t *timer);
void STIMER_Reset(simpletimer_t *timer);
void STIMER_SetInterval(simpletimer_t *timer, uint32_t interval);
void STIMER_Handler(void);

//void STIMER_Pause(simpletimer_t *timer);
//void STIMER_Resume(simpletimer_t *timer);
//void STIMER_IsActive(simpletimer_t *timer);
//uint32_t STIMER_GetInterval(simpletimer_t *timer);
//uint32_t STIMER_Get(simpletimer_t *timer);
#ifdef __cplusplus
}
#endif

#endif