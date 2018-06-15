/*************************************************
14-07-2011 alterado def de _osc para pll 

**************************************************/

#ifndef _blueboard_h_
#define _blueboard_h_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32 
    #include <LPC17xx.h>
#endif

#include <stdint.h>
//#include "util.h"   // conflito com type.h

/* valores dados por NXP lpc17xx.pll.calculator.xls */	 

#ifdef PLL48
/* Fcclk = 48Mhz, Fcco = 288Mhz */
	#define PLL0_MValue 12
	#define PLL0_NValue 1
	#define CCLKDivValue 6	
	#define _PLL
#endif

#ifdef PLL72
/* Fcclk = 72Mhz, Fcco = 288Mhz */
	#define PLL0_MValue 24
	#define PLL0_NValue 2
	#define CCLKDivValue 4	
	#define _PLL
#endif

#ifdef PLL100
/* Fcclk = 100Mhz, Fcco = 300Mhz */
	#define PLL0_MValue 25
	#define PLL0_NValue 2
	#define CCLKDivValue 3
	#define _PLL
#endif

#ifdef PLL80
/* Fcclk = 80Mhz, Fcco = 400Mhz */
  #define PLL0_MValue 50
	#define PLL0_NValue 3
	#define CCLKDivValue 5
	#define _PLL
#endif


//-----------------------------------------------------
// Joy pins P1.14, p1.15, p1.16, p1.17, p1.28
//-----------------------------------------------------
// see button.h
//-----------------------------------------------------
#ifndef LEDS
	#define LED1 (1<<29) //P1.29 (D8)
	#define LED1_ON  LPC_GPIO1->FIOSET = LED1
	#define LED1_OFF LPC_GPIO1->FIOCLR = LED1
	
	#define LED2 (1<<18) //P1.18 (D7)
	#define LED2_OFF LPC_GPIO1->FIOSET = LED2
	#define LED2_ON  LPC_GPIO1->FIOCLR = LED2
	
	#define LED3 (1<<9)  //P2.9 (D1)
	#define LED3_OFF LPC_GPIO2->FIOSET = LED3
	#define LED3_ON  LPC_GPIO2->FIOCLR = LED3
#endif

#define ACCEL_CS (1<<6)
#define SELECT_ACCEL()    LPC_GPIO0->FIOCLR = ACCEL_CS
#define DESELECT_ACCEL()  LPC_GPIO0->FIOSET = ACCEL_CS

#define MMC_CS (1<<16)
#define SELECT_CARD()	LPC_GPIO0->FIOCLR = MMC_CS		/* MMC CS = L */
#define	DESELECT_CARD()	LPC_GPIO0->FIOSET = MMC_CS		/* MMC CS = H */
#define	MMC_SEL       !(LPC_GPIO0->FIOPIN & MMC_CS)		/* MMC CS status (true:selected) */


//-----------------------------------------------------
void BB_Init(void);

#define RAM_FUNC __attribute__ ((section(".ram_code")))

#ifdef __cplusplus
}
#endif

#endif






