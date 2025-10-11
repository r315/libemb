#ifndef _wdt_h_
#define _wdt_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Initializes watchdog timer
 * @param timeout expire time in milliseconds
 */
void WDT_Init(uint32_t timeout);
/**
 * @brief Stop/disable watchdog
 * @param
 */
void WDT_Stop(void);
/**
 * @brief Watchdog reset, called in application loop
 * @param
 */
void WDT_Reset(void);

#ifdef __cplusplus
}
#endif

#endif