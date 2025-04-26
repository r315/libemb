#include "at32f4xx.h"
#include "adc.h"
#include "adc_at32f4xx.h"


static void adc_calibration_at32f4xx(ADC_Type* ADCx)
{
    /* Enable ADC1 */
    ADCx->CTRL2 |= ADC_CTRL2_ADON;
    /* Enable ADC1 reset calibration register */
    ADCx->CTRL2 |= ADC_CTRL2_RSTCAL;
    /* Check the end of ADC1 reset calibration register */
    while(ADCx->CTRL2 & ADC_CTRL2_RSTCAL);
    /* Start ADC1 calibration */
    ADCx->CTRL2 |= ADC_CTRL2_CAL;
    /* Check the end of ADC1 calibration */
    while(ADCx->CTRL2 & ADC_CTRL2_CAL);
}

/**
  * @brief  set ordinary channel's corresponding rank in the sequencer and sample time of the specified adc peripheral.
  * @param  adc_x: select the adc peripheral.
  *         this parameter can be one of the following values:
  *         ADC1.
  * @param  adc_channel: select the channel.
  *         this parameter can be one of the following values:
  *         - ADC_CHANNEL_0    - ADC_CHANNEL_1    - ADC_CHANNEL_2    - ADC_CHANNEL_3
  *         - ADC_CHANNEL_4    - ADC_CHANNEL_5    - ADC_CHANNEL_6    - ADC_CHANNEL_7
  *         - ADC_CHANNEL_8    - ADC_CHANNEL_9    - ADC_CHANNEL_10   - ADC_CHANNEL_11
  *         - ADC_CHANNEL_12   - ADC_CHANNEL_13   - ADC_CHANNEL_14   - ADC_CHANNEL_15
  *         - ADC_CHANNEL_16   - ADC_CHANNEL_17
  * @param  adc_sequence: set rank in the ordinary group sequencer.
  *         this parameter must be:
  *         - between 1 to 16
  * @param  adc_sampletime: set the sampletime of adc channel.
  *         this parameter can be one of the following values:
  *         - ADC_SAMPLETIME_1_5
  *         - ADC_SAMPLETIME_7_5
  *         - ADC_SAMPLETIME_13_5
  *         - ADC_SAMPLETIME_28_5
  *         - ADC_SAMPLETIME_41_5
  *         - ADC_SAMPLETIME_55_5
  *         - ADC_SAMPLETIME_71_5
  *         - ADC_SAMPLETIME_239_5
  * @retval none
  */
 void adc_ordinary_channel_set(ADC_Type *adc_x, adc_channel_select_type adc_channel, uint8_t adc_sequence, adc_sampletime_select_type adc_sampletime)
 {
    uint32_t tmp_reg;
    if(adc_channel < ADC_CHANNEL_10)
    {
        tmp_reg = adc_x->SMPT2;
        tmp_reg &= ~(0x07 << 3 * adc_channel);
        tmp_reg |= adc_sampletime << 3 * adc_channel;
        adc_x->SMPT2 = tmp_reg;
    }
    else
    {
        tmp_reg = adc_x->SMPT1;
        tmp_reg &= ~(0x07 << 3 * (adc_channel - ADC_CHANNEL_10));
        tmp_reg |= adc_sampletime << 3 * (adc_channel - ADC_CHANNEL_10);
        adc_x->SMPT1 = tmp_reg;
    }

    if(adc_sequence >= 13)
    {
        tmp_reg = adc_x->RSQ1;
        tmp_reg &= ~(0x01F << 5 * (adc_sequence - 13));
        tmp_reg |= adc_channel << 5 * (adc_sequence - 13);
        adc_x->RSQ1 = tmp_reg;
    }
    else if(adc_sequence >= 7)
    {
        tmp_reg = adc_x->RSQ2;
        tmp_reg &= ~(0x01F << 5 * (adc_sequence - 7));
        tmp_reg |= adc_channel << 5 * (adc_sequence - 7);
        adc_x->RSQ2 = tmp_reg;
    }
    else
    {
        tmp_reg = adc_x->RSQ3;
        tmp_reg &= ~(0x01F << 5 * (adc_sequence - 1));
        tmp_reg |= adc_channel << 5 * (adc_sequence - 1);
        adc_x->RSQ3 = tmp_reg;
    }
}

