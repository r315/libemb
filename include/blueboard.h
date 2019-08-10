/*************************************************
14-07-2011 alterado def de _osc para pll 

**************************************************/

#ifndef _blueboard_h_
#define _blueboard_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef __BB__
#define __BB__
#endif

#include <LPC17xx.h>
#include <clock_lpc17xx.h>
#include <pwm.h>
#include <i2c.h>
#include <dac.h>
#include <spi.h>
#include <lcd.h>
#include <button.h>
#include <ili9328.h>
#include <display.h>
#include <uart_lpc17xx.h>
#include <timer.h>


#define PLL48   0
#define PLL72   1
#define PLL80   2
#define PLL100  3

/* Watchdog */
#define WDMOD_WDEN    (1 << 0)
#define WDMOD_WDRESET (1 << 1)
#define WDCLKSEL_WDSEL_PCLK (1 << 0)

#define WDT_RELOAD          \
{                           \
    LPC_WDT->WDFEED = 0xAA; \
    LPC_WDT->WDFEED = 0x55; \
}

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

#define LED1 (1<<29) //P1.29 (D8 BLUE)
#define LED1_ON  LPC_GPIO1->FIOSET = LED1
#define LED1_OFF LPC_GPIO1->FIOCLR = LED1
#define LED1_TOGGLE LPC_GPIO1->FIOPIN ^= LED1

#define LED2 (1<<18) //P1.18 (D7 RED)
#define LED2_OFF LPC_GPIO1->FIOSET = LED2
#define LED2_ON  LPC_GPIO1->FIOCLR = LED2

#define LED3 (1<<9)  //P2.9 (D1 RED)
#define LED3_OFF LPC_GPIO2->FIOSET = LED3
#define LED3_ON  LPC_GPIO2->FIOCLR = LED3

#define LEDS_CFG


#define ACCEL_CS_PIN      (1<<6)
#define SELECT_ACCEL      LPC_GPIO0->FIOCLR = ACCEL_CS_PIM
#define DESELECT_ACCEL    LPC_GPIO0->FIOSET = ACCEL_CS_PIN

#define MMC_CS_PIN      (1<<16)
#define SELECT_CARD     LPC_GPIO0->FIOCLR = MMC_CS_PIN		/* MMC CS = L */
#define	DESELECT_CARD	LPC_GPIO0->FIOSET = MMC_CS_PIN		/* MMC CS = H */
#define	MMC_SEL       	!(LPC_GPIO0->FIOPIN & MMC_CS_PIN)		/* MMC CS status (true:selected) */

#define  LCD_CS	 		(1<<10) //P1.10
#define  LCD_RS	 		(1<<9)	 //P1.9
#define  LCD_WR	 		(1<<8)	 //P1.8
#define  LCD_RD	 		(1<<4)	 //P1.4
#define  LCD_LED 		(1<<1)  //P1.1
#define  LCD_RST 		(1<<0)	 //P1.0	

#define  LCD_CTRLPORT    	LPC_GPIO1
#define  LCD_DATAPORT    	LPC_GPIO0->FIOPIN0
#define  LCD_DATAPORTDIR 	LPC_GPIO0->FIODIR0

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


#define BB_Init()                                                        \
{                                                                        \
	LPC_GPIO0->FIODIR |= 0xFF;                                           \
	LPC_GPIO1->FIODIR |= LED1|LED2;                                      \
	LPC_GPIO2->FIODIR |= LED3;                                           \
    /* accelerometer cs pin */                                           \
    LPC_GPIO0->FIODIR   |= ACCEL_CS_PIN;  /* en cs pin */                \
    LPC_PINCON->PINSEL0 &= ~(3<<12);  /* P0.6 (used as GPIO) */          \
    /* mmc cs pin */                                                     \
	LPC_GPIO0->FIODIR   |=  MMC_CS_PIN;   /* SET MMC_CS pin  as output */\
	LPC_PINCON->PINSEL1 &= ~(3<<0);   /* P0.16 (used as GPIO)   */       \
	LED1_OFF;                                                            \
	LED2_OFF;                                                            \
	LED3_OFF;                                                            \
	DESELECT_ACCEL;                                                      \
	DESELECT_CARD;                                                       \
}

//-----------------------------------------------------
void BB_ConfigPLL(uint8_t fmhz);
void SW_Reset(void);
void DelayMs(uint32_t dl);
uint32_t GetTicks(void);


#define RAM_FUNC __attribute__ ((section(".ram_code")))

#ifdef __cplusplus
}
#endif

#endif






