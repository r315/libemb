#include <lpc17xx.h>

#include "rittimer.h"


void toggleLed(void){
	LPC_GPIO1->FIOPIN ^= (1<<29);
}

int main(void){
	LPC_GPIO1->FIODIR |= (1<<29);	
	//RIT_InitHandler(250000,toggleLed);  //250ms
	RIT_Init(1000);
	while(1){		
		toggleLed();
		RIT_DelayMs(250);
	}
}