/**
  * @brief  enable or disable the ordinary channel's external trigger and
  *         set external trigger event of the specified adc peripheral.
  * @param  adc_x: select the adc peripheral.
  *         this parameter can be one of the following values:
  *         ADC1.
  * @param  adc_ordinary_trig: select the external trigger event.
  *         this parameter can be one of the following values:
  *         adc1
  *         - ADC12_ORDINARY_TRIG_TMR1CH1     - ADC12_ORDINARY_TRIG_TMR1CH2  - ADC12_ORDINARY_TRIG_TMR1CH3             - ADC12_ORDINARY_TRIG_TMR2CH2
  *         - ADC12_ORDINARY_TRIG_TMR3TRGOUT  - ADC12_ORDINARY_TRIG_TMR4CH4  - ADC12_ORDINARY_TRIG_EXINT11_TMR1TRGOUT  - ADC12_ORDINARY_TRIG_SOFTWARE
  *         - ADC12_ORDINARY_TRIG_TMR1TRGOUT
  * @param  new_state: new state of ordinary channel's external trigger.
  *         this parameter can be: TRUE or FALSE.
  * @retval none
  */
void adc_ordinary_conversion_trigger_set(ADC_Type *adc_x, adc_ordinary_trig_select_type adc_ordinary_trig, uint8_t new_state)
{
    adc_x->CTRL2 &= ~(ADC_CTRL2_EXSEL | ADC_CTRL2_EXTREN);

    if(adc_ordinary_trig > 7){
        adc_x->CTRL2 |= ADC_CTRL2_EXSEL_3;
    }

    adc_x->CTRL2 |= ((adc_ordinary_trig & 0x7) << 17) | (new_state << 20);
}

/**
  * @brief  enable or disable ordinary software start conversion of the specified adc peripheral.
  * @param  adc_x: select the adc peripheral.
  *         this parameter can be one of the following values:
  *         ADC1.
  * @param  new_state: new state of ordinary software start conversion.
  *         this parameter can be: TRUE or FALSE.
  * @retval none
  */
 void adc_ordinary_software_trigger_enable(ADC_Type *adc_x, uint8_t new_state)
 {
    adc_x->CTRL2 |= new_state << 22;
 }

/**
  * @brief  adc base default para init.
  * @param  sequence_mode: set the state of adc sequence mode.
  *         this parameter can be:TRUE or FALSE
  * @param  repeat_mode: set the state of adc repeat conversion mode.
  *         this parameter can be:TRUE or FALSE
  * @param  data_align: set the state of adc data alignment.
  *         this parameter can be one of the following values:
  *         - ADC_RIGHT_ALIGNMENT
  *         - ADC_LEFT_ALIGNMENT
  * @param  ordinary_channel_length: configure the adc ordinary channel sequence length.
  *         this parameter can be:
  *         - (0x1~0x10)
  * @retval none
  */
void adc_base_config(ADC_Type *adcx, adc_base_config_type *adc_base_config)
{
    adcx->CTRL1 = (adcx->CTRL1 & ~ADC_CTRL1_SCN) | (adc_base_config->sequence_mode << 8);
    adcx->CTRL2 = (adcx->CTRL2 & ~(ADC_CTRL2_CON | ADC_CTRL2_DALIGN)) |
                  (adc_base_config->repeat_mode << 1) | (adc_base_config->data_align << 11);
    adcx->RSQ1 = (adcx->CTRL1 & ~ADC_RSQ1_LEN) | ((adc_base_config->ordinary_channel_length - 1) << 20);
}

/**
  * @brief  return the last conversion data for ordinary channel of the specified adc peripheral.
  * @param  adc_x: select the adc peripheral.
  *         this parameter can be one of the following values:
  *         ADC1.
  * @retval the last conversion data for ordinary channel.
  */
 uint16_t adc_ordinary_conversion_data_get(ADC_Type *adc_x)
 {
    return (uint16_t)adc_x->RDOR;
 }


void ADC_Init(adctype_t *adc)
{
    if(!adc->per){
        return;
    }

    RCC->APB2EN |= RCC_APB2EN_ADC1EN;
    RCC->APB2RST |= RCC_APB2RST_ADC1RST;
    RCC->APB2RST &= ~RCC_APB2RST_ADC1RST;

    RCC->CFG = (RCC->CFG & ~RCC_CFG_ADCPSC) | RCC_CFG_ADCPSC_DIV8;

    adc_base_config(ADC1, adc->per);

    adc_calibration_at32f4xx(ADC1);
}

uint16_t ADC_ConvertSingle(adctype_t *adc)
{
    adc_ordinary_software_trigger_enable(ADC1, 1);
    return adc_ordinary_conversion_data_get(ADC1);
}
