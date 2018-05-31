/**
* @file		common.h
* @brief	Common headers for drivers
*
* @version	1.0
* @date		15 Agos. 2017
* @author	Hugo Reis
**********************************************************************/

#ifndef _common_h_
#define _common_h_

#include <stdint.h>

#if defined(__TDSO__)
#include <stm32f103xb.h>

uint32_t GetTicks(void);
void DelayMs(uint32_t ms);

#elif defined(__BB__) /* __TDO__ */

#include <LPC17xx.h>
#include <blueboard.h>

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
#define DelayMs(x) ets_delay_us(x * 1000)
#define GetTicks() xthal_get_ccount()

#define call_delay_us( time ) { asm volatile( "mov.n a2, %0\n_call0 delay4clk" : : "r"(time*13) : "a2" ); }

#endif /* __EMU__ */


#define ElapsedTicks(x) (GetTicks() - x)

void Board_Init(void);

#endif /* _common_h_ */
