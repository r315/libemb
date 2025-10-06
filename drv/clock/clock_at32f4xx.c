#include "at32f4xx.h"
#include "clock.h"

#error "Tjis is still in development"

static const uint8_t APBAHBPscTable[8] = {1, 2, 3, 4, 6, 7, 8, 9};
static const uint8_t ADCPscTable[8] = {2, 4, 6, 8, 2, 12, 8, 16};

static void clock_cpu(sysclock_t *clk)
{
#if defined(AT32F415xx) || defined(AT32F421xx)
    uint32_t pllcfgen = 0, pllns = 0, pllms = 0, pllfr = 0;
    uint32_t pllsrcfreq = 0, retfr = 0;
#endif
#if defined(AT32F403Axx) || defined(AT32F407xx)
    uint32_t prediv = 0;
#endif
    uint32_t tmp = 0, pllmult = 0, pllrefclk = 0, psc = 0;

    /* Get SYSCLK source -------------------------------------------------------*/
    tmp = RCC->CFG & RCC_CFG_SYSCLKSTS;

    switch (tmp)
    {
    case 0x00: /* HSI used as system clock */
#if !defined(AT32F403xx)
        if (BIT_READ(RCC->MISC, RCC_HSI_DIV_EN) && BIT_READ(RCC->MISC2, RCC_HSI_SYS_CTRL))
            clk->cpu = HSI_VALUE * 6;
        else
            clk->cpu = HSI_VALUE;
#else
        clk->cpu = HSI_VALUE;
#endif
        break;

    case 0x04: /* HSE used as system clock */
        clk->cpu = HSE_VALUE;
        break;

    case 0x08: /* PLL used as system clock */
        pllrefclk = RCC->CFG & RCC_CFG_PLLRC;

#if defined(AT32F415xx) || defined(AT32F421xx)
        /* Get_ClocksFreq for PLLconfig2 */
        pllcfgen = BIT_READ(RCC->PLL, PLL_CFGEN_MASK);

        if (pllcfgen == PLL_CFGEN_ENABLE)
        {
            pllns = BIT_READ(RCC->PLL, PLL_NS_MASK);
            pllms = BIT_READ(RCC->PLL, PLL_MS_MASK);
            pllfr = BIT_READ(RCC->PLL, PLL_FR_MASK);

            RCC_FR_VALUE(pllfr, retfr);

            if (pllrefclk == 0x00)
            {
                /* HSI oscillator clock divided by 2 selected as PLL clock entry */
                pllsrcfreq = (HSI_VALUE >> 1);
            }
            else
            {
                /* HSE selected as PLL clock entry */
                if ((RCC->CFG & RCC_CFG_PLLHSEPSC) != (uint32_t)RESET)
                {
                    pllsrcfreq = (HSE_VALUE >> 1);
                }
                else
                {
                    pllsrcfreq = HSE_VALUE;
                }
            }

            clk->cpu = (pllsrcfreq * (pllns >> PLL_NS_POS)) /
                        ((pllms >> PLL_MS_POS) * retfr);
        }
        else
#endif
        {
            /* Get PLL clock source and multiplication factor ----------------------*/
            pllmult = BIT_READ(RCC->CFG, RCC_CFG_PLLMULT);
            pllmult = RCC_GET_PLLMULT(pllmult);

            if (pllrefclk == 0x00)
            {
                /* HSI oscillator clock divided by 2 selected as PLL clock entry */
                clk->cpu = (HSI_VALUE >> 1) * pllmult;
            }
            else
            {
                /* HSE selected as PLL clock entry */
                if ((RCC->CFG & RCC_CFG_PLLHSEPSC) != (uint32_t)RESET)
                {
#if defined(AT32F403Axx) || defined(AT32F407xx)
                    prediv = (RCC->MISC2 & RCC_HSE_DIV_MASK);
                    prediv = prediv >> RCC_HSE_DIV_POS;
                    /* HSE oscillator clock divided by 2 */
                    clk->cpu = (HSE_VALUE / (prediv + 2)) * pllmult;
#else
                    /* HSE oscillator clock divided by 2 */
                    clk->cpu = (HSE_VALUE >> 1) * pllmult;
#endif
                }
                else
                {
                    clk->cpu = HSE_VALUE * pllmult;
                }
            }
        }
#if !defined(AT32F415xx) && !defined(AT32F421xx)
        if (((RCC->CFG & RCC_CFG_PLLRANGE) == 0) && (RCC_Clocks->SYSCLK_Freq > RCC_PLL_RANGE))
        {
            /* Not setup PLLRANGE, fixed in 72 MHz */
            clk->cpu = RCC_PLL_RANGE;
        }
#endif
        break;

    default:
        clk->cpu = HSI_VALUE;
        break;
    }

}

void CLOCK_GetAll(sysclock_t *clk)
{
    uint32_t tmp = 0, psc = 0;
    clock_cpu(clk);

    /* Compute HCLK, PCLK1, PCLK2 and ADCCLK clocks frequencies ----------------*/
    /* Get HCLK prescaler */
    tmp = (RCC->CFG & (RCC_CFG_AHBPSC ^ RCC_CFG_AHBPSC_3)) >> 4;
    psc = (RCC->CFG & RCC_CFG_AHBPSC_3) ? APBAHBPscTable[tmp] : 0;

    /* HCLK clock frequency */
    clk->hclk = clk->cpu >> psc;
    /* Get PCLK1 prescaler */
    tmp = (RCC->CFG & (RCC_CFG_APB1PSC ^ RCC_CFG_APB1PSC_2)) >> 8;
    psc = (RCC->CFG & RCC_CFG_APB1PSC_2) ? APBAHBPscTable[tmp] : 0;
    /* PCLK1 clock frequency */
    clk->pclk1 = clk->hclk >> psc;
    /* Get PCLK2 prescaler */
    tmp = (RCC->CFG & (RCC_CFG_APB2PSC ^ RCC_CFG_APB2PSC_2)) >> 11;
    psc = (RCC->CFG & RCC_CFG_APB2PSC_2) ? APBAHBPscTable[tmp] : 0;
    /* PCLK2 clock frequency */
    clk->pclk2 = clk->hclk >> psc;

    /* Get ADCCLK prescaler */
    tmp = (RCC->CFG & CFG_ADCPSC_Set_Mask) >> CFG_ADCPSC_Pos;

    if (RCC->CFG & CFG_ADCPSCBit2_Set_Mask)
    {
        tmp |= CFG_ADCPSCBit2_IdxMask;
    }

    psc = ADCPscTable[tmp];
    /* ADCCLK clock frequency */
    clk->pclk3 = clk->pclk2 / psc;
}

void CLOCK_Enable(uint32_t per, uint8_t state)
{
}
