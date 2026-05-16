#include "at32f4xx.h"

#define VECT_TAB_OFFSET 0x0

#ifndef USE_STDPERIPH_DRIVER
#define PLL_CFGEN_ENABLE                ((uint32_t)0x80000000)
#define PLL_CFGEN_MASK                  ((uint32_t)0x80000000)
#define RCC_CFG_PLLMULT_LB_MASK         ((uint32_t)0x003C0000)
#define PLL_FREF_MASK                   ((uint32_t)0x07000000)
#define RCC_GET_PLLMULT(MULT)           ((((MULT & RCC_CFG_PLLMULT_LB_MASK) >> RCC_CFG_PLLMULT_LB_POS) | \
                                        ((MULT & RCC_CFG_PLLMULT_HB_MASK) >> (RCC_CFG_PLLMULT_HB_POS - RCC_CFG_PLLMULT_HB_OFFSET))) +\
                                        ((((MULT & RCC_CFG_PLLMULT_HB_MASK)==0) && \
                                        ((MULT & RCC_CFG_PLLMULT_LB_MASK)!=RCC_CFG_PLLMULT_LB_MASK) )? 2 : 1 ))

#define RCC_APB1PERIPH_PWR              ((uint32_t)0x10000000)
#define RCC_AUTO_STEP_EN                ((uint32_t)0x00000030)
#define PLL_MS_POS                      4
#define PLL_NS_POS                      8
#define PLL_FR_POS                      0
#define PLL_MS_MASK                     ((uint32_t)0x000000F0)
#define PLL_NS_MASK                     ((uint32_t)0x0001FF00)
#define PLL_FR_MASK                     ((uint32_t)0x00000007)
#define PLL_FR_1                        ((uint32_t)0x00000000)
#define PLL_FR_2                        ((uint32_t)0x00000001)
#define PLL_FR_4                        ((uint32_t)0x00000002)
#define PLL_FR_8                        ((uint32_t)0x00000003)
#define PLL_FR_16                       ((uint32_t)0x00000004)
#define PLL_FR_32                       ((uint32_t)0x00000005)

#define RCC_FR_VALUE(VALUE, RET)        do { \
                                            switch (VALUE) { \
                                            default: RET = 1; break; \
                                            case PLL_FR_1: RET = 1; break; \
                                            case PLL_FR_2: RET = 2; break; \
                                            case PLL_FR_4: RET = 4; break; \
                                            case PLL_FR_8: RET = 8; break; \
                                            case PLL_FR_16: RET = 16; break; \
                                            case PLL_FR_32: RET = 32; break; }\
                                        }while(0)


void RCC_PLLconfig2(uint32_t PLL_ns, uint32_t PLL_ms, uint32_t PLL_fr)
{
    uint32_t pll_reg = 0, pllrefclk = 0, pllrcfreq = 0;

    pllrefclk = RCC->CFG & RCC_CFG_PLLRC;

    if (pllrefclk == 0x00){
        /* HSI oscillator clock divided by 2 selected as PLL clock entry */
        pllrcfreq = (HSI_VALUE >> 1);
    }else{
        pllrcfreq = HSE_VALUE;
        /* HSE selected as PLL clock entry */
        if ((RCC->CFG & RCC_CFG_PLLHSEPSC) != (uint32_t)RESET)    {
            /* HSE oscillator clock divided by 2 */
            pllrcfreq = (pllrcfreq >> 1);
        }
    }

    pll_reg = RCC->PLL;

    /* Clear PLL */
    pll_reg &= ~(PLL_FR_MASK | PLL_MS_MASK | PLL_NS_MASK | PLL_FREF_MASK | PLL_CFGEN_MASK);

    /* Config pll */
    pll_reg |= ((PLL_ns << PLL_NS_POS) | (PLL_ms << PLL_MS_POS) | PLL_fr);

    /* Enable PLLGEN */
    pll_reg |= PLL_CFGEN_ENABLE;

    RCC->PLL = pll_reg;
}
#endif

uint32_t SystemCoreClock;
static const uint8_t AHBPscTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, pllmult = 0, pllrefclk = 0, tempcfg = 0;

