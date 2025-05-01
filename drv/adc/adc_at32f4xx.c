#include "at32f4xx.h"
#include "adc.h"
#include "adc_at32f4xx.h"


static void adc_calibration(ADC_Type *ADCx)
{
    /* Enable ADC1 */
    ADCx->CTRL2 |= ADC_CTRL2_ADON;
    /* Enable ADC1 reset calibration register */
    ADCx->CTRL2 |= ADC_CTRL2_RSTCAL;
    /* Check the end of ADC1 reset calibration register */
    while (ADCx->CTRL2 & ADC_CTRL2_RSTCAL)
        ;
    /* Start ADC1 calibration */
    ADCx->CTRL2 |= ADC_CTRL2_CAL;
    /* Check the end of ADC1 calibration */
    while (ADCx->CTRL2 & ADC_CTRL2_CAL)
        ;
}

/**
 * @brief  Initializes ADC in sequence mode.
 * Because of the used of preemptive channels,
 * only 4 channels are available for conversion.
 * Using the preemptive channels avoids DMA since
 * each channel has his own data register.
 */
uint8_t ADC_Init(adctype_t *adc)
{
    RCC->APB2EN |= RCC_APB2EN_ADC1EN;
    RCC->APB2RST |= RCC_APB2RST_ADC1RST;
    RCC->APB2RST &= ~RCC_APB2RST_ADC1RST;
    // clk = APB2 / 8
    RCC->CFG = (RCC->CFG & ~RCC_CFG_ADCPSC) | RCC_CFG_ADCPSC_DIV8;

    ADC1->CTRL1 = ADC_CTRL1_SCN; // Enable sequence mode
    ADC1->CTRL2 = (ADC12_PREEMPT_TRIG_SOFTWARE << 12) |
                  ADC_CTRL2_JEXTREN; // Enable preemptive trigger
                                     // align right, no repeat

    ADC1->JSQ = 0; // Using preemptive channels.
                   // One channel on sequence
                   // if not set it is channel 0

    adc->per = ADC1;

    adc_calibration(ADC1);

    return 1;
}

/**
 * @brief  start the conversion of the specified adc peripheral.
 *         ADC_ChannelLenSet() must be called before this function
 *         with the number of channels to convert.
 * @param  adc_x: select the adc peripheral.
 * @param ch: channel number 0~17
 * @param seq: sequence number 1~4
 * @params sampletime: sample time
 *         this parameter can be one of the following values:
 *     - ADC_SAMPLETIME_1_5
 *     - ADC_SAMPLETIME_7_5
 *     - ADC_SAMPLETIME_13_5
 *     - ADC_SAMPLETIME_28_5
 *     - ADC_SAMPLETIME_41_5
 *     - ADC_SAMPLETIME_55_5
 *     - ADC_SAMPLETIME_71_5
 *     - ADC_SAMPLETIME_239_5
 * @retval none
 */
void ADC_ChannelSet(adctype_t *adc, uint8_t ch, uint8_t seq, uint8_t sampletime)
{
    uint8_t len = (ADC1->JSQ & ADC_JSQ_JLEN) >> 20;
    uint32_t tmp_reg;
    uint8_t sequence_index;

    if (ch < ADC_CHANNEL_10) {
        tmp_reg = ADC1->SMPT2;
        tmp_reg &= ~(0x07 << 3 * ch);
        tmp_reg |= sampletime << 3 * ch;
        ADC1->SMPT2 = tmp_reg;
    }
    else
    {
        tmp_reg = ADC1->SMPT1;
        tmp_reg &= ~(0x07 << 3 * (ch - ADC_CHANNEL_10));
        tmp_reg |= sampletime << 3 * (ch - ADC_CHANNEL_10);
        ADC1->SMPT1 = tmp_reg;
    }

    sequence_index = seq + 2 - len;

    uint32_t mask = (0x1f << (5 * sequence_index));

    ADC1->JSQ = (ADC1->JSQ & ~mask) | (ch << (5 * sequence_index));
}

/**
 * @brief  set the length of the conversion sequence
 * @param  adc
 * @param  new_lenght: length of the conversion sequence
 *         this parameter can be:
 *         - (0x1~0x4)
 */
void ADC_ChannelLenSet(adctype_t *adc, uint8_t new_lenght)
{
    ADC1->JSQ = (ADC1->JSQ & ~ADC_JSQ_JLEN) | ((new_lenght - 1) << 20);
}

/**
 * @brief  Initiates a conversion sequence of preemptive channels.
 * @param  adc_x: select the adc peripheral.
 *         this parameter can be one of the following values:
 *         ADC1.
 * @param  wait: wait for conversion to finish
 * @retval none
 */
uint8_t ADC_Acquire(adctype_t *adc, uint8_t wait)
{
    ADC1->STS = 0;

    ADC1->CTRL2 |= ADC_CTRL2_JSWSTR;

    if (wait) {
        uint32_t timeout = 0xFFFF;
        while (!(ADC1->STS & ADC_STS_JEC) && timeout) {
            timeout--;
        }
        if (!timeout) {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief  Get the data of the last acquisition for a given channel
 * @param  adc_x: select the adc peripheral.
 *         this parameter can be one of the following values:
 *         ADC1.
 * @param  ch: channel number
 * @retval none
 */
uint16_t ADC_ChannelDataGet(adctype_t *adc, uint8_t ch)
{
    uint8_t len = (ADC1->JSQ & ADC_JSQ_JLEN) >> 20;
    uint32_t reg_data = ADC1->JSQ;
    uint32_t *pdt = (uint32_t *)&ADC1->JDOR1;

    if (ch > 17) {
        return 0;
    }
    // Check if the channel is in the preemptive sequence
    reg_data >>= ((3 - len) * 5);

    do {
        if ((reg_data & 0x1f) == ch) {
            // If the channel is in the sequence, return the data
            return *pdt;
        }
        reg_data >>= 5;
        pdt++;
    } while (len--);

    return 0;
}
