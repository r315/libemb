#ifndef _pwm_h_
#define _pwm_h_

#include <stdint.h>

#define PWM_1       1
#define PWM_2       2
#define PWM_3       3
#define PWM_4       4
#define PWM_5       5
#define PWM_6       6

/**
 * @brief initialyze PWM with given frequency
 * \param tcclk [in] : PWM frequency
 * */
void PWM_Init(uint32_t tcclk);

/**
 * @brief Changes PWM frequency, duty of enabled channels
 * is adjusted
 * 
 * \param freq [in] : New pwm frequency
 * */
void PWM_Freq(uint32_t freq);

/**
 * @brief set the duty in % for the given pwm channel
 * */
void PWM_Set(uint8_t channel, uint8_t duty);

/**
 * @brief get the duty in % of given pwm channel
 * */
uint8_t PWM_Get(uint8_t channel);

/**
 * @brief Enable one PWM channel range(1:4)
 **/
 void PWM_Enable(uint8_t ch);
 
 /**
 * @brief Disable one PWM channel range(1:4)
 **/
 void PWM_Disable(uint8_t ch);
 
#endif
