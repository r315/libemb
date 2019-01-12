/*************************************************
14-07-2011 alterado def de _osc para pll 

**************************************************/

#ifndef _blueboard_h_
#define _blueboard_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define __BB__

#include <LPC17xx.h>
#include <clock.h>
#include <pwm.h>
#include <i2c.h>
#include <dac.h>
#include <spi.h>
#include <lcd.h>
#include <button.h>
#include <ili9328.h>
#include <display.h>

#define XTAL        (12000000UL)        /* Oscillator frequency               */
#define OSC_CLK     (      XTAL)        /* Main oscillator frequency          */
#define RTC_CLK     (   32000UL)        /* RTC oscillator frequency           */
#define IRC_OSC     ( 4000000UL)        /* Internal RC oscillator frequency   */

#define CLOCK_OUT {LPC_SC->CLKOUTCFG = (1<<8); /* CCLK/2, CLKOU_EN */     \
	              LPC_PINCON->PINSEL3 |= (1<<22);}    /* P1.27 CLKOUT */  \


//PCLKSEL0 Bits
#define PCLK_TIMER0 2
#define PCLK_TIMER1 4
#define PCLK_UART0  6
#define PCLK_UART1  8
#define PCLK_PWM1   12
#define PCLK_I2C0   14
#define PCLK_SPI    16
#define PCLK_SSP1   20
#define PCLK_DAC    22
#define PCLK_ADC    24
#define PCLK_CAN1   26
#define PCLK_CAN2   28
#define PCLK_ACF    30

//PCLKSEL1 Bits
#define PCLK_RIT	26
#define PCLK_TIMER2 12
#define PCLK_TIMER3 14

#define PCLK_1 		1
#define PCLK_2 		2
#define PCLK_4 		0
#define PCLK_8 		3

#define CCLK_DIV1   1
#define CCLK_DIV2   2
#define CCLK_DIV4   0
#define CCLK_DIV8   3 

#define CCLK_100	100
#define CCLK_80		80
#define CCLK_72		72
#define CCLK_48		48

#define SET_PCLK_TIMER3(x) {LPC_SC->PCLKSEL1 = (LPC_SC->PCLKSEL1 & ~(3 << PCLK_TIMER3)) | (x<<PCLK_TIMER3);}
#define SET_PCLK_PWM1(x)   {LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & ~(3 << PCLK_PWM1)) | (x << PCLK_PWM1);}
#define SET_PCLK_DAC(x)    {LPC_SC->PCLKSEL1 = (LPC_SC->PCLKSEL1 & ~(3 << PCLK_DAC)) | (x << PCLK_DAC);}

#define __USE_SYSTICK


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
// Joystick pins P1.14, p1.15, p1.16, p1.17, p1.28
// see button.h
//-----------------------------------------------------
#define BUTTON_UP    (1<<15)
#define BUTTON_DOWN  (1<<17) //0x1001C000
#define BUTTON_LEFT  (1<<16) //0x1002C000
#define BUTTON_RIGHT (1<<28) //0x0003C000
#define BUTTON_A     (1<<14) //0x10038000
#define BUTTON_CENTER BUTTON_A

#define BUTTON_Capture() (~(LPC_GPIO1->FIOPIN) & BUTTON_MASK)
#define BUTTON_Cfg() (LPC_GPIO1->FIODIR &= ~(BUTTON_MASK))

#define BUTTON_MASK (BUTTON_UP | BUTTON_DOWN | BUTTON_LEFT | BUTTON_RIGHT | BUTTON_A )
//-----------------------------------------------------
//
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
#define	MMC_SEL       	!(LPC_GPIO0->FIOPIN & MMC_CS)		/* MMC CS status (true:selected) */

#define  LCD_CS	 		(1<<10) //P1.10
#define  LCD_RS	 		(1<<9)	 //P1.9
#define  LCD_WR	 		(1<<8)	 //P1.8
#define  LCD_RD	 		(1<<4)	 //P1.4
#define  LCD_LED 		(1<<1)  //P1.1
#define  LCD_RST 		(1<<0)	 //P1.0	

#define  LCD_CTRLPORT    	GPIO1
#define  LCD_DATAPORT    	GPIO0->FIOPIN0
#define  LCD_DATAPORTDIR 	GPIO0->FIODIR0

#define  LCDCS0      LCD_CTRLPORT->FIOCLR = LCD_CS;
#define  LCDCS1      LCD_CTRLPORT->FIOSET = LCD_CS; 
#define  LCDRS0      LCD_CTRLPORT->FIOCLR = LCD_RS;
#define  LCDRS1      LCD_CTRLPORT->FIOSET = LCD_RS;
#define  LCDWR0      LCD_CTRLPORT->FIOCLR = LCD_WR; //asm("nop");
#define  LCDWR1      LCD_CTRLPORT->FIOSET = LCD_WR;  
#define  LCDRD0      LCD_CTRLPORT->FIOCLR = LCD_RD;
#define  LCDRD1      LCD_CTRLPORT->FIOSET = LCD_RD; 
#define  LCDRST0     LCD_CTRLPORT->FIOCLR = LCD_RST;
#define  LCDRST1     LCD_CTRLPORT->FIOSET = LCD_RST; 
#define  LCD_BKL0    LCD_CTRLPORT->FIOCLR = LCD_LED;
#define  LCD_BKL1    LCD_CTRLPORT->FIOSET = LCD_LED;

#define LCD_IO_INIT                                                   \
	LCD_CTRLPORT->FIODIR |= LCD_CS|LCD_RS|LCD_WR|LCD_RD|LCD_LED|LCD_RST;  \
	LCD_DATAPORTDIR |= 0xFF;


//-----------------------------------------------------
void BB_Init(void);

#define RAM_FUNC __attribute__ ((section(".ram_code")))

#ifdef __cplusplus
}
#endif

#endif






