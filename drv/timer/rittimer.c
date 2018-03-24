#include <lpc1768.h>
#include <core_cm3.h>
#include <clock.h>
#include "rittimer.h"

void (*_rit_handler)(void);


static uint32_t rit_clk_selector(){
        switch( (LPC_SC->PCLKSEL1 >> PCLK_RIT) & 3){            
            case PCLK_1:
                return 1000000UL;
            case PCLK_2:
                return 2000000UL;    
            default:
            case PCLK_4:
                return 4000000UL;
            case PCLK_8:
                return 8000000UL;
        }
}

//--------------------------------------------------
//
//--------------------------------------------------
void RIT_SetClockDivider(uint8_t ckdiv){
    LPC_SC->PCLKSEL1 &= ~(3<<PCLK_RIT);
    LPC_SC->PCLKSEL1 |= ((ckdiv&3)<<PCLK_RIT);
}
//--------------------------------------------------
//
//--------------------------------------------------
void RIT_InitHandler(unsigned int ticks, void *handler){
	SC->PCONP |=  RIT_PEN; // power up rit
	RIT->RICOMPVAL = ticks * (CLOCK_GetCCLK() / rit_clk_selector());
	RIT->RICTRL = 0; 		// stop timer
	RIT->RICOUNTER = 0;     // reset timer
	_rit_handler = (void(*)(void))handler; 
	NVIC_EnableIRQ(RIT_IRQn);   // enable irq
	RIT->RICTRL = RITENCLR | RITENBR | RITEN; //Start timer
}
//--------------------------------------------------
//  hw irq call
//--------------------------------------------------
void RIT_IRQHandler(void){
	RIT->RICTRL |= RITINT;    // flag must be clear at the begin of handler		
	_rit_handler();
}

void RIT_Stop(void){
    RIT->RICTRL = 0;
}

void RIT_Start(){
    RIT->RICTRL = 0; 		
	RIT->RICOUNTER = 0;     
    RIT->RICTRL = RITENCLR | RITENBR | RITEN;
}

void RIT_Disable(){
    RIT->RICTRL = 0;
    NVIC_DisableIRQ(RIT_IRQn);
}

