#include <common.h>
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


//-----------------------------------------------
//TIMER 3 
//-----------------------------------------------

static CallBack tim3callback;
uint32_t capval;

void TIMER_CAP_Init(LPC_TIM_TypeDef *tim, char ch, char edge, CallBack cb){

    ch &= 1;

    switch((uint32_t)tim){
        case (uint32_t)LPC_TIM0:
        case (uint32_t)LPC_TIM1:
        case (uint32_t)LPC_TIM2:
        case (uint32_t)LPC_TIM3:
            LPC_SC->PCONP |= PCONP_PCTIM3;
            SET_PCLK_TIMER3(CCLK_DIV4);           
            if(ch == 0){
                PINSEL_P0_23(PINSEL_CAP3_0);
            }
            else{
                PINSEL_P0_24(PINSEL_CAP3_1);
            }
            NVIC_EnableIRQ(TIMER3_IRQn);    
            break;
    }
    
    tim->TCR = TIMER_RESET;                              // Stop Timer and reset
    tim->PR = ((SystemCoreClock / 1000000UL) >> 1) - 1;  // Set TC Clock to 1Mhz

    switch(edge){
        case CAP_RE:
            tim->CCR = (tim->CCR & ~(1 << ch * 3)) | (1 << ch * 3); // set CAPxRE bit
            break;
        case CAP_FE:
            tim->CCR = (tim->CCR & ~(2 << ch * 3)) | (2 << ch * 3); // set CAPxFE bit
            break;
        case CAP_RFE:
            tim->CCR |= (3 << ch * 3); // set CAPxRE and CAPxFE
            break;
    }

	
    if(cb != NULL){
        tim3callback = cb;
        tim->CCR = (tim->CCR & ~(4 << ch *3)) | (4 << ch * 3); // Enable Irq
    }

    tim->TCR = TIMER_ENABLE;
}

void TIMER_CAP_Restart(LPC_TIM_TypeDef *tim){
    tim->TCR = TIMER_RESET;
    tim->TCR = TIMER_ENABLE;
}

void TIMER_CAP_Stop(LPC_TIM_TypeDef *tim){
    tim->TCR = TIMER_RESET;
}
//--------------------------------------------------
//  hw irq call
//--------------------------------------------------
void TIMER3_IRQHandler(void){
    tim3callback((void*)&LPC_TIM3->CR0);
}

