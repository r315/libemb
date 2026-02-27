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

void STIMER_Config(stimer_t *timer);    // Adds timer to internal timer list
void STIMER_Cancel(stimer_t *timer);    // Remove timer from timer list
void STIMER_Start(stimer_t *timer);     // Starts timer
void STIMER_Stop(stimer_t *timer);      // Stops timer
void STIMER_SetInterval(stimer_t *timer, uint32_t interval);  // Configures a new interval for timer
uint32_t STIMER_IsActive(stimer_t *timer);
void STIMER_Handler(void);              // handler for interrupt, called periodically and increments counts by 1
void STIMER_Tick(uint32_t tick);        // function call for application loop for counts increment.

#ifdef __cplusplus
}
#endif

#endif