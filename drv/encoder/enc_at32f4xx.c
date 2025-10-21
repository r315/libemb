#include <stdint.h>
#include "at32f4xx.h"
#include "encoder.h"
#include "gpio_at32f4xx.h"

uint32_t ENC_Init(enctype_t *enc, uint32_t cfg)
{
    TMR_Type *tmr;
    switch(cfg){
        case 0:
            RCC->APB1EN |= RCC_APB1EN_TMR2EN | RCC_APB2EN_AFIOEN;
            // Partial remap for TIM2, PA15 -> CH1, PB3 -> CH2
            AFIO->MAP = (AFIO->MAP & ~(3 << 8)) | (1 << 8);
            GPIO_Config(PB_3, GPI_PU);
            GPIO_Config(PA_15, GPI_PU);
            tmr = TMR2;
            break;

        default: return;
    }

    tmr->CTRL1 = 0;
    tmr->CTRL2 = 0;
    tmr->SMC = TMR_SMC_SMSEL_1;                         // Encoder mode 2, count only on TI1 edges
    tmr->CCM1 = (15 << 12) | (15 << 4) |                // Max length if input filter on TI1, TI2
                    (1 << 8) | (1 << 0);                // Map TIxFP1 to TIx,
    tmr->CCE = 0;                                       // Falling polarity
    tmr->STS = 0;
    tmr->CTRL1 = TMR_CTRL1_CNTEN;
}

/**
 * @brief Retuns the absolute counter value
 * @param enc
 * @return
 */
uint32_t ENC_CountGet(enctype_t *enc)
{
    return ((TMR_Type *)enc->handle)->CNT;
}

/**
 * @brief Calculates how much the encoder has moved since last call
 * @param enc
 * @return amount changed
 */
int32_t ENC_IncrementGet(enctype_t *enc)
{
    int16_t diff = ((TMR_Type *)enc->handle)->CNT - enc->count;
    enc->count += diff;
    return diff;
}

/**
 * @brief Update value in enctype structure
 *
 * @param enc
 * @return Amount changed in value
 */
int32_t ENC_Update(enctype_t *enc)
{
    int16_t diff = ENC_IncrementGet(enc);
    if(diff != 0){
        uint32_t tmp = enc->value;
        tmp += diff * 10;                       // increment/decrement, TODO: acceleration
        if(tmp > enc->max){ tmp = enc->max; }
        if(tmp < enc->min){ tmp = enc->min; }
        enc->value = tmp;
        //DBG_APP_PRINTF("\rEnc %s%d : %d ", diff > 0 ? "+" : "", diff, tmp);
    }
    return diff;
}