#ifndef _clock_lpc17xx_h_
#define _clock_lpc17xx_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


#define XTAL        (12000000UL)        /* Oscillator frequency               */
#define OSC_CLK     (      XTAL)        /* Main oscillator frequency          */
#define RTC_CLK     (   32000UL)        /* RTC oscillator frequency           */
#define IRC_OSC     ( 4000000UL)        /* Internal RC oscillator frequency   */


#define CLOCK_OUT {LPC_SC->CLKOUTCFG = (1<<8); /* CCLK/2, CLKOU_EN */     \
	              LPC_PINCON->PINSEL3 |= (1<<22);}    /* P1.27 CLKOUT */  \


//PCLKSEL0 Bits
#define PCLK_TIMER0_pos      2
#define PCLK_TIMER1_pos      4
#define PCLK_UART0_pos       6
#define PCLK_UART1_pos       8
#define PCLK_PWM1_pos        12
#define PCLK_I2C0_pos        14
#define PCLK_SPI_pos         16
#define PCLK_SSP1_pos        20
#define PCLK_DAC_pos         22
#define PCLK_ADC_pos         24
#define PCLK_CAN1_pos        26
#define PCLK_CAN2_pos        28
#define PCLK_ACF_pos         30

//PCLKSEL1 Bits
#define PCLK_RIT_pos         26
#define PCLK_TIMER2_pos      12
#define PCLK_TIMER3_pos      14

// PCLK Peripherals index
#define PCLK_WDT        0
#define PCLK_TIMER0     1
#define PCLK_TIMER1     2
#define PCLK_UART0      3
#define PCLK_UART1      4
#define PCLK_PWM1       6
#define PCLK_I2C0       7
#define PCLK_SPI        8
#define PCLK_SSP1       10
#define PCLK_DAC        11
#define PCLK_ADC        12
#define PCLK_CAN1       13
#define PCLK_CAN2       14
#define PCLK_ACF        15

#define PCLK_QEI        16
#define PCLK_GPIOINT    17
#define PCLK_PCB        18
#define PCLK_I2C1       19
#define PCLK_SSP0       21
#define PCLK_TIMER2     22
#define PCLK_TIMER3     23
#define PCLK_UART2      24
#define PCLK_UART3      25
#define PCLK_I2C2       26
#define PCLK_I2S        27
#define PCLK_RIT        29
#define PCLK_SYSCON     30
#define PCLK_MC         31


// CCLK Dividers
#define PCLK_1 		1
#define PCLK_2 		2
#define PCLK_4 		0
#define PCLK_8 		3

//
#define CCLK_DIV1   1
#define CCLK_DIV2   2
#define CCLK_DIV4   0
#define CCLK_DIV8   3 

// Core clock
#define CCLK_100	100
#define CCLK_80		80
#define CCLK_72		72
#define CCLK_48		48

#define __USE_SYSTICK

 /**
 * @brief get current system clock
 **/
 uint32_t CLOCK_GetCCLK(void);

/**
 * @brief set system clock, for values < 48Mhz internal osccilator is activated
 *        on invalid clock settings defaults to 48Mhz
 **/
void CLOCK_Init(uint32_t cclk);

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

uint32_t CLOCK_GetTicks(void);

/**
 * @brief returns ticks passed from the parameter ticks
 **/
uint32_t CLOCK_ElapsedTicks(uint32_t ticks);

/**
 *  Get the current clock (Hz) for the supplied peripheral
 **/
uint32_t CLOCK_GetPCLK(uint8_t peripheral);

/**
 *  Set cclk divider for the supplied peripheral
 **/
void CLOCK_SetPCLK(uint8_t peripheral, uint8_t div);


#ifdef __cplusplus
}
#endif

#endif /* _clock_h_ */
