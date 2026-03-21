#include "gd32e23x.h"
#include "clock.h"

#define SYS_CLK_MAX_FREQ            80000000UL
#define HXTAL_MAX_FREQ              32000000UL
#define HXTAL_MIN_FREQ              4000000UL
#define CK_IRC8M                    8000000UL

#ifndef CK_HXTAL
#define CK_HXTAL                    8000000UL
#endif

enum clock_err{
    CLOCK_OK = 0,
    CLOCK_INVALID,
    CLOCK_NOT_STABLE,
    CLOCK_PLL_NOT_STABLE,
    CLOCK_SWITCH_FAIL
};

struct pll_cfg{
    uint32_t xtal;
    uint32_t clk;
    uint8_t mul;
    uint8_t div;
};

static const uint32_t ahb_psc_table[] = {2, 4, 8, 16, 64, 128, 256, 512};
static const uint32_t apb_psc_table[] = {2, 4, 8, 16};
static const uint32_t adc_psc_table[] = {2, 4, 6, 8, 3, 5, 7, 9};
#if 0
/**
 * @brief finds the highest possible system clock
 *
 *  sys_clk = xtal*pllmul/predv
 *
 *  predv [2,16]
 *  pllmul [2,32]
 *  xtal 4000000 to 32000000
 *
 * @param cfg struct pll_cfg
 * @return uint8_t
 */
static void system_clock_max(struct pll_cfg *cfg)
{
    cfg->clk = 0;
    cfg->div = 0;
    cfg->mul = 0;

    for (uint8_t pllmul = 2; pllmul <= 32; pllmul++) {
        uint32_t product = cfg->xtal * pllmul;
        // The optimal `predv` is the smallest value that maximizes sys_clk
        for (uint8_t predv = 2; predv <= 16; predv++) {
            uint32_t clk = product / predv;

            if(clk > SYS_CLK_MAX_FREQ){
                continue;
            }

            if (clk > cfg->clk) {
                cfg->clk = clk;
                cfg->mul = pllmul;
                cfg->div = predv;
            }

            // Since `predv` only increases the denominator, we can break early
            if (clk < cfg->clk) {
                break;
            }
        }
    }
}
/**
 * @brief Tries to configure the highest system clock
 * from a given xtal
 *
 * @param xtal
 * @return uint8_t
 */
static uint8_t system_clock_from_xtal(uint32_t xtal)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    if (xtal > HXTAL_MAX_FREQ || HXTAL_MIN_FREQ > xtal){
        return CLOCK_INVALID;
    }

    /* Enable high speed crystal */
    RCU_CTL0 |= RCU_CTL0_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than timeout */
    do{
        timeout++;
        stab_flag = (RCU_CTL0 & RCU_CTL0_HXTALSTB);
    }
    while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail, return without changing clock */
    if(!(RCU_CTL0 & RCU_CTL0_HXTALSTB)){
        return CLOCK_NOT_STABLE;
    }

    /* HXTAL is stable */
    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_2;

    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;

    struct pll_cfg cfg = {
        .xtal = xtal
    };

    system_clock_max(&cfg);

    /* pll multiplier */
    RCU_CFG0 = (RCU_CFG0 & ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF | RCU_CFG0_PLLDV)) |
               RCU_PLLSRC_HXTAL | CFG0_PLLMF(cfg.mul - 2) | ((cfg.mul > 16) ? RCU_CFG0_PLLMF4 : 0);

    /* pll prediv */
    RCU_CFG1 = (RCU_CFG1 & ~(RCU_CFG1_PREDV)) | CFG1_PREDV(cfg.div - 1);

    /* enable PLL */
    RCU_CTL0 |= RCU_CTL0_PLLEN;

    /* wait until PLL is stable */
    timeout = 0;
    do{
        timeout++;
        stab_flag = RCU_CTL0 & RCU_CTL0_PLLSTB;
    }while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    if(!(RCU_CTL0 & RCU_CTL0_PLLSTB)){
        return CLOCK_PLL_NOT_STABLE;
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;

    /* wait until PLL is selected as system clock */
     timeout = 0;
    do{
        timeout++;
        stab_flag = RCU_CFG0 & RCU_CFG0_SCSS;
    }while((RCU_SCSS_PLL != stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    if(RCU_SCSS_PLL != (RCU_CFG0 & RCU_CFG0_SCSS)){
        return CLOCK_SWITCH_FAIL;
    }

    SystemCoreClock = cfg.clk;

    return CLOCK_OK;
}
#endif

/**
 * @brief Compute current system clock speed
 *
 * \return cy_sys frequency
 */
static uint32_t cksys(uint32_t xtal)
{
    uint32_t pllmult, pllrefclk;

    switch ((RCU_CFG0 >> 2) & 3)
    {
        default:
        case 0x00:
            return CK_IRC8M;

        case 0x01:
            return xtal;

        case 0x02:
            if(RCU_CFG0 & RCU_CFG0_PLLSEL){
                pllrefclk = xtal / ((RCU_CFG1 & 15) + 1);
            }else {
                pllrefclk =  CK_IRC8M / 2;
            }

            pllmult = (RCU_CFG0 >> 18) & 0xF;
            pllmult |= (RCU_CFG0 & RCU_CFG0_PLLMF4) >> 23;

            return pllrefclk * (pllmult + 1);
    }
}

static uint32_t hclk(uint32_t cksys)
{
    uint8_t div = (RCU_CFG0 >> 4) & 15;

    return ((div < 8) ? cksys : cksys / ahb_psc_table[div]);
}

static uint32_t pclk1(uint32_t hclk)
{
    uint8_t div = (RCU_CFG0 >> 8) & 7;

    return ((div < 4) ? hclk : hclk / apb_psc_table[div]);
}

static uint32_t pclk2(uint32_t hclk)
{
    uint8_t div = (RCU_CFG0 >> 11) & 7;

    return ((div < 4) ? hclk : hclk / apb_psc_table[div]);
}

static uint32_t adcclk(uint32_t hclk)
{
    uint8_t div = (RCU_CFG0 >> 14) & 3;
    div |= (RCU_CFG2 & RCU_CFG2_ADCPSC2) >> 29;

    return hclk / adc_psc_table[div];
}

/**
 * @brief Enables ouput of system clock to pin
 *
 * @param src   clock to be output
 */
void CLOCK_MCOConfig(uint8_t src)
{
    if(!src){
        RCU_CFG0 = (RCU_CFG0 & ~RCU_CFG0_CKOUTSEL);
        gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
        return;
    }

    RCU_CFG0 = (RCU_CFG0 & ~RCU_CFG0_CKOUTSEL) | (src << 24);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_8);
}

/**
 * @brief
 * @param
 * @return
 */
uint32_t CLOCK_Get(enum clocknr clock)
{
    uint32_t ck_sys = cksys(CK_HXTAL);
    uint32_t hck = hclk(ck_sys);

    switch(clock){
        case CLOCK_CPU:  return hck;
        case CLOCK_CLK1: return pclk1(hck);
        case CLOCK_CLK2: return pclk2(hck);
        case CLOCK_CLK3: return adcclk(hck);
        default: return 0;
    }
}
