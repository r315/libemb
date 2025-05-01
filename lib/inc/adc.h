#ifndef __adc_h__
#define __adc_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// TODO: This API needs to be improved.

typedef struct {
    void *per;          // ADC peripheral
    uint16_t *buf;      // Buffer for DMA
    void (*eoc)(void);  // End of conversion callback
}adctype_t;

/**
 * @brief
 *
 **/
uint8_t ADC_Init(adctype_t *adc);

/**
 * @brief Start of convertion or multiple convertions
 * which will ended in callback
 **/
void ADC_Start(adctype_t *adc);

/**
 * @brief
 *
 * @param adc
 */
void ADC_Stop(adctype_t *adc);

/**
 * @brief Get single convertion on current selected channel
 **/
uint16_t ADC_ConvertSingle(adctype_t *adc);

/**
 * @brief Convert multiple channels
 **/
uint8_t ADC_Acquire(adctype_t *adc, uint8_t wait);


/**
 * @brief Get data of last acquisition for a given channel
 *
 */
uint16_t ADC_ChannelDataGet(adctype_t *adc, uint8_t ch);

/**
 * @brief Set the channel for conversion
 *
 * @param adc
 * @param ch
 * @param seq
 * @param sampletime
 */
void ADC_ChannelSet(adctype_t *adc, uint8_t ch, uint8_t seq, uint8_t sampletime);

/**
 * @brief Set the length of the conversion sequence
 *
 * @param adc
 * @param new_lenght
 */
void ADC_ChannelLenSet(adctype_t *adc, uint8_t new_lenght);

#ifdef __cplusplus
}
#endif

#endif