#if defined(AT32F415xx) || defined(AT32F421xx)
    uint32_t pllcfgen = 0, pllns = 0, pllms = 0, pllfr = 0;
    uint32_t pllsrcfreq = 0, retfr = 0;
#endif
#if defined(AT32F403Axx) || defined(AT32F407xx)
    uint32_t prediv = 0;
#endif
    /* Get SYSCLK source -------------------------------------------------------*/
    tmp = RCC->CFG & RCC_CFG_SYSCLKSTS;

    switch (tmp)
    {
    case RCC_CFG_SYSCLKSTS_HSI: /* HSI used as system clock */
        SystemCoreClock = HSI_VALUE;
        break;

    case RCC_CFG_SYSCLKSTS_HSE: /* HSE used as system clock */
        SystemCoreClock = HSE_VALUE;
        break;

    case RCC_CFG_SYSCLKSTS_PLL: /* PLL used as system clock */
        /* Get PLL clock source and multiplication factor ----------------------*/
        pllrefclk = RCC->CFG & RCC_CFG_PLLRC;
        tempcfg = RCC->CFG;
        pllmult = RCC_GET_PLLMULT(tempcfg);

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

            SystemCoreClock = (pllsrcfreq * (pllns >> PLL_NS_POS)) /
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
                SystemCoreClock = (HSI_VALUE >> 1) * pllmult;
            }
            else
            {
                /* HSE selected as PLL clock entry */
                if ((RCC->CFG & RCC_CFG_PLLHSEPSC) != (uint32_t)RESET)
                {
#if defined(AT32F403Axx) || defined(AT32F407xx)
                    prediv = (RCC->MISC2 & RCC_HSE_DIV_MASK);
                    prediv = prediv >> RCC_HSE_DIV_POS;
                    /* HSE oscillator clock divided by prediv */
                    SystemCoreClock = (HSE_VALUE / (prediv + 2)) * pllmult;
#else
                    /* HSE oscillator clock divided by 2 */
                    SystemCoreClock = (HSE_VALUE >> 1) * pllmult;
#endif
                }
                else
                {
                    SystemCoreClock = HSE_VALUE * pllmult;
                }
            }
        }

        break;

    default:
        SystemCoreClock = HSI_VALUE;
        break;
    }

    /* Compute HCLK clock frequency ----------------*/
    /* Get HCLK prescaler */
    tmp = AHBPscTable[((RCC->CFG & RCC_CFG_AHBPSC) >> 4)];
    /* HCLK clock frequency */
    SystemCoreClock >>= tmp;
}

static void SystemCoreClockSet(void)
{
    __IO uint32_t StartUpCounter = 0, HSIStatus = 0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSI */
    RCC->CTRL |= ((uint32_t)RCC_CTRL_HSIEN);

    /* Wait till HSI is ready and if Time out is reached exit */
    do
    {
        HSIStatus = RCC->CTRL & RCC_CTRL_HSISTBL;
        StartUpCounter++;
    } while ((HSIStatus == 0) && (StartUpCounter != 0xFFFF));

    if ((RCC->CTRL & RCC_CTRL_HSISTBL) != RESET)
    {
        HSIStatus = (uint32_t)0x01;
    }
    else
    {
        HSIStatus = (uint32_t)0x00;
    }

    if (HSIStatus == (uint32_t)0x01)
    {
#if defined(AT32F415xx)
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 1 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_4;
#endif
        /* HCLK = SYSCLK */
        RCC->CFG |= (uint32_t)RCC_CFG_AHBPSC_DIV1;

        /* PCLK2 = HCLK/2 */
        RCC->CFG &= 0xFFFFC7FF;
        RCC->CFG |= (uint32_t)RCC_CFG_APB2PSC_DIV2;

        /* PCLK1 = HCLK/2 */
        RCC->CFG &= 0xFFFFF8FF;
        RCC->CFG |= (uint32_t)RCC_CFG_APB1PSC_DIV2;

        /*  PLL configuration: PLLCLK = ((HSI/2) * 150) / (1 * 4) = 150 MHz */
        RCC->CFG &= RCC_CFG_PLLCFG_MASK;
        RCC->CFG |= (uint32_t)(RCC_CFG_PLLRC_HSI_DIV2);
        RCC_PLLconfig2(150, 1, PLL_FR_4);

        /* Enable PLL */
        RCC->CTRL |= RCC_CTRL_PLLEN;

        /* Wait till PLL is ready */
        while ((RCC->CTRL & RCC_CTRL_PLLSTBL) == 0)
        {
        }
#if defined(AT32F413xx) || defined(AT32F403Axx) || \
    defined(AT32F407xx) || defined(AT32F415xx)
        RCC->MISC2 |= RCC_AUTO_STEP_EN;
#endif
        /* Select PLL as system clock source */
        RCC->CFG &= (uint32_t)((uint32_t) ~(RCC_CFG_SYSCLKSEL));
        RCC->CFG |= (uint32_t)RCC_CFG_SYSCLKSEL_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFG & (uint32_t)RCC_CFG_SYSCLKSTS) != RCC_CFG_SYSCLKSTS_PLL)
        {
        }
