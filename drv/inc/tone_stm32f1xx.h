#ifndef _tone_stm32f1xx_h_
#define _tone_stm32f1xx_h_

#include <stdint.h>
#include "stm32f1xx.h"

typedef struct tone_init {
    TIM_TypeDef *tim;   // Timer instance to be configured
    uint8_t ch;         // Compare channel nr - 1
    uint8_t pin;        // Pin name
    uint8_t pin_idle;   // Pin idle state
    uint8_t volume;     // Initial volume
}tone_pwm_init_t;

uint32_t TONE_PwmInit(tone_pwm_init_t *init);

#endif /* _tone_stm32f1xx_h_ */
