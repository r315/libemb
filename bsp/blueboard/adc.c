// Header:		ADC stuff
// File Name: ADC.c
// Author:
// Date:	28-10-2010

//#include "board.h"
#include "lpc17xx.h"
#include "adc.h"

//-------------------------------------------------------
//
//-------------------------------------------------------
void ADC_Init(void){
    uint32_t u32PCLK;

    LPC_SC->PCONP	|= (1<<12);      	// Enable power to ADC block

    LPC_PINCON->PINSEL1 &= ~(0xFF<<14);	// P0.26:23 as GPIO       
    LPC_PINCON->PINSEL1 |=  (0x55<<14); // then P0.26:23 as AD0.3:0

    /* After configuring PINSEL, ADC needs a delay before starting sampling (Otherwise, higher values will be read)  */
    DelayMs(5);

    // Find peripheral clock divider  	
    switch ( (LPC_SC->PCLKSEL0 >> 24) & 0x03 ){
        case 0x00:
        default:
            u32PCLK = SystemCoreClock/4;
            break;
        case 0x01:
            u32PCLK = SystemCoreClock;
            break; 
        case 0x02:
            u32PCLK = SystemCoreClock/2;
            break; 
        case 0x03:
            u32PCLK = SystemCoreClock/8;
            break;
    }

    /* Configure ADC */
    LPC_ADC->ADCR =	(((u32PCLK / 13000000) - 1 ) << 8 ) |  /* CLKDIV = Fpclk / Max ADC_Clk - 1 */ 
                    ( 0 << 16 ) | 		/* BURST = 0, no BURST, software controlled */
                    ( 0 << 17 ) |  		/* CLKS = 0, 11 clocks/10 bits */
                    ( 1 << 21 ) |		/* PDN = 1, normal operation */
                    ( 0 << 24 ) |  		/* START = 0 A/D conversion stops */					
                    ( 0 << 27 );
}
//-------------------------------------------------------
//
//-------------------------------------------------------
uint16_t ADC_Convert(uint8_t ch){

    LPC_ADC->ADCR &= 0xFFFFFF00;	// clr channels		
    LPC_ADC->ADCR |= (1UL<<ch);     // select ch
    LPC_ADC->ADCR |= (1UL << 24);   // START
    
    while ((LPC_ADC->ADGDR & 0x80000000UL) == 0);   // Wait for DONE flag

    return (LPC_ADC->ADGDR >> 4) & 0x00000FFFUL;    // return result
}
