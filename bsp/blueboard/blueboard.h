/*************************************************
14-07-2011 alterado def de _osc para pll 

**************************************************/

#ifndef _blueboard_h_
#define _blueboard_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#ifndef __BB__
#define __BB__
#endif

#include <LPC17xx.h>
#include <clock_lpc17xx.h>
#include <system_LPC17xx.h>
//#include <pwm.h>
//#include <i2c.h>
//#include <dac.h>
//#include <spi.h>
//#include <lcd.h>
#include <button.h>
#include <ili9328.h>
#include <display.h>
#include <uart_lpc17xx.h>
//#include <timer.h>



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
#define BUTTON_DOWN  (1<<17)
#define BUTTON_LEFT  (1<<16)
#define BUTTON_RIGHT (1<<28)
#define BUTTON_A     (1<<14)
#define BUTTON_CENTER BUTTON_A

#define BUTTON_HW_READ (~(LPC_GPIO1->FIOPIN) & BUTTON_MASK)
#define BUTTON_HW_INIT (LPC_GPIO1->FIODIR &= ~(BUTTON_MASK))

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
    
#define RAM_FUNC __attribute__ ((section(".ram_code")))

#define P0_0

#define GPIO_P0  0
#define GPIO_P1  1
#define GPIO_P2  2
#define GPIO_P3  3
#define GPIO_P4  4

#define GPIO_INT_LOW  1
#define GPIO_INT_HIGH 2

#define ON 1
#define OFF 0
#define GPIO_INPUT  0
#define GPIO_OUTPUT 1
#define GPIO_HIGH   1
#define GPIO_LOW    0


#define GPIO0 LPC_GPIO0
#define GPIO1 LPC_GPIO1
#define GPIO2 LPC_GPIO2
#define GPIO3 LPC_GPIO3
#define GPIO4 LPC_GPIO4


#define SETPIN GPIO0->FIOSET
#define CLRPIN GPIO0->FIOCLR
#define SETOUTPUT(x) GPIO0->FIODIR |= x
#define SETINPUT(x) GPIO0->FIODIR &= ~(x)

/* ****** PINSEL bit positions ****** */
//PINSEL1
#define PINSEL_PIN_P0_23    14
#define PINSEL_PIN_P0_24    16

//PINSEL4
#define PINSEL_PIN_P2_0     0
#define PINSEL_PIN_P2_1     2
#define PINSEL_PIN_P2_2     4
#define PINSEL_PIN_P2_3     6
#define PINSEL_PIN_P2_4     8
#define PINSEL_PIN_P2_5     10
#define PINSEL_PIN_P2_6     12

/* ****** Port pin functions ****** */
//Port 0 pin functions
#define P0_23_GPIO       0
#define P0_23_AD0_0      1
#define P0_23_I2SRX_CLK  2
#define P0_23_CAP3_0     3

#define P0_24_GPIO       0
#define P0_24_AD0_2      1
#define P0_24_I2SRX_WS   2
#define P0_24_CAP3_1     3

//Port 2 pin functions
#define P2_0_GPIO      0
#define P2_0_PWM1_1    1
#define P2_0_TXD1      2

#define P2_1_GPIO      0
#define P2_1_PWM1_2    1
#define P2_1_RXD1      2

#define P2_2_GPIO      0
#define P2_2_PWM1_3    1
#define P2_2_CTS1      2

#define P2_3_GPIO      0
#define P2_3_PWM1_4    1
#define P2_3_DCD1      2

#define P2_4_GPIO      0
#define P2_4_PWM1_5    1
#define P2_4_DSR1      2

#define P2_5_GPIO      0
#define P2_5_PWM1_6    1
#define P2_5_DTR1      2

#define P2_6_GPIO      0
#define P2_6_PACP1_0   1
#define P2_6_RI1       2

#define P2_7_GPIO      0
#define P2_7_RD2       1
#define P2_7_RTS1      2


#define PINSEL_P0_23(x) { LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(3 << PINSEL_PIN_P0_23)) | (x << PINSEL_PIN_P0_23); } 
#define PINSEL_P0_24(x) { LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(3 << PINSEL_PIN_P0_24)) | (x << PINSEL_PIN_P0_24); }

#define PINSEL_P2_0(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_0)) | (x << PINSEL_PIN_P2_0); }
#define PINSEL_P2_1(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_1)) | (x << PINSEL_PIN_P2_1); }
#define PINSEL_P2_2(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_2)) | (x << PINSEL_PIN_P2_2); }
#define PINSEL_P2_3(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_3)) | (x << PINSEL_PIN_P2_3); }
#define PINSEL_P2_4(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_4)) | (x << PINSEL_PIN_P2_4); }
#define PINSEL_P2_5(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_5)) | (x << PINSEL_PIN_P2_5); }
#define PINSEL_P2_6(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_6)) | (x << PINSEL_PIN_P2_6); }

#define PINDIR_P0_23(x) { LPC_GPIO0->FIODIR = (LPC_GPIO0->FIODIR & ~(1<<23)) | (x<<23); }
#define PINDIR_P0_24(x) { LPC_GPIO0->FIODIR = (LPC_GPIO0->FIODIR & ~(1<<24)) | (x<<23); }

#define PINDIR_P2_0(x) { LPC_GPIO2->FIODIR = (LPC_GPIO2->FIODIR & ~(1 << 0)) | (x << 0); }



//-----------------------------------------------------
void BB_Init(void);
void BB_SW_Reset(void);
void BB_ConfigClockOut(uint8_t en);
void BB_RitTimeBase_Init(void);
void BB_RitDelay(uint32_t ms);
uint32_t BB_RitTicks(void);

#ifdef __cplusplus
}
#endif

#endif






