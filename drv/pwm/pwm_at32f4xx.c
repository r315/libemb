#include "at32f4xx_rcc.h"
#include "pwm.h"
#include "clock.h"

#define PWM_MAX_FREQ    250000UL
#define TMR_CCM1_OC1MODE_PWM_A    0x60
/**
 * @brief Calculate timer period and prescaler for a given frequency.
 * @param tmr
 * @param frequency
 * @return timer ticks, 0 if the frequency is out of range.
 */
static uint32_t pwm_freq_cfg(TMR_Type *tmr, uint32_t frequency)
{
    uint32_t pclk;
    uint32_t prescaler, period;

    if(frequency > PWM_MAX_FREQ){
        return 0;
    }

    #ifdef USE_STDPERIPH_DRIVER
    RCC_ClockType clocks;
    RCC_GetClocksFreq(&clocks);
    #else
    sysclock_t clocks;
    CLOCK_GetAll(&clocks);
    #endif


    if(((uint32_t)tmr & APB2PERIPH_BASE) == APB2PERIPH_BASE){
        /* When TMR’s APB clock prescaler factor is 1, the CK_INT frequency is equal to that of APB, otherwise, it doubles the
            APB clock frequency. */
#ifdef USE_STDPERIPH_DRIVER
        pclk = (RCC->CFG & (4 << 11)) ? clocks.APB2CLK_Freq << 1 : clocks.APB2CLK_Freq;
    }else{
        pclk = (RCC->CFG & (4 << 8)) ? clocks.APB1CLK_Freq << 1 : clocks.APB1CLK_Freq;
#else
        pclk = (RCC->CFG & (4 << 11)) ? clocks.pclk2 << 1 : clocks.pclk2;
    }else{
        pclk = (RCC->CFG & (4 << 8)) ? clocks.pclk1 << 1 : clocks.pclk1;
#endif
    }

    // Calculate the total timer clock cycles needed for the given frequency
    uint32_t total_cycles = pclk / frequency;

     // Find a suitable prescaler and period within 16-bit limits
    prescaler = (total_cycles + 0xFFFF) / 0x10000; // Ceiling division
    if (prescaler > 0xFFFF) {
        return 0; // Cannot achieve the desired frequency
    }

    period = total_cycles / prescaler;
    if (period > 0xFFFF) {
        return 0; // Cannot achieve the desired frequency
    }

    tmr->DIV = prescaler - 1;
    tmr->AR = period - 1;

    return period;
}

/**
 * @brief Enable and configure a timer in pwm mode and
 * iinitializes pwmchip structure.
 *
 * @param chip
 * @return number of ticks in given frequency period
 */
