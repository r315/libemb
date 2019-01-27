#ifndef _libemb_h_
#define _libemb_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>  // NULL

typedef void (*CallBack)(void *);


#if defined(__TDSO__)
#include <stm32f103xb.h>

uint32_t GetTicks(void);
void DelayMs(uint32_t ms);
#define ON  1
#define OFF 0


#elif defined(__BB__) /* __TDO__ */

#include <LPC17xx.h>
#include <blueboard.h>
#include <clock_lpc17xx.h>
#include <gpio.h>
#include <timer.h>
#include <pwm.h>
#include <i2c.h>
#include <dac.h>

#define DelayMs CLOCK_DelayMs
#define GetTicks CLOCK_GetTicks

#define YES 1
#define NO  0

#elif defined(__EMU__) /* __BLUEBOARD__ */
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#define DelayMs(x) SDL_Delay(x)
#define GetTicks() SDL_GetTicks()
#define SystemCoreClock 100000000UL

#elif defined(__ESP03__) /* __EMU__  */
//#include "c_types.h"
//#include "esp8266_auxrom.h"
//#include "esp8266_rom.h"
//#include "eagle_soc.h"
//#include "ets_sys.h"
//#include "nosdk8266.h"
//#include "nosdki2s.h"
#include "esp8266/rom_functions.h"
#define DelayMs(x) ets_delay_us(x * 1000)
#define GetTicks() xthal_get_ccount()

#define call_delay_us( time ) { asm volatile( "mov.n a2, %0\n_call0 delay4clk" : : "r"(time*13) : "a2" ); }

#endif /* __EMU__ */

//#define NULL (void*)0
#define ElapsedTicks(x) (GetTicks() - (x))

void Board_Init(void);


#ifdef __cplusplus
}
#endif

#endif
