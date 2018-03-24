#include <clock.h>
#include "adc.h"
#include "lpc1768.h"


void ADC_SetClockDivider(uint8_t ckdiv){
    LPC_SC->PCLKSEL0 &= ~(3<<PCLK_ADC);
    LPC_SC->PCLKSEL0 |= ((ckdiv&3)<<PCLK_ADC);
}

void ADC_Init(uint32_t clk){
uint8_t div;
   
    LPC_SC->PCONP |= (1 << PCADC);      /* Power up ADC Module */
    
    if(clk > ADC_MAX_CLK)
      clk = ADC_MAX_CLK;
      
    if(clk < ADC_MIN_CLK)
      clk = ADC_MIN_CLK;
      
    if(clk >= SystemCoreClock)
      clk = SystemCoreClock >> 1;
    
    ADC_SetClockDivider(PCLK_1);

    div = (SystemCoreClock/clk) > ADC_MAX_DIV ? ADC_MAX_DIV : (SystemCoreClock/clk) - 1;
    
    LPC_ADC->ADCR = ((1<<SBIT_PDN) | (div << SBIT_CLKDIV) ); /* Power ON ADC module, set ADC CLK */
    
    //LPC_ADC->ADCR = ((1<<SBIT_PDN) | (div << SBIT_CLKDIV) | (START_NOW << SBIT_START) | (1<<ADC_0));                     
    //ADC_GetValue();
}

void ADC_ConfigureChannel(uint8_t ch){
uint8_t bitpos;
    ch &= (ADC_CHANNELS - 1);
    
    if(ch < 4){                          // configure channels 3:0
        bitpos = (14 + (2 * ch));       // calculate bit position on pinsel1
        LPC_PINCON->PINSEL1 &= ~(0x03 << bitpos);
        LPC_PINCON->PINSEL1 |=  (0x01 << bitpos);  // Function1
    }else{ 
        if(ch > 3 && ch < 6){            // configure channels 5:4
            bitpos = (28 + (2 * (ch & 1)));
            LPC_PINCON->PINSEL3 |= (0x03 << bitpos); //function3
        }
        else{                            // configure channels 7:6
            bitpos = (6 - (2 * (ch & 1)));
            LPC_PINCON->PINSEL0 &= ~(0x03 << bitpos);
            LPC_PINCON->PINSEL0 |=  (0x02 << bitpos); //function2
        }
    }
}

void ADC_SelectChannel(uint8_t ch){
    LPC_ADC->ADCR &= ~(255);
    LPC_ADC->ADCR |= (1 << (ch&7));
}

uint16_t ADC_GetValue(void){
    while(!(LPC_ADC->ADGDR & (1 << SBIT_DONE)));
    return (LPC_ADC->ADGDR >> 4) & 0xFFF; 
}

void ADC_Start(void){    
    LPC_ADC->ADCR |= (START_NOW << SBIT_START);
}

uint16_t ADC_ConvertSingle(uint8_t ch){
    ADC_SelectChannel(ch);
    ADC_Start();
return ADC_GetValue();    
}

