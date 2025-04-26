#ifndef ADC_AT32F4XX_H
#define ADC_AT32F4XX_H

typedef enum
{
  ADC_RIGHT_ALIGNMENT                    = 0x00, /*!< data right alignment */
  ADC_LEFT_ALIGNMENT                     = 0x01  /*!< data left alignment */
} adc_data_align_type;

typedef enum
{
  ADC_CHANNEL_0                          = 0x00, /*!< adc channel 0 */
  ADC_CHANNEL_1                          = 0x01, /*!< adc channel 1 */
  ADC_CHANNEL_2                          = 0x02, /*!< adc channel 2 */
  ADC_CHANNEL_3                          = 0x03, /*!< adc channel 3 */
  ADC_CHANNEL_4                          = 0x04, /*!< adc channel 4 */
  ADC_CHANNEL_5                          = 0x05, /*!< adc channel 5 */
  ADC_CHANNEL_6                          = 0x06, /*!< adc channel 6 */
  ADC_CHANNEL_7                          = 0x07, /*!< adc channel 7 */
  ADC_CHANNEL_8                          = 0x08, /*!< adc channel 8 */
  ADC_CHANNEL_9                          = 0x09, /*!< adc channel 9 */
  ADC_CHANNEL_10                         = 0x0A, /*!< adc channel 10 */
  ADC_CHANNEL_11                         = 0x0B, /*!< adc channel 11 */
  ADC_CHANNEL_12                         = 0x0C, /*!< adc channel 12 */
  ADC_CHANNEL_13                         = 0x0D, /*!< adc channel 13 */
  ADC_CHANNEL_14                         = 0x0E, /*!< adc channel 14 */
  ADC_CHANNEL_15                         = 0x0F, /*!< adc channel 15 */
  ADC_CHANNEL_16                         = 0x10, /*!< adc channel 16 */
  ADC_CHANNEL_17                         = 0x11  /*!< adc channel 17 */
} adc_channel_select_type;

typedef enum
{
  ADC_SAMPLETIME_1_5                     = 0x00, /*!< adc sample time 1.5 cycle */
  ADC_SAMPLETIME_7_5                     = 0x01, /*!< adc sample time 7.5 cycle */
  ADC_SAMPLETIME_13_5                    = 0x02, /*!< adc sample time 13.5 cycle */
  ADC_SAMPLETIME_28_5                    = 0x03, /*!< adc sample time 28.5 cycle */
  ADC_SAMPLETIME_41_5                    = 0x04, /*!< adc sample time 41.5 cycle */
  ADC_SAMPLETIME_55_5                    = 0x05, /*!< adc sample time 55.5 cycle */
  ADC_SAMPLETIME_71_5                    = 0x06, /*!< adc sample time 71.5 cycle */
  ADC_SAMPLETIME_239_5                   = 0x07  /*!< adc sample time 239.5 cycle */
} adc_sampletime_select_type;

typedef enum
{
  /*adc1 ordinary trigger event*/
  ADC12_ORDINARY_TRIG_TMR1CH1            = 0x00, /*!< timer1 ch1 event as trigger source of adc1 ordinary sequence */
  ADC12_ORDINARY_TRIG_TMR1CH2            = 0x01, /*!< timer1 ch2 event as trigger source of adc1 ordinary sequence */
  ADC12_ORDINARY_TRIG_TMR1CH3            = 0x02, /*!< timer1 ch3 event as trigger source of adc1 ordinary sequence */
  ADC12_ORDINARY_TRIG_TMR2CH2            = 0x03, /*!< timer2 ch2 event as trigger source of adc1 ordinary sequence */
  ADC12_ORDINARY_TRIG_TMR3TRGOUT         = 0x04, /*!< timer3 trgout event as trigger source of adc1 ordinary sequence */
  ADC12_ORDINARY_TRIG_TMR4CH4            = 0x05, /*!< timer4 ch4 event as trigger source of adc1 ordinary sequence */
  ADC12_ORDINARY_TRIG_EXINT11_TMR1TRGOUT = 0x06, /*!< exint line11/timer1 trgout event as trigger source of adc1 ordinary sequence */
  ADC12_ORDINARY_TRIG_SOFTWARE           = 0x07, /*!< software(OCSWTRG) control bit as trigger source of adc1 ordinary sequence */
  ADC12_ORDINARY_TRIG_TMR1TRGOUT         = 0x0D, /*!< timer1 trgout event as trigger source of adc1 ordinary sequence */
} adc_ordinary_trig_select_type;

typedef struct
{
  void*                            per;                     /*!< adc peripheral */
  uint8_t                          sequence_mode;           /*!< adc sequence mode */
  uint8_t                          repeat_mode;             /*!< adc repeat mode */
  adc_data_align_type              data_align;              /*!< adc data alignment */
  uint8_t                          ordinary_channel_length; /*!< adc ordinary channel sequence length*/
} adc_base_config_type;

void adc_base_config(ADC_Type *adcx, adc_base_config_type *adc_base_config);
void adc_ordinary_conversion_trigger_set(ADC_Type *adc_x, adc_ordinary_trig_select_type adc_ordinary_trig, uint8_t new_state);
void adc_ordinary_channel_set(ADC_Type *adc_x, adc_channel_select_type adc_channel, uint8_t adc_sequence, adc_sampletime_select_type adc_sampletime);

#endif // ADC_AT32F4XX_H