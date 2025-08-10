#include <stddef.h>
#include "lpc17xx_hal.h"
#include "timer.h"

static timer_t *rit_timers_head;

#define RIT_TICK_RATE       1000UL

//--------------------------------------------------
//
//--------------------------------------------------
void RIT_Init(void){
	PCONP_RIT_ENABLE;
	LPC_RIT->RICOMPVAL = (CLOCK_GetPCLK(PCLK_RIT) / RIT_TICK_RATE) - 1;
	LPC_RIT->RICTRL = 0; 		    // stop timer
	LPC_RIT->RICOUNTER = 0;         // reset timer
	LPC_RIT->RICTRL =   RIT_RICTRL_RITENCLR | 
                    RIT_RICTRL_RITENBR | 
                    RIT_RICTRL_RITEN; //Start timer
    rit_timers_head = NULL;
    ticks = 0;
	NVIC_EnableIRQ(RIT_IRQn);   // enable irq
}

void RIT_DeInit(void){
    LPC_RIT->RICTRL = 0;
    NVIC_DisableIRQ(RIT_IRQn);
}

void RIT_StartInterval(timer_t *tm){
    timer_t *tim;

    tm->next = NULL;
    tm->ticks = tm->interval;

    if(rit_timers_head == NULL){
        rit_timers_head = tm;
        return;
    }

    tim = rit_timers_head;
    
    while(tim->next != NULL){
        tim = tim->next;
    }

    tim->next = tm;
}

void RIT_StartTimeout(timer_t *tm){
    RIT_StartInterval(tm);
    tm->interval = 0;
}

void RIT_Stop(timer_t *tim){
    timer_t *tm;

    if(rit_timers_head == tim){
        rit_timers_head = tim->next;
        tim->next = NULL;
        return;
    }

    tm = rit_timers_head;

    while(tm->next != NULL){
        if(tm->next == tim){
            tm->next = tim->next;
            tim->next = NULL;
            return;
        }
    }
}

//--------------------------------------------------
//  hw irq call
//--------------------------------------------------
void RIT_IRQHandler(void){
    timer_t *tm, *prev;
	LPC_RIT->RICTRL |= RIT_RICTRL_RITINT;    // flag must be clear at the begin of handler

    tm = rit_timers_head;

	while(tm != NULL){
        if(tm->ticks == 0){
            if(tm->func)
                tm->func();
            if(tm->interval > 0){
                tm->ticks = tm->interval;
            }else{
                prev->next = tm->next;
                tm->next = NULL;
            }
        }else{
            tm->ticks--;
        }

        prev = tm;
        tm = tm->next;
    }
}

