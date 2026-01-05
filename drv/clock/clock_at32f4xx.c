#include "at32f4xx.h"
#include "clock.h"

static const uint8_t APBAHBPscTable[8] = {1, 2, 3, 4, 6, 7, 8, 9};
static const uint8_t ADCPscTable[8] = {2, 4, 6, 8, 2, 12, 8, 16};

/**
 * @brief Get cpu clock frequency bases on configuration registers
 * @param clk
 */
static uint32_t sclk(void)
{
    uint32_t pllns = 0, pllms = 0, pllfr = 0;
    uint32_t pllsrcfreq = 0;
    uint32_t tmp = 0, pllmult = 0, pllrefclk = 0;

    /* Get SYSCLK source -------------------------------------------------------*/
    switch (RCC->CFG & RCC_CFG_SYSCLKSTS)
    {
        case 0x00: /* HSI used as system clock */
            return ((RCC->MISC & RCC_MISC_HSI_DIV_EN) && (RCC->MISC2 & RCC_MISC2_HSI_SYS_CTRL)) ?
                HSI_VALUE * 6 : HSI_VALUE;

        case 0x04: /* HSE used as system clock */
            return HSE_VALUE;

        case 0x08: /* PLL used as system clock */
            pllrefclk = RCC->CFG & RCC_CFG_PLLRC;
            tmp = RCC->PLL;
            /* Get_ClocksFreq for PLLconfig2 */
            if(tmp & RCC_PLL_PLLCFGEN){
                /* Flexible configuration mode */
                pllfr = (1 << (tmp & RCC_PLL_PLLFR));
                pllns = (tmp & RCC_PLL_PLLNS) >> 8;
                pllms = (tmp & RCC_PLL_PLLMS) >> 4;
                if(pllrefclk == 0x00){
                    /* HSI oscillator clock divided by 2 selected as PLL clock entry */
                    pllsrcfreq = (HSI_VALUE >> 1);
                }else{
                    /* HSE selected as PLL clock entry */
                    if((RCC->CFG & RCC_CFG_PLLHSEPSC) != (uint32_t)RESET){
                        pllsrcfreq = (HSE_VALUE >> 1);
                    }else{
                        pllsrcfreq = HSE_VALUE;
                    }
                }
                return (pllsrcfreq * pllns) / (pllms * pllfr);
            }else{
                /* Get PLL clock source and multiplication factor ----------------------*/
                tmp = (RCC->CFG & RCC_CFG_PLLMULT) >> 18;
                pllmult = tmp & 0b1111;
                pllmult |= (tmp >> 8);

                if(pllrefclk == 0x00) {
                    /* HSI oscillator clock divided by 2 selected as PLL clock entry */
                    return (HSI_VALUE >> 1) * pllmult;
                }else{
                    /* HSE selected as PLL clock entry */
                    if (RCC->CFG & RCC_CFG_PLLHSEPSC){
                        /* HSE oscillator clock divided by 2 */
                        return (HSE_VALUE >> 1) * pllmult;
                    }else{
                        return HSE_VALUE * pllmult;
                    }
                }
            }
        default:
            return HSI_VALUE;
    }
}

static uint32_t hclk(uint32_t sclk)
{
    uint32_t tmp, psc;
    /* Compute HCLK, PCLK1, PCLK2 and ADCCLK clocks frequencies ----------------*/
    /* Get HCLK prescaler */
    tmp = (RCC->CFG & (RCC_CFG_AHBPSC ^ RCC_CFG_AHBPSC_3)) >> 4;
    psc = (RCC->CFG & RCC_CFG_AHBPSC_3) ? APBAHBPscTable[tmp] : 0;

    return sclk >> psc;
}

static uint32_t pclk1(uint32_t hclk)
{
    uint32_t tmp, psc;
    /* Get PCLK1 prescaler */
    tmp = (RCC->CFG & (RCC_CFG_APB1PSC ^ RCC_CFG_APB1PSC_2)) >> 8;
    psc = (RCC->CFG & RCC_CFG_APB1PSC_2) ? APBAHBPscTable[tmp] : 0;

    return hclk >> psc;
}

static uint32_t pclk2(uint32_t hclk)
{
    uint32_t tmp, psc;
    /* Get PCLK2 prescaler */
    tmp = (RCC->CFG & (RCC_CFG_APB2PSC ^ RCC_CFG_APB2PSC_2)) >> 11;
    psc = (RCC->CFG & RCC_CFG_APB2PSC_2) ? APBAHBPscTable[tmp] : 0;

    return hclk >> psc;
}

static uint32_t adcclk(uint32_t pclk2)
{
    uint32_t tmp, psc;
    /* Get ADCCLK prescaler */
    tmp = (RCC->CFG & RCC_CFG_ADCPSC_DIV16) >> 14;
    tmp = (tmp & 0b11) | (tmp >> 12);
    psc = ADCPscTable[tmp];

    return pclk2 / psc;
}

void CLOCK_GetAll(sysclock_t *clk)
{
    uint32_t h_clk = hclk(sclk());
    /* HCLK clock frequency */
    clk->cpu = h_clk;
    /* PCLK1 clock frequency */
    clk->clk1 = pclk1(h_clk);
    /* PCLK2 clock frequency */
    clk->clk2 = pclk2(h_clk);
    /* ADCCLK clock frequency */
    clk->clk3 = adcclk(clk->clk2);
}

uint32_t CLOCK_Get(enum clocknr clock)
{
    uint32_t clk = sclk();

    switch(clock){
        case CLOCK_CPU:  return hclk(clk);
        case CLOCK_CLK1: return pclk1(clk);
        case CLOCK_CLK2: return pclk2(clk);
        case CLOCK_CLK3: return adcclk(pclk2(clk));
        default: return 0;
    }
}

void CLOCK_Enable(uint32_t per, uint8_t state)
{
    (void)per;
    (void)state;
}
