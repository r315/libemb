#include "stm32f1xx.h"
#include "wdt.h"

#define DHCSR               *((uint32_t*)0xE000EDF0) // Debug Halting Control and Status Register

/**
 * @brief Independend watchdog configuration
 *
 *
 * @param timeout in miliseconds
 */
void WDT_Init(uint32_t interval)
{
    uint32_t timeout = 4096;
    uint8_t pres = 0;

    if ((DHCSR & 0x1) != 0) {
        // debugger is attached, return
        return;
    }

    interval *= 10;

    if(interval > 0xFFFF){
        interval = 0xFFFF;
    }

    while( interval > timeout){
        timeout <<= 1;
        pres++;
    }

    if(IWDG->SR != 0){
        // other update is in progress
        return;
    }

    IWDG->KR = 0x5555; // Enable access to PR and RLR registers
    IWDG->PR = pres;
    IWDG->RLR = (interval * 0xFFFF) / timeout;
    IWDG->KR = 0xAAAA;  // Reload
    IWDG->KR = 0xCCCC;  // Start IWDG
}

void WDT_Stop(void)
{

}

void WDT_Reset(void)
{
    IWDG->KR = 0xAAAA; // Reload RLR on counter
}