uint32_t PWM_Init(pwmchip_t *pwmchip)
{
    TMR_Type *tmr;
    uint8_t nchannels;

    switch(pwmchip->chip){
        case 1:
            #ifdef USE_STDPERIPH_DRIVER
            RCC_APB2PeriphClockCmd(RCC_APB2EN_TMR1EN, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2RST_TMR1RST, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2RST_TMR1RST, DISABLE);
            #else
            RCC->APB2EN |= RCC_APB2EN_TMR1EN;
            RCC->APB2RST |= RCC_APB2RST_TMR1RST;
            RCC->APB2RST &= ~RCC_APB2RST_TMR1RST;
            #endif
            tmr = TMR1;
            nchannels = 4;
            tmr->BRKDT = TMR_BRKDT_MOEN;
            break;
        case 2:
            #ifdef USE_STDPERIPH_DRIVER
            RCC_APB1PeriphClockCmd(RCC_APB1EN_TMR2EN, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_TMR2RST, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_TMR2RST, DISABLE);
            #else
            RCC->APB1EN |= RCC_APB1EN_TMR2EN;
            RCC->APB1RST |= RCC_APB1RST_TMR2RST;
            RCC->APB1RST &= ~RCC_APB1RST_TMR2RST;
            #endif
            tmr = TMR2;
            nchannels = 4;
            break;
        case 3:
            #ifdef USE_STDPERIPH_DRIVER
            RCC_APB1PeriphClockCmd(RCC_APB1EN_TMR3EN, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_TMR2RST, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_TMR2RST, DISABLE);
            #else
            RCC->APB1EN |= RCC_APB1EN_TMR3EN;
            RCC->APB1RST |= RCC_APB1RST_TMR2RST;
            RCC->APB1RST &= ~RCC_APB1RST_TMR2RST;
            #endif
            tmr = TMR3;
            nchannels = 4;
            break;
        case 4:
            #ifdef USE_STDPERIPH_DRIVER
            RCC_APB1PeriphClockCmd(RCC_APB1EN_TMR4EN, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_TMR2RST, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_TMR2RST, DISABLE);
            #else
            RCC->APB1EN |= RCC_APB1EN_TMR4EN;
            RCC->APB1RST |= RCC_APB1RST_TMR2RST;
            RCC->APB1RST &= ~RCC_APB1RST_TMR2RST;
            #endif
            tmr = TMR4;
            nchannels = 4;
            break;
        case 5:
            #ifdef USE_STDPERIPH_DRIVER
            RCC_APB1PeriphClockCmd(RCC_APB1EN_TMR5EN, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_TMR2RST, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_TMR2RST, DISABLE);
            #else
            RCC->APB1EN |= RCC_APB1EN_TMR5EN;
            RCC->APB1RST |= RCC_APB1RST_TMR2RST;
            RCC->APB1RST &= ~RCC_APB1RST_TMR2RST;
            #endif
            tmr = TMR5;
            nchannels = 4;
            break;
        case 9:
            #ifdef USE_STDPERIPH_DRIVER
            RCC_APB2PeriphClockCmd(RCC_APB2EN_TMR9EN, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2RST_TMR9RST, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2RST_TMR9RST, DISABLE);
            #else
            RCC->APB2EN |= RCC_APB2EN_TMR9EN;
            RCC->APB2RST |= RCC_APB2RST_TMR9RST;
            RCC->APB2RST &= ~RCC_APB2RST_TMR9RST;
            #endif
            tmr = TMR9;
            nchannels = 2;
            break;
        case 10:
            #ifdef USE_STDPERIPH_DRIVER
            RCC_APB2PeriphClockCmd(RCC_APB2EN_TMR10EN, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2RST_TMR10RST, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2RST_TMR10RST, DISABLE);
            #else
            RCC->APB2EN |= RCC_APB2EN_TMR10EN;
            RCC->APB2RST |= RCC_APB2RST_TMR10RST;
            RCC->APB2RST &= ~RCC_APB2RST_TMR10RST;
            #endif
            tmr = TMR10;
            nchannels = 1;
            break;
        case 11:
            #ifdef USE_STDPERIPH_DRIVER
            RCC_APB2PeriphClockCmd(RCC_APB2EN_TMR11EN, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2RST_TMR11RST, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2RST_TMR11RST, DISABLE);
            #else
            RCC->APB2EN |= RCC_APB2EN_TMR11EN;
            RCC->APB2RST |= RCC_APB2RST_TMR11RST;
            RCC->APB2RST &= ~RCC_APB2RST_TMR11RST;
            #endif
            tmr = TMR11;
            nchannels = 1;
            break;
        default :
            return 0;
    }

    uint32_t ticks = pwm_freq_cfg(tmr, pwmchip->frequency);

    if(ticks){
        tmr->CTRL1 = TMR_CTRL1_CNTEN;
        pwmchip->nchannels = nchannels;
        pwmchip->handle = tmr;
    }

    return ticks;
}

/**
 * @brief Set pwm frequency, duty is preserved
 * @param chip
 * @param freq
 * @return number of ticks in given frequency period, 0 on failure
 */
uint32_t PWM_FrequencySet(pwmchip_t *pwmchip, uint32_t frequency)
{
    TMR_Type *tmr;
    uint8_t ch_duty[4];
    uint32_t ticks;

    if(!pwmchip){
        return 0;
    }

    /* Get channels duty */
    tmr = pwmchip->handle;
    ticks = tmr->AR + 1;

    for(uint8_t i = 0; i < pwmchip->nchannels; i++){
        uint32_t ccx = ((uint32_t*)&tmr->CC1)[i];
        ch_duty[i] = ccx ? ccx * 100 / ticks : 0;
    }

    /* Set new frequency */
    ticks = pwm_freq_cfg(pwmchip->handle, frequency);

    if(ticks){
        /* adjust compare channels according duty */
        for(uint8_t i = 0; i < pwmchip->nchannels; i++){
            ((uint32_t*)&tmr->CC1)[i] = ticks * ch_duty[i] / 100;
        }
        pwmchip->frequency = frequency;
    }

    return ticks;
}

/**
 * @brief Set the duty in % for the given pwm channel
 * */