#ifdef AT32F403xx
        WaitHseStbl(PLL_STABLE_DELAY);
#endif
#if defined(AT32F413xx) || defined(AT32F403Axx) || \
    defined(AT32F407xx) || defined(AT32F415xx)
        RCC->MISC2 &= ~RCC_AUTO_STEP_EN;
#endif
    }
}

void SystemInit(void)
{
#if defined(__FPU_USED) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
#endif
    RCC->APB1EN |= RCC_APB1PERIPH_PWR;
    /* Enable low power mode, 0x40007050[bit2] */
    *(volatile uint8_t *)(0x40007050) |= (uint8_t)(0x1 << 2);
    RCC->APB1EN &= ~RCC_APB1PERIPH_PWR;

    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set HSIEN bit */
    BIT_SET(RCC->CTRL, RCC_CTRL_HSIEN);

    /* Reset SW, AHBPSC, APB1PSC, APB2PSC, ADCPSC and CLKOUT bits */
    BIT_CLEAR(RCC->CFG, RCC_CFG_SYSCLKSEL | RCC_CFG_AHBPSC |
                            RCC_CFG_APB1PSC | RCC_CFG_APB2PSC |
                            RCC_CFG_ADCPSC | RCC_CFG_CLKOUT);

    /* Reset HSEEN, HSECFDEN and PLLEN bits */
    BIT_CLEAR(RCC->CTRL, RCC_CTRL_HSEEN | RCC_CTRL_HSECFDEN |
                             RCC_CTRL_PLLEN);

    /* Reset HSEBYPS bit */
    BIT_CLEAR(RCC->CTRL, RCC_CTRL_HSEBYPS);

    /* Reset PLLRC, PLLHSEPSC, PLLMUL, USBPSC and PLLRANGE bits */
    BIT_CLEAR(RCC->CFG, RCC_CFG_PLLRC | RCC_CFG_PLLHSEPSC |
                            RCC_CFG_PLLMULT | RCC_CFG_USBPSC | RCC_CFG_PLLRANGE);

    /* Reset USB768B, CLKOUT[3], HSICAL_KEY[7:0] */
    BIT_CLEAR(RCC->MISC, 0x010100FF);

    /* Disable all interrupts and clear pending bits  */
    RCC->CLKINT = RCC_CLKINT_LSISTBLFC | RCC_CLKINT_LSESTBLFC |
                  RCC_CLKINT_HSISTBLFC | RCC_CLKINT_HSESTBLFC |
                  RCC_CLKINT_PLLSTBLFC | RCC_CLKINT_HSECFDFC;

    /* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
    /* Configure the Flash Latency cycles and enable prefetch buffer */
    SystemCoreClockSet();
    SystemCoreClockUpdate();
#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
#else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
#endif
}

/**
 * @brief       PLLCLK = fosc / pll_ms * pll_ns / pll_fr
 * @param fin
 * @param fout
 * @return
 */
