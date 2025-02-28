#include "wdt.h"
#include "at32f4xx.h"

#define IWDG_MIN_TIMEOUT    10      //1.0ms
#define IWDG_MAX_TIMEOUT    262144  //26214.4ms
#define IWDG_LICK           40000UL //Hz
#define IWDG_MAX_RLD        0x1000

#define DHCSR               *((uint32_t*)0xE000EDF0) // Debug Halting Control and Status Register

/**
 * @brief Independend watchdog configuration
 * 
 * 
 * @param timeout in miliseconds
 */
void WDT_Init(uint32_t timeout)
{
    uint32_t tpre;

    if ((DHCSR & 0x1) != 0) {
        // debugger is attached, return
        return;
    }

    if(timeout < IWDG_MIN_TIMEOUT){
        timeout = IWDG_MIN_TIMEOUT;
    }

    if(timeout > IWDG_MAX_TIMEOUT){
        timeout = IWDG_MAX_TIMEOUT;
    }

    uint32_t div = 0;
    uint32_t fiwdg = IWDG_LICK / 4;

    do{
        tpre = (IWDG_MAX_RLD * 1000UL) / fiwdg;
        if(tpre > timeout){
            break;
        }
        div++;
        fiwdg >>= 1;
    }while(div < 6);

    tpre = (fiwdg * timeout) / 1000UL;

    IWDG->KEY = 0x5555;

    IWDG->RLD = tpre;
    IWDG->PSC = div;

    while(IWDG->STS & (IWDG_STS_RLDF | IWDG_STS_PSCF));

    IWDG->KEY = 0xCCCC;
}

void WDT_Stop(void)
{
    MCUDBG->CTRL |= MCUDBG_CTRL_DBG_IWDG_STOP;
}

void WDT_Reset(void)
{
    IWDG->KEY = 0xAAAA;
}