// File Name: blueboard.c
// Date:  	  6-10-2010
/* 
07-05-2013 void getSysFreq(void) -> int getSysFreq(void)
			removida a chamada lcdInit() no metodo blueboardInit()
TODO: inicializar clock usb
*/

#include "blueboard.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------
//	
//---------------------------------------------------
void BB_Init(void)
{
	LPC_GPIO0->FIODIR |= 0xFF;
	LPC_GPIO1->FIODIR |= LED1|LED2;
	LPC_GPIO2->FIODIR |= LED3;

    // accelerometer cs pin
    LPC_GPIO0->FIODIR   |= ACCEL_CS_PIN;  // en cs pin   	
    LPC_PINCON->PINSEL0 &= ~(3<<12);  // P0.6 (used as GPIO)    

    // mmc cs pin
	LPC_GPIO0->FIODIR   |=  MMC_CS_PIN;   /* SET MMC_CS pin  as output */	
	LPC_PINCON->PINSEL1 &= ~(3<<0);  /* P0.16 (used as GPIO)   */ 
	
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	
	DESELECT_ACCEL;	
	DESELECT_CARD;
}

void BB_SW_Reset(void){
    LPC_WDT->WDCLKSEL = WDCLKSEL_WDSEL_PCLK;
    LPC_WDT->WDMOD |= WDMOD_WDEN | WDMOD_WDRESET;    
    WDT_RELOAD;
}

//---------------------------------------------------
//	
//---------------------------------------------------
void BB_ConfigClockOut(uint8_t en){
	if(en){
		LPC_SC->CLKOUTCFG = (1<<4)|/* CCLK/2 */ (1<<8);/* CLKOU_EN*/
		LPC_PINCON->PINSEL3 |= (1<<22);// P1.27 CLKOUT 
	}else{
		LPC_SC->CLKOUTCFG = (1<<4)|/* CCLK/2 */ (1<<8);/* CLKOU_EN*/
		LPC_PINCON->PINSEL3 &= ~(1<<22);
	}
}


//--------------------------------------------------
//
//--------------------------------------------------
void BB_RitTimeBase_Init(void){


}

//--------------------------------------------------
//
//--------------------------------------------------
void BB_RitDelay(uint32_t ms){
	
}
//--------------------------------------------------
//
//--------------------------------------------------
uint32_t BB_RitTicks(void){
	return 0;
}

#ifdef __cplusplus
}
#endif