static uint32_t SystemComputePll(uint32_t fin, uint32_t fout)
{
    uint32_t pll_cfg;

    fin /= 1000;

    uint32_t div = fout / fin;

    fout /= 1000;

    if((div % 1000) == 0){
        pll_cfg = 0;
        if(div > 64){
            RCC->CFG |= RCC_CFG_PLLHSEPSC;
        }

        // é tarde e não me aptece fazer melhor
        pll_cfg |= (3 << 24);
        // Isto esta errad0
        RCC->CFG = (RCC->CFG & ~RCC_CFG_PLLMULT) | (div << 18);

        return fin * div;
    }

    // use flexible configuration

    pll_cfg = RCC_PLL_PLLCFGEN;

    for (uint32_t ms = 1; ms <= 64; ms++)
    {
        uint32_t fref = fin / ms;

        // 2 MHz <= FIN/MS <= 16 MHz
        if (fref < 2000U || fref > 16000U)
            continue;

        for (uint32_t fr = 1; fr <= 64; fr++)
        {
            uint32_t numerator   = (uint32_t)fout * ms * fr;

            // Require exact integer multiplier
            if ((numerator % fin) != 0)
                continue;

            uint32_t ns = (uint32_t)(numerator / fin);

            if (ns == 0)
                continue;

            uint32_t vco = ((uint32_t)fin * ns) / ms;

            // 500 MHz <= VCO <= 1000 MHz
            if (vco < 500000ULL || vco > 1000000ULL)
                continue;

            pll_cfg |= (ns << 8) | (ms << 4) | (fr << 0);

            RCC->PLL = pll_cfg;

            return vco * 1000 / fr;
        }
    }

    return 0;
}

/**
 * @brief
 * @param fosc      Input frequency
 * @param pll_src   Source of input frequency, 0:HIC, 1:HEXT
 * @param pllclk    Desired output clock
 * @return          Real pllclk, 0 if fail
 */
uint32_t SystemConfigPll(uint8_t pll_src, uint32_t fosc, uint32_t pllclk)
{
    __IO uint32_t StartUpCounter = 0, HSIStatus = 0;
    uint32_t fout;

    // Check if system is currenly using pll
    if(((RCC->CTRL & RCC_CFG_SYSCLKSTS) >> 2) == 2){
        return 0;
    }

    pll_src &= 1;

    if(pll_src){
        // HEXT
        RCC->CTRL |= ((uint32_t)RCC_CTRL_HSEEN);

        do{
            HSIStatus = RCC->CTRL & RCC_CTRL_HSESTBL;
            StartUpCounter++;
        } while ((HSIStatus == 0) && (StartUpCounter != 0xFFFF));

        if ((RCC->CTRL & RCC_CTRL_HSESTBL) == RESET) {
            // Fail to get stable
            return 0;
        }
    }else{
        // HICK
        RCC->CTRL |= ((uint32_t)RCC_CTRL_HSISTBL);

        do{
            HSIStatus = RCC->CTRL & RCC_CTRL_HSESTBL;
            StartUpCounter++;
        } while ((HSIStatus == 0) && (StartUpCounter != 0xFFFF));

        if ((RCC->CTRL & RCC_CTRL_HSISTBL) == RESET) {
            // Fail to get stable
            return 0;
        }
    }

    // Disable pll
    RCC->CTRL &= ~RCC_CTRL_PLLEN;
    // Set clock src
    RCC->CFG = (RCC->CFG & ~RCC_CFG_PLLRC) | (pll_src << 16);

    fout = SystemComputePll(fosc, pllclk);

    if(fout){
        // Enable pll
        StartUpCounter = 0;
        RCC->CTRL |= RCC_CTRL_PLLEN;

        do{
            HSIStatus = RCC->CTRL & RCC_CTRL_PLLSTBL;
            StartUpCounter++;
        } while ((HSIStatus == 0) && (StartUpCounter != 0xFFFF));
    }

    return fout;
}

/**
 * @brief
 * @param src   Switch system clock
 * @return      0 on timeout, src otherwise
 */
uint32_t SystemConfigClockSrc(uint8_t src)
{
    __IO uint32_t StartUpCounter = 0, HSIStatus = 0;

    src &= 3;

    /* Select PLL as system clock source */
    RCC->CFG = (RCC->CFG & ~(RCC_CFG_SYSCLKSEL)) | src;

    do{
        HSIStatus = (RCC->CTRL & RCC_CFG_SYSCLKSTS) >> 2;
        if(HSIStatus == src){
            return src;
        }
    } while ((++StartUpCounter) != 0xFFFF);

    return 0;
}