#include <stdint.h>
#include "board.h"
#include "pwm.h"

#ifndef PWM_TIM
#define PWM_TIM     TIM1
#endif

#define PWM_MAX_CH                  4
#define TIM_CCMR1_OC1M_PWM1         (6U << TIM_CCMR1_OC1M_Pos)
#define TIM_CCMR1_OC2M_PWM1         (6U << TIM_CCMR1_OC2M_Pos)
#define TIM_CCMR2_OC3M_PWM1         (6U << TIM_CCMR2_OC3M_Pos)
#define TIM_CCMR2_OC4M_PWM1         (6U << TIM_CCMR2_OC4M_Pos)

#define DISABLE                     0
#define ENABLE                      1

/**
 * PA8 (Pin D9)    ------> TIM1_CH1
 * PA9 (Pin D1)    ------> TIM1_CH2
 * PA10 (Pin D0)   ------> TIM1_CH3
 * PA11 (Pin D10)  ------> TIM1_CH4
 * */
static void PWM_CfgGpio(uint16_t pin, uint8_t enable){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();

    if(enable){
        GPIO_InitStruct.Pin = (1 << pin);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }else{
        HAL_GPIO_DeInit(GPIOA, (1 << pin));
    }
}

/**
 * @brief initalise PWM timer and configure GPIO pins
 * 
 * \param period [in] : PWM Period in us
 * */
void PWM_Init(uint32_t period){
    __HAL_RCC_TIM1_CLK_ENABLE();

    __HAL_RCC_TIM1_FORCE_RESET();
    __HAL_RCC_TIM1_RELEASE_RESET();

    PWM_TIM->PSC = (SystemCoreClock / 1000000) - 1; // Count us
    PWM_TIM->ARR = period;
    PWM_TIM->CCMR1 =                                // Configure all channels as output compare (PWM) and enable preload
        TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC1PE |
        TIM_CCMR1_OC2M_PWM1 | TIM_CCMR1_OC2PE;
    PWM_TIM->CCMR2 =
        TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE |
        TIM_CCMR2_OC4M_PWM1 | TIM_CCMR2_OC4PE;

    PWM_TIM->BDTR = TIM_BDTR_MOE | TIM_BDTR_BKP | TIM_BDTR_BK2P;

    PWM_TIM->CR1 = TIM_CR1_CEN;
}

/**
 * @brief Changes PWM frequency, duty of enabled channels
 * is adjusted
 * 
 * \param period [in] : New pwm frequency
 * */
void PWM_Freq(uint32_t period){
    PWM_TIM->ARR = period;
    PWM_TIM->EGR |= TIM_EGR_UG;
}

/**
 * @brief Set duty cycle for given channel
 * 
 * \param channel [in] : Channel to be configured
 *                       PWM_1 ... PWM_6
 * \param duty [in] :    Duty cycle 0 - 100
 * */
void PWM_Set(uint8_t channel, uint8_t duty){
    if(channel > PWM_MAX_CH)
        return;
    
    if(duty > 100)
        duty = 100;

    uint32_t match = PWM_TIM->ARR;
    match *= duty;
    match /= 100;

    uint32_t *ccr = (uint32_t*)&PWM_TIM->CCR1;
    
    ccr[channel - 1] = match;
}

/**
 * @brief get the duty in % of given pwm channel
 * 
 * \param channel [in] : Channel to get value
 * \retval : pwm duty 0 - 100
 * */
uint8_t PWM_Get(uint8_t channel){  
    if(channel > PWM_MAX_CH)
        return 0;

    uint32_t *ccr = (uint32_t*)&PWM_TIM->CCR1;
    
    uint32_t match = ccr[channel - 1] * 100;    
    return match / PWM_TIM->ARR;
}

/**
 * @brief Enables PWM channel and configure GPIO pin to PWM function
 * 
 * \param ch [in] Channel to be configured
 *                  PWM_1
 *                  ---
 *                  PWM_6
 * 
 * PWM pins start on PA8
 * */
void PWM_Enable(uint8_t channel){
    if(channel > PWM_MAX_CH)
        return;

    uint8_t shift = (channel - 1) << 2;
    PWM_TIM->CCER |= (TIM_CCER_CC1E << shift);

    PWM_CfgGpio(7 + channel, ENABLE);
}

/**
 * @brief Disables PWM channel and configure GPIO pin to default
 * 
 * \param ch [in] Channel to be configured
 *                 PWM_1 ... PWM_6
 * */
void PWM_Disable(uint8_t channel){
    if(channel > PWM_MAX_CH)
        return;

    uint8_t shift = (channel - 1) << 2;
    PWM_TIM->CCER &= ~(TIM_CCER_CC1E << shift);
    PWM_CfgGpio(7 + channel, DISABLE);
}
