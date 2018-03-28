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

#if defined(__TDSO__)

#if defined(USE_HAL_DRIVER)
	#include "stm32f1xx_hal.h"
	#define DelayMs(x) HAL_Delay(x)
	#define GetTick() HAL_GetTick()
#else
	#error "common.h: missing macros"
#endif /* USE_HAL_DRIVER */

#elif defined(__BLUEBOARD__) /* __TDO__ */

#include <LPC17xx.h>
#include <blueboard.h>
#define GetTick GetTicks

#elif defined(__EMU__) /* __BLUEBOARD__ */

#include <SDL2/SDL.h>
#define DelayMs(x) SDL_Delay(x)
#define GetTick() SDL_GetTicks()
#define SystemCoreClock 100000000UL

#elif defined(__ESP03__) /* __EMU__  */
//#include <system.h>
#define DelayMs(x) ets_delay_us(x * 1000)
#endif /* __EMU__ */


#define ElapsedTicks(x) (GetTick() - x)

#endif /* _common_h_ */
