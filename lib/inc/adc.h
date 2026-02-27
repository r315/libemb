#ifndef __adc_h__
#define __adc_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    void *handle;           // ADC internal structure
    void (*eoc)(uint16_t *, uint16_t);      // End of conversion callback
}adctype_t;

/**
 * @brief Powerup and initializes ADC peripheral
 *
 * @param adc pointer to handle structure
 * @return Internal reference voltage value
 */
uint16_t ADC_Init(adctype_t *adc);

/**
 * @brief Starts acquiring data from a sequence of channels
 * and calling end of conversion callback with converted data.
 **/
void ADC_StartAcquisition(adctype_t *adc);

/**
 * @brief Stops a running acquisition
 *
 * @param adc
 */
void ADC_StopAcquisition(adctype_t *adc);

/**
 * @brief Do a single conversion on a channel
 **/
uint16_t ADC_ConvertSingle(adctype_t *adc, uint8_t ch);

/**
 * @brief Single conversion on a sequence of channels
 *
 * @param wait 0: Returns immediately, otherwise waits for end of conversion
 **/
uint8_t ADC_Convert(adctype_t *adc, uint8_t wait);

/**
 * @brief Get data of a given channel obtained from last
 * converted sequence
 *
 */
uint16_t ADC_ConvertedDataGet(adctype_t *adc, uint8_t ch);

/**
 * @brief Configure a channel to make part of a conversion sequence
 *
 * @param adc           ADC handle
 * @param ch            Channel number to be converted
 * @param seq           Sequence index for this channel
 * @param sampletime    Sample time for this channel
 */
void ADC_ConfigChannel(adctype_t *adc, uint8_t ch, uint8_t seq, uint8_t sampletime);

/**
 * @brief Configures a sequence acquisition stream
 *
 * @param adc
 * @param freq
 */
void ADC_ConfigAcquisition(adctype_t *adct, uint32_t freq, uint16_t *samples, uint16_t size);

/**
 * @brief Same as config channel, but for acquisition sequence
 * @param adct
 * @param ch
 * @param seq_idx
 * @param sampletime
 */
void ADC_ConfigAcquisitionChannel(adctype_t *adct, uint8_t ch, uint8_t seq_idx, uint8_t sampletime);

#ifdef __cplusplus
}
#endif

#endif
