
#include "clock.h"
#include "stm32f1xx.h"

static const uint8_t APB_DivTable[8U] =  {0, 0, 0, 0, 1, 2, 3, 4};

static uint32_t pclk1(void)
{
  return (SystemCoreClock >> APB_DivTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos]);
}

static uint32_t pclk2(void)
{
  return (SystemCoreClock >> APB_DivTable[(RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos]);
}

void CLOCK_GetAll(sysclock_t *clock)
{
    if(clock){
        return;
    }

    clock->cpu = SystemCoreClock;
    clock->clk1 = pclk1();
    clock->clk2 = pclk2();
}

uint32_t CLOCK_Get(enum clocknr clock)
{
    switch(clock){
        case CLOCK_CPU: return SystemCoreClock;
        case CLOCK_CLK1: return pclk1();
        case CLOCK_CLK2: return pclk2();
        default: break;
    }
    return 0;
}