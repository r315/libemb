#include <lpc1768.h>
#include "timer.h"

void TIMER_Start(Timer *tm, uint32_t interval){
    tm->initial = TIMER_GetTicks();
    tm->expires = TIMER_GetTicks() + interval;
    tm->interval = interval;
}

uint8_t TIMER_IsExpired(Timer *tm){
return TIMER_GetTicks() >= tm->expires;
}

void TIMER_Reset(Timer *tm){
    tm->expires += tm->interval;
}

void TIMER_Restart(Timer *tm){
    TIMER_Start(tm, tm->interval);
}


void TIMER_SetClockDivider(uint8_t tim, uint8_t div){
    div &= 3;
    switch(tim){
        case 0:
            LPC_SC->PCLKSEL0 &= ~(3<<PCLK_TIMER0);
            LPC_SC->PCLKSEL0 |= ~(div<<PCLK_TIMER0);
            break;
        case 1:
            LPC_SC->PCLKSEL0 &= ~(3<<PCLK_TIMER1);
            LPC_SC->PCLKSEL0 |= ~(div<<PCLK_TIMER1);
            break;
        case 2:
            LPC_SC->PCLKSEL1 &= ~(3<<PCLK_TIMER2);
            LPC_SC->PCLKSEL1 |= ~(div<<PCLK_TIMER2);
            break;
        case 3:
            LPC_SC->PCLKSEL1 &= ~(3<<PCLK_TIMER3);
            LPC_SC->PCLKSEL1 |= ~(div<<PCLK_TIMER3);
            break;
        default: 
            break;
    }
}


