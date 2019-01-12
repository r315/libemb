#include <board.h>
#include <timer.h>

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

void TIM_Stop(LPC_TIM_TypeDef *tim){
    tim->TCR = TIMER_RESET;
}

void TIM_Restart(LPC_TIM_TypeDef *tim){
    tim->TCR = TIMER_RESET;
    tim->TCR = TIMER_ENABLE;
}

//-----------------------------------------------
// Capture
//-----------------------------------------------

static CallBack tim3matchcallback;
static CallBack tim3capcallback;
uint32_t capval;

void TIMER_CAP_Init(LPC_TIM_TypeDef *tim, char ch, char edge, CallBack cb){

     if(cb == NULL){
        return ;       
    }

    ch &= 1;   

    if (tim == LPC_TIM0){
    }else if (tim == LPC_TIM1){
    }else if (tim == LPC_TIM2){
    }else if (tim == LPC_TIM3){
        LPC_SC->PCONP |= PCONP_PCTIM3;
        tim->TCR = TIMER_RESET;                              // Stop Timer and reset
        SET_PCLK_TIMER3(CCLK_DIV1); 

        if(ch == 0){
            PINSEL_P0_23(P0_23_CAP3_0);
        }
        else{
            PINSEL_P0_24(P0_24_CAP3_1);
        }
        NVIC_EnableIRQ(TIMER3_IRQn);
    }else{
        return;
    }    

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
    
    tim3capcallback = cb;
    tim->MCR = 0;                        // Disable match
    tim->PR = (SystemCoreClock / 1000000UL) - 1;  // Set TC Clock to 1Mhz
    tim->CCR |= (4 << (ch * 3)); // Enable Irq    
    tim->TCR = TIMER_ENABLE;
}



//--------------------------------------------------
//  hw irq call
//--------------------------------------------------
void TIMER3_IRQHandler(void){

    if(LPC_TIM3->IR & (3 << 4)){
        tim3capcallback((void*)&LPC_TIM3->CR0);
        LPC_TIM3->IR = (3 << 4);
    }

    if(LPC_TIM3->IR & ( 0X0f << 0)){
        tim3matchcallback(NULL);
        LPC_TIM3->IR = ( 0x0f << 0);
    }
    
}
  
// -------------------------------------------------
// Compare
// -------------------------------------------------
void TIMER_Match_Init(LPC_TIM_TypeDef *tim, char ch, uint32_t count, CallBack cb){

    if(cb == NULL){
        return ;       
    }    

    ch &= 3;
    
    if (tim == LPC_TIM0){
    }else if (tim == LPC_TIM1){
    }else if (tim == LPC_TIM2){
    }else if (tim == LPC_TIM3){
        LPC_SC->PCONP |= PCONP_PCTIM3;
        tim->TCR = TIMER_RESET;
        SET_PCLK_TIMER3(CCLK_DIV1);
        *((uint32_t*)(&tim->MR0 + (ch << 2))) = count - 1;
        NVIC_EnableIRQ(TIMER3_IRQn);    
            
    }else{
        return;
    }    
    
    tim3matchcallback = cb;
    tim->CCR = 0;                                                // Disable capture    
    tim->PR = (SystemCoreClock / 1000000UL) - 1;                 // Set TC Clock to 1Mhz
    tim->MCR |= (MCR_MRxI << (ch * 3)) | (MCR_MRxR << (ch * 3)); // Interrupt on match and reset timer     
    tim->TCR = TIMER_ENABLE;
}    
  
 

