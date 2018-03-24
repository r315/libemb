

#ifndef __EMU__

#if defined(__USE_CMSIS)
#include <LPC17xx.h>
#else
#include <lpc1768.h>
#endif

#include <clock.h>
#include <pwm.h>

void PWM_Init(uint32_t tcclk){
    LPC_SC->PCONP |= PCPWM1;                        // Enable PWM Module
    
	LPC_SC->PCLKSEL0 &= ~(PCLK_8<<PCLK_PWM1);        
	LPC_SC->PCLKSEL0 |= (PCLK_2 << PCLK_PWM1);       // Divide System clock by 2

	LPC_PINCON->PINSEL4 &= ~(0x0);
    LPC_PINCON->PINSEL4 |= 0x55;                    // Select PWM function for P2.3:0

	LPC_PWM1->PR = ((SystemCoreClock / 1000000UL) >> 1) - 1;     // Set TC Clock to 1Mhz
    
    LPC_PWM1->TCR = (1<<TCR_TCEN) | (1<<TCR_PWMEN);

	LPC_PWM1->MCR = (1<<PWMMR0R);                   // reset TC on MR0 match

	LPC_PWM1->MR0 = tcclk;                          // set all outputs on match
}

void PWM_Set(uint8_t channel, uint8_t duty){
uint32_t match;
    
    if(duty > 100)
        duty = 100;
        
    match = LPC_PWM1->MR0;
    match *= duty;
    match /= 100;
    
	switch(channel){
		case PWM_1: LPC_PWM1->MR1 = match; break;
		case PWM_2: LPC_PWM1->MR2 = match; break;
		case PWM_3: LPC_PWM1->MR3 = match; break;
		case PWM_4: LPC_PWM1->MR4 = match; break;
		default:
			break;
	}
	LPC_PWM1->LER = (1<<channel);
}


void PWM_Enable(uint8_t ch){
    if(ch > PWM_MAX_CH)return;
    ch -= 1;
    LPC_PWM1->PCR |= ((1<<ch)<<PWMENA1);                // Enable pwm, single edge
}

void PWM_Disable(uint8_t ch){    
    if(ch > PWM_MAX_CH)return;
    ch -= 1;
    LPC_PWM1->PCR &= ~((1<<ch)<<PWMENA1);
}


#else
void PWM_Init(uint32_t tcclk){}
void PWM_Set(uint8_t channel,uint32_t duty){}
void PWM_Enable(uint8_t ch){}
void PWM_Disable(uint8_t ch){}
#endif
