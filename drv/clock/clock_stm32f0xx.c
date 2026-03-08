
#include "clock.h"
#include "stm32f0xx.h"

/**
 * STM32F04/7/9
 *
 * HSI = 8MHz
 * HSI48 = 48MHz
 *
 * SYSCLK, HCLK, PCLK
 */


extern const uint8_t AHBPrescTable[16];
extern const uint8_t APBPrescTable[8];

static uint32_t hclk(void)
{
  return (SystemCoreClock >> AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos]);
}

static uint32_t pclk(void)
{
  return (SystemCoreClock >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE) >> RCC_CFGR_PPRE_Pos]);
}

void CLOCK_GetAll(sysclock_t *clock)
{
    if(!clock){
        return;
    }

    clock->cpu = hclk();;
    clock->clk1 = pclk();
}

uint32_t CLOCK_Get(enum clocknr clock)
{
    switch(clock){
        case CLOCK_CPU: return hclk();
        case CLOCK_CLK1: return pclk();
        default: break;
    }
    return 0;
}