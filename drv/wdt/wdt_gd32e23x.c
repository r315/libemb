#include "wdt.h"
#include "gd32e23x_rcu.h"

#define FWDGT_MIN_TIMEOUT    10      //1.0ms
#define FWDGT_MAX_TIMEOUT    262144  //26214.4ms
#define FWDGT_LICK           40000UL //Hz
#define FWDGT_MAX_RLD        0x1000

#define DHCSR               *((uint32_t*)0xE000EDF0)    /*!< Debug Halting Control and Status Register */

/**
 * @brief Independend watchdog configuration
 *
 *
 * @param expire in miliseconds
 */
void WDT_Init(uint32_t expire)
{
    uint32_t tpre;
    uint32_t flag_status = RESET;
    uint32_t timeout = FWDGT_PSC_TIMEOUT;

    if ((DHCSR & 0x1) != 0) {
        // debugger is attached, return
        return;
    }

    if(expire < FWDGT_MIN_TIMEOUT){
        expire = FWDGT_MIN_TIMEOUT;
    }

    if(expire > FWDGT_MAX_TIMEOUT){
        expire = FWDGT_MAX_TIMEOUT;
    }

    uint32_t div = 0;
    uint32_t fiwdg = FWDGT_LICK / 4;

    do{
        tpre = (FWDGT_MAX_RLD * 1000UL) / fiwdg;
        if(tpre > expire){
            break;
        }
        div++;
        fiwdg >>= 1;
    }while(div < 6);

    tpre = (fiwdg * expire) / 1000UL;

    FWDGT_CTL = FWDGT_WRITEACCESS_ENABLE;

    do{
        flag_status = FWDGT_STAT & FWDGT_STAT_PUD;
    }while((--timeout > 0U) && ((uint32_t)RESET != flag_status));

    FWDGT_PSC = div;

    timeout = FWDGT_RLD_TIMEOUT;

    do{
        flag_status = FWDGT_STAT & FWDGT_STAT_RUD;
    }while((--timeout > 0U) && ((uint32_t)RESET != flag_status));

    FWDGT_RLD = tpre;

    FWDGT_CTL = FWDGT_KEY_RELOAD;

    FWDGT_CTL = FWDGT_KEY_ENABLE;
}

void WDT_Stop(void)
{
    DBG_CTL0 |= DBG_CTL0_FWDGT_HOLD;
}

void WDT_Reset(void)
{
    FWDGT_CTL = FWDGT_KEY_RELOAD;
}