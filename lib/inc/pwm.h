#ifndef _pwm_h_
#define _pwm_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define PWM_1       1
#define PWM_2       2
#define PWM_3       3
#define PWM_4       4
#define PWM_5       5
#define PWM_6       6

enum pwmpstate {
    PWM_PIN_OFF,
    PWM_PIN_ON,
    PWM_PIN_LOW,
    PWM_PIN_HIGH,
};

typedef struct pwmchip{
    void *handle;           // Internal driver handler
    uint32_t frequency;     // Current PWM frequency
    uint8_t chip;           // PWM chip number, obtained from board
    uint8_t nchannels;      // Number of channels on chip, set by driver
}pwmchip_t;

/**
 * @brief
 * @param pwmchip
 * @return 1: Initialized, 0: failed
 */
uint32_t PWM_Init(pwmchip_t *pwmchip);

/**
 * @brief Set PWM frequency
 * @param pwmchip
 * @param freq
 * @return 1: Frequency configured, 0: failure
 */
uint32_t PWM_FrequencySet(pwmchip_t *pwmchip, uint32_t freq);

/**
 * @brief Changes pwm period
 * @param chip
 * @param period in ticks
 * @return 0 on failure
 */
uint32_t PWM_PeriodSet(pwmchip_t *pwmchip, uint32_t period);

/**
 * @brief
 * @param chip
 * @return perioud in ticks
 */
uint32_t PWM_PeriodGet(pwmchip_t *pwmchip);

/**
 * @brief Set the duty in % for the given pwm channel
 * */
void PWM_DutySet(pwmchip_t *pwmchip, uint8_t ch, uint8_t duty);

/**
 * @brief Get the duty in % of given pwm channel
 * */
uint8_t PWM_DutyGet(pwmchip_t *pwmchip, uint8_t ch);

/**
 * @brief Set pwm pulse in ticks
 * @param chip
 * @param ch
 * @return 0 on failure
 */
uint32_t PWM_PulseSet(pwmchip_t *pwmchip, uint8_t ch, uint32_t pulse);

/**
 * @brief Get pwm pulse in ticks
 * @param chip
 * @param ch
 * @return pulse ticks
 */
uint32_t PWM_PulseGet(pwmchip_t *pwmchip, uint8_t ch);

/**
 * @brief Enable PWM channel
 * */
void PWM_Enable(pwmchip_t *pwmchip, uint8_t ch, enum pwmpstate state);

/**
 * @brief Set channel polarity
 *
 * \param ch    : PWM channel
 * \param pol   : Polarity 0 - 1
 * */
void PWM_Polarity(pwmchip_t *pwmchip, uint8_t ch, uint8_t pol);


#ifdef __cplusplus
}
#endif

#endif
