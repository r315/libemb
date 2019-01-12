#ifndef _clock_h_
#define _clock_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

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