void PWM_DutySet(pwmchip_t *pwmchip, uint8_t channel, uint8_t duty)
{
    TMR_Type *tmr;
    uint32_t match;

    if(!pwmchip || channel > pwmchip->nchannels){
        return;
    }

    if(duty > 100)
        duty = 100;

    tmr = pwmchip->handle;
    match = tmr->AR;

    match *= duty;
    match /= 100;

    ((uint32_t*)&tmr->CC1)[channel - 1] = match;
}

/**
 * @brief Get the duty in % of given pwm channel
 * @param pwmchip
 * @param channel
 * @return configured value
 */
uint8_t PWM_DutyGet(pwmchip_t *pwmchip, uint8_t channel)
{
    TMR_Type *tmr = pwmchip->handle;
    uint32_t ticks, ccx;

    if(!pwmchip || channel > pwmchip->nchannels){
        return 0;
    }

    ticks = tmr->AR;
    ccx = ((uint32_t*)&tmr->CC1)[channel - 1];

    return ccx * 100 / ticks;
}

/**
 * @brief
 * @param chip
 * @param time
 * @return configured period
 */
uint32_t PWM_PeriodSet(pwmchip_t *pwmchip, uint32_t period)
{
    TMR_Type *tmr;

    if(!pwmchip || period > 0x0000FFFF){
        return 0;
    }

    tmr = pwmchip->handle;
    tmr->AR = period;

    return period;
}

/**
 * @brief
 * @param chip
 * @return
 */
uint32_t PWM_PeriodGet(pwmchip_t *pwmchip)
{
    if(!pwmchip){
        return 0;
    }
    return ((TMR_Type*)pwmchip->handle)->AR;
}

/**
 * @brief Set pwm pulse in ticks
 * @param chip
 * @param ch
 * @return pulse timer ticks
 */
uint32_t PWM_PulseSet(pwmchip_t *pwmchip, uint8_t channel, uint32_t pulse)
{
    if(!pwmchip || channel > pwmchip->nchannels || pulse > 0xFFFF){
        return 0;
    }

    TMR_Type *tmr = pwmchip->handle;

    ((uint32_t*)&tmr->CC1)[channel - 1] = pulse;

    return pulse;
}

/**
 * @brief Get pwm pulse in ticks
 * @param chip
 * @param ch
 * @return channel pulse ticks
 */
uint32_t PWM_PulseGet(pwmchip_t *pwmchip, uint8_t channel)
{
    if(!pwmchip){
        return 0;
    }

    TMR_Type *tmr = pwmchip->handle;

    return ((uint32_t*)&tmr->CC1)[channel - 1];
}

/**
 * @brief
 * @param chip
 * @param ch
 * @param
 */
void PWM_Enable(pwmchip_t *pwmchip, uint8_t channel, enum pwmpstate state)
{
    if(!pwmchip || channel > pwmchip->nchannels){
        return;
    }

    TMR_Type *tmr = pwmchip->handle;
    volatile uint16_t *ccmx = (channel > 2) ? &tmr->CCM2 : &tmr->CCM1;
    uint16_t cce_mask = TMR_CCE_C1EN  << ((channel - 1) << 2);
    uint8_t ccmx_shift = (((channel - 1) & 1) << 3);

    switch(state){
        case PWM_PIN_OFF:
            *ccmx = *ccmx & ~(TMR_CCM1_OC1MODE << ccmx_shift);
            tmr->CCE = tmr->CCE & ~cce_mask;
            break;
        case PWM_PIN_ON:
            *ccmx = (*ccmx & ~(TMR_CCM1_OC1MODE << ccmx_shift)) | (TMR_CCM1_OC1MODE_PWM_A << ccmx_shift);
            tmr->CCE = tmr->CCE | cce_mask;
            break;
            
        case PWM_PIN_LOW: break;
        case PWM_PIN_HIGH: break;
        default: break;
    }
}

/**
 * @brief Set channel polarity
 *
 * \param ch    : PWM channel
 * \param pol   : Polarity 0 - 1
 * */
void PWM_Polarity(pwmchip_t *pwmchip, uint8_t channel, uint8_t pol)
{
    TMR_Type *tmr = pwmchip->handle;

    if(!pwmchip || channel > pwmchip->nchannels){
        return;
    }

    uint8_t shift = (channel - 1) << 2;
    tmr->CCE = pol ? tmr->CCE | (TMR_CCE_C1P << shift) : tmr->CCE & ~(TMR_CCE_C1P << shift);
}
