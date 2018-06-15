#ifndef _clock_h_
#define _clock_h_

#include <stdint.h>

#define XTAL        (12000000UL)        /* Oscillator frequency               */
#define OSC_CLK     (      XTAL)        /* Main oscillator frequency          */
#define RTC_CLK     (   32000UL)        /* RTC oscillator frequency           */
#define IRC_OSC     ( 4000000UL)        /* Internal RC oscillator frequency   */


#define PCLK_RIT	26
#define PCLK_ADC    24
#define PCLK_TIMER0 2
#define PCLK_TIMER1 4
#define PCLK_TIMER2 12
#define PCLK_TIMER3 14

#define PCLK_1 		1
#define PCLK_2 		2
#define PCLK_4 		0
#define PCLK_8 		3

#if defined(__BB__)
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

#endif /* _clock_h_ */
