#ifndef _clock_h_
#define _clock_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if defined(__BB__)
#include <LPC17xx.h>

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

#define SET_PCLK_TIMER3(x) {LPC_SC->PCLKSEL1 &= ~(3<<PCLK_TIMER3); LPC_SC->PCLKSEL1 |= (x<<PCLK_TIMER3);}
#define SET_PCLK_PWM1(x)  {LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & ~(3 << PCLK_PWM1)) | (x << PCLK_PWM1);}

#define __USE_SYSTICK
#endif

 /**
 * @brief get current system clock
 **/
 int CLOCK_GetCCLK(void);

/**
 * @brief set system clock, for values < 48Mhz internal osccilator is activated
 *        on invalid clock settings defaults to 48Mhz
 **/
void CLOCK_Init(unsigned int cclk);

/**
 * @brief set PLL1 to generat 48Mhz clock for usb 
 **/
void CLOCK_InitUSBCLK(void);

/**
 * @brief delay ms function
 *         this function uses systick
 **/
void CLOCK_DelayMs(uint32_t ms);
 
 /**
 * @brief returns ms passed from the last powerup/reset
 **/

unsigned int CLOCK_GetTicks(void);

/**
 * @brief returns ticks passed from the parameter ticks
 **/
unsigned int CLOCK_ElapsedTicks(unsigned int ticks);

#ifdef __cplusplus
}
#endif

#endif /* _clock_h_ */
