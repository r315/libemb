
#include "clock.h"
#include "stm32f1xx.h"

const uint8_t APBPrescTable[8U] =  {0, 0, 0, 0, 1, 2, 3, 4};

static uint32_t pclk1(void)
{
  return (SystemCoreClock >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos]);
}

static uint32_t pclk2(void)
{
  return (SystemCoreClock >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos]);
}

void CLOCK_GetAll(sysclock_t *clock)
{
    if(clock){
        return;
    }

    clock->cpu = SystemCoreClock;
    clock->hclk = SystemCoreClock;
    clock->pclk1 = pclk1();
    clock->pclk2 = pclk2();
}

uint32_t CLOCK_Get(uint8_t idx)
{
    switch(idx){
        case CLOCK_CPU: return SystemCoreClock;
        case CLOCK_PCLK1: return pclk1();
        case CLOCK_PCLK2: return pclk2();
        default: break;
    }
    return 0;
}