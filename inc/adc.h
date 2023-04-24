#ifndef __adc_h__
#define __adc_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    void *per;
    uint16_t *buf;
    uint32_t len;
    uint8_t ch;
    void (*eoc)(void);
}adctype_t;

/**
 * @brief 
 * 
 **/  
void ADC_Init(adctype_t *adc);


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

#ifdef __cplusplus
}
#endif

#endif
