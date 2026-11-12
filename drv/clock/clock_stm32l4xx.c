
#include "clock.h"
#include "stm32l4xx.h"

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

void CLOCK_Enable(void *peripheral, uint8_t state)
{
    uint32_t per = (uint32_t)peripheral;
    uint32_t shift = (per >> 10) & 0x3F;
    uint32_t mask = (1 << shift);

    if(per >= AHB2PERIPH_BASE){
        per = (uint32_t)&RCC->AHB2ENR;
    }else if(per >= AHB1PERIPH_BASE){
        per = (uint32_t)&RCC->AHB1ENR;
    }else if(per >= APB2PERIPH_BASE){
        per = (uint32_t)&RCC->APB2ENR;
    }else if(per >= APB1PERIPH_BASE + 0x8000){
        per = (uint32_t)&RCC->APB1ENR2;
    }else {
        per = (uint32_t)&RCC->APB1ENR1;
    }

    if(state){
        *(uint32_t*)per |= mask;
    }else{
        *(uint32_t*)per &= ~mask;
    }
}

/**
 * @brief Enable/disable Main Clock Output.
 *      Configures PA8 to output the desired clock
 *
 * @param src Source of output clock and division
 *
 *   src[3:0]:
 *          MCO_NONE  - disabled
 *          MCO_SYSCLK
 *          MCO_MSI
 *          MCO_HSI16
 *          MCO_HSE
 *          MCO_PLL
 *          MCO_LSI
 *          MCO_LSE
 *          MCO_HSI48
 *
 *   src[7:4]:
 *          MCO_DIV_1
 *          .....
 *          MCO_DEV_16
 *
 */
void CLOCK_Output(uint32_t src)
{
    src = src & 0x7f;

    if(!src){
        // PA8 Input float
        GPIOA->MODER = (GPIOA->MODER & ~(3 << 16));
        // Disable clock out source
        RCC->CFGR = (RCC->CFGR & ~(15 << 24));
    }else{
        // Set CLKOUT_SEL[2:0]
        RCC->CFGR = (RCC->CFGR & ~(0x7F << 24)) | (src << 24);
        // PA8 multiplexed push-pull
        GPIOA->OSPEEDR |= (3 << 16);
        GPIOA->OTYPER &= ~(1 << 8);
        GPIOA->MODER = (GPIOA->MODER & ~(3 << 16)) | (2 << 16);
    }
}
