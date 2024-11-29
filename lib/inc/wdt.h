#ifndef _wdt_h_
#define _wdt_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void WDT_Init(uint32_t timeout);
void WDT_Stop(void);
void WDT_Reset(void);

#ifdef __cplusplus
}
#endif

#endif