#ifndef _tone_stm32f1xx_h_
#define _tone_stm32f1xx_h_

#include <stdint.h>
#include "stm32f1xx.h"

typedef struct tone_init {
    TIM_TypeDef *tim;
    uint8_t tim_ch;
    uint8_t pin;
    uint8_t pin_idle;
    uint8_t dma_req;
}tone_pwm_init_t;

uint32_t TONE_PwmInit(tone_pwm_init_t *init);

#endif /* _tone_stm32f1xx_h_ */
