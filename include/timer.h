#ifndef __timer_h__
#define __timer_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct stimer{
    uint32_t interval;
    uint32_t ticks;
    void (*func)(void);
    struct stimer *next;
    void *regs;
};

typedef struct stimer timer_t;

/**
 *@brief Start a timer
 **/
void TIMER_SetInterval(timer_t *tm, uint32_t interval);

void TIMER_SetTimeout(timer_t *tm, uint32_t interval);


#ifdef __cplusplus
}
#endif

#endif
