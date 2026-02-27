#include "at32f4xx.h"
#include "adc_at32f4xx.h"
#include "dma_at32f4xx.h"
#include "adc.h"
#include "dma.h"
#include "clock.h"

typedef struct{
    ADC_Type *adc;
    TMR_Type *tmr;
    dmatype_t dma;
    uint16_t size;
    uint16_t *buf;
    void (*cb)(uint16_t*, uint16_t);
}adchandle_t;

static adchandle_t hadc1;

/**
 * @brief
 *
 * @param
 */
static void adc_cb(void)
{
    if(hadc1.cb){
        uint32_t half = hadc1.size >> 1;
        if(DMA_GetTransfers(&hadc1.dma) < half){
            // Second half
            hadc1.cb(hadc1.buf + half, half);
        }else{
            hadc1.cb(hadc1.buf, half);
        }
    }
}

/**
 * @brief
 *
 * @param ADCx
 * @return
 */
static uint16_t adc_calibration(ADC_Type *ADCx)
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
    return ADCx->RDOR;
}

/**
 * @brief Configures a timer to trigger adc
 *
 *
 * @param tmr
 * @param pclk
 * @param freq
 */
static void adc_cfg_timer(TMR_Type *tmr, uint32_t freq)
{
    uint32_t pclk;
    uint32_t bestPrescaler = 1;

    #ifdef USE_STDPERIPH_DRIVER
    RCC_ClockType clocks;
    RCC_GetClocksFreq(&clocks);
    pclk = clocks.APB1CLK_Freq;
    #else
    sysclock_t clocks;
    CLOCK_GetAll(&clocks);
    pclk = clocks.clk1;
    #endif

    pclk = ((RCC->CFG >> 8) < 4) ?
                pclk :                  // APB pres is 1
                pclk << 1;              // APB is doubled

    uint32_t ticks = pclk / freq;
    uint32_t bestReload = ticks / bestPrescaler;

    while(bestReload > 0x10000){
        bestPrescaler++;
        bestReload = ticks / bestPrescaler;
    }

    tmr->CTRL1  = 0,
    tmr->AR = (uint16_t)(bestReload);
    tmr->DIV = (uint16_t)bestPrescaler;

#if 0 /* DEBUG */
    tmr->CC3 = tmr->AR >> 1;
    tmr->CCM2 = (6 << 4);
    GPIOB->CTRLL = (GPIOB->CTRLL & ~0x0F) | 0xB;
    tmr->CCE  = TMR_CCE_C3EN;
#endif
}

/**
 * @brief Initializes ADC.
 *
 * If using preemptive channels, only 4 channels are available, in this mode,
 * DMA is not necessary since each channel has his own data register.
 *
 * By default ADC is configured to convert a single regular channel (AN0) with software trigger.
 *
 * Note each ADC trigger, will make sequence length DMA transfers.
 *
 * ex:
 *      seq len = 3
 *      DMA len = 10
 *
 * At ADC EoC DMA has transferred 3 samples and DMA->TNT is 7
 *
 * Possible ADC triggers:
 * TMR1_CH1     TMR2_CH2
 * TMR1_CH2     TMR3_TGROUT
 * TMR1_CH3     TMR4_CH4
 * TMR1_TRGOUT
 *
 * @retval Internal reference voltage value (1.17V - 1.23V)
 */
uint16_t ADC_Init(adctype_t *adct)
{
    adchandle_t *hadc = &hadc1;

    RCC->APB2EN |= RCC_APB2EN_ADC1EN;
    RCC->APB2RST |= RCC_APB2RST_ADC1RST;
    RCC->APB2RST &= ~RCC_APB2RST_ADC1RST;
    // Use slowest clock for better results, Fadc = APB2 / 16
    RCC->CFG = (RCC->CFG & ~RCC_CFG_ADCPSC) | RCC_CFG_ADCPSC_DIV16;
    // Use preemptive channels for single conversions
    ADC1->CTRL2 = (ADC12_PREEMPT_TRIG_SOFTWARE << 12) |
                  ADC_CTRL2_JEXTREN |   // Enable preemptive trigger
    /* and ordinary channels for acquisition stream */
                  (ADC12_ORDINARY_TRIG_TMR3TRGOUT << 17) |
                  ADC_CTRL2_TSREF |     // Enable Voltage reference
                  ADC_CTRL2_EXTREN |    // Enable trigger on ordinary channels
                  ADC_CTRL2_ADON;       // Align right, no repeat

    ADC1->JSQ = 0;                      // start with one channel on preemptive sequence
    ADC1->RSQ1 = 0;                     // start with one channel on ordinary sequence

    hadc->adc = ADC1;
    hadc->tmr = TMR3;
    adct->handle = hadc;

    adc_calibration(ADC1);
    /* Configure interval voltage reference to make a conversion on it */
    ADC_ConfigChannel(adct, ADC_CHANNEL_INTRV, 0, ADC_SAMPLETIME_239_5);
    /* Return internal reference value */
    return ADC_ConvertSingle(adct, ADC_CHANNEL_INTRV);
}

/**
 * @brief  Configures a channel to be converted as part of conversion sequence
 *          and it's sample time
 *
 * @param adct:     Adc type instance
 * @param ch:       Channel number 0~17
 * @param seq_idx   Index of conversion for channel in sequence 0~16, if index is > 16,
 *                  the channel is configured on ordinary sequence for acquisition.
 *
 * @param sampletime: sample time in cycles
 *
 *  Tsample = Cycles/Fadc, Fadc = 4.68MHz => Max Tsample = 51us
 *
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
void ADC_ConfigChannel(adctype_t *adc, uint8_t ch, uint8_t seq_idx, uint8_t sampletime)
{
    (void)adc;
    uint32_t tmp_reg;

    if(ch > ADC_CHANNEL_17){
        return;
    }

    // Configure sample time in correspondent register/bits
    if (ch < ADC_CHANNEL_10) {
        tmp_reg = ADC1->SMPT2;
        tmp_reg &= ~(0x07 << 3 * ch);
        tmp_reg |= sampletime << 3 * ch;
        ADC1->SMPT2 = tmp_reg;
    } else {
        tmp_reg = ADC1->SMPT1;
        tmp_reg &= ~(0x07 << 3 * (ch - ADC_CHANNEL_10));
        tmp_reg |= sampletime << 3 * (ch - ADC_CHANNEL_10);
        ADC1->SMPT1 = tmp_reg;
    }

    if(!(seq_idx & 0x80)){
        // Configure channel on preemptive sequence
        uint32_t len = (ADC1->JSQ & ADC_JSQ_JLEN) >> 20;
        uint32_t shift = (seq_idx & 3) + 3 - len;   // The conversion sequence starts from 4-PCLEN
        uint32_t mask = (0x1f << (shift * 5));
        ADC1->JSQ = (ADC1->JSQ & ~mask) | (ch << (shift * 5));
    }else{
        // Configure channel in ordinary sequence
        uint32_t len = (ADC1->RSQ1 & ADC_RSQ1_LEN) >> 20;

        seq_idx &= 0x7F;

        if(seq_idx > len){
            // increase sequence size
            ADC1->RSQ1 = (ADC1->RSQ1 & ~ADC_RSQ1_LEN) | (seq_idx << 20);
            ADC1->CTRL1 = ADC_CTRL1_SCN;        // Enable sequence mode
        }

        uint32_t offset = seq_idx / 6;          // Number of channels in each RSQx regiter

        __IO uint32_t *seq_reg = (&ADC1->RSQ3) - offset;

        uint32_t shift = seq_idx % 6;
        uint32_t mask = (ADC_RSQ3_SQ1 << (shift * 5));

        *seq_reg = (*seq_reg & ~mask) | (ch << (shift * 5));
    }
}

/**
 * @brief Configures acquisition channel, not sure if necessary
 *
 * @param adct
 * @param ch
 * @param seq_idx
 * @param sampletime
 */
void ADC_ConfigAcquisitionChannel(adctype_t *adct, uint8_t ch, uint8_t seq_idx, uint8_t sampletime)
{
    ADC_ConfigChannel(adct, ch, seq_idx | 0x80, sampletime);
}

/**
 * @brief
 * @param adct
 * @param freq
 */
void ADC_ConfigAcquisition(adctype_t *adct, uint32_t freq, uint16_t *buf, uint16_t size)
{
    adchandle_t *hadc = (adchandle_t*)adct->handle;

    hadc->size = size;
    hadc->buf = buf;
    hadc->cb = adct->eoc;

    hadc->dma.dir = DMA_DIR_P2M;
    hadc->dma.dsize = DMA_DATA_SIZE_16;
    hadc->dma.ssize = DMA_DATA_SIZE_16;
    hadc->dma.src = (void*)&hadc->adc->RDOR;
    hadc->dma.dst = (void*)buf;
    hadc->dma.len = size;
    hadc->dma.eot = adc_cb;
    hadc->dma.half = 1;

    if(DMA_Config(&hadc->dma, DMA1_REQ_ADC1)){
        // Using TMR3
        RCC->APB1EN |= RCC_APB1EN_TMR3EN;
        // Configure TMR3 MASTER mode, TRGOUT = UEVT
        hadc->tmr->CTRL2 = TMR_CTRL2_MMSEL_1;
        // Configure timer frequency
        adc_cfg_timer(hadc->tmr, freq);
        // Enable DMA
        hadc->adc->CTRL2 |= ADC_CTRL2_DMAEN;
    }
}

/**
 * @brief
 * @param adc
 */
void ADC_StartAcquisition(adctype_t *adc)
{
    adchandle_t *hadc = (adchandle_t *)adc->handle;

    DMA_Start(&hadc->dma);

    hadc->tmr->CTRL1 |= TMR_CTRL1_CNTEN;
}

/**
 * @brief
 * @param adc
 */
void ADC_StopAcquisition(adctype_t *adc)
{
    adchandle_t *hadc = (adchandle_t *)adc->handle;

    hadc->tmr->CTRL1 &= ~TMR_CTRL1_CNTEN;

    DMA_Cancel(&hadc->dma);
}

/**
 * @brief Do a single conversion on a given channel
 * This conversion uses preemptive channels.
 *
 * @param adc   ADC Instance
 * @param ch    Channel to be converted
 *
 * @return 12-bit sample value or 0xFFFF on error
 */
uint16_t ADC_ConvertSingle(adctype_t *adct, uint8_t ch)
{
    uint32_t timeout = 0xFFFF;
    adchandle_t *hadc = (adchandle_t*)adct->handle;
    ADC_Type *adc = hadc->adc;

    if(ch > ADC_CHANNEL_17){
        return 0xFFFF;
    }

    adc->STS = 0;
    /**
     * Remove any configured sequence and configure channel on
     * first conversion
     */
    adc->JSQ = ch << 15;   // Conversion sequence starts from 4-PCLEN.
    // start conversion
    adc->CTRL2 |= ADC_CTRL2_JSWSTR;

    while (!(adc->STS & ADC_STS_JEC) && timeout) {
        timeout--;
    }

    return timeout ? adc->JDOR1 : 0xFFFF;
}
/**
 * @brief  Initiates a conversion sequence of preemptive channels.
 *
 * @param  adct: adc type instance
 * @param  wait: wait for conversion to finish
 *
 * @retval 0: if conversion timedout, 1: otherwise
 */
uint8_t ADC_Convert(adctype_t *adct, uint8_t wait)
{
    (void)adct;
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
 * @brief  Get the data of the last conversion for a given channel
 *         This function gets data independently of channel position on
 *         preemptive  sequence
 *
 * @param  adc_x: select the adc peripheral.
 *         this parameter can be one of the following values:
 *         ADC1.
 *
 * @param  ch: channel number
 * @retval Channel data (12-bit) or 0xFFFF if invalid channel or not on sequence.
 */
uint16_t ADC_ConvertedDataGet(adctype_t *adc, uint8_t ch)
{
    (void)adc;
    uint8_t len = (ADC1->JSQ & ADC_JSQ_JLEN) >> 20;
    uint32_t reg_data = ADC1->JSQ;
    uint32_t *pdt = (uint32_t *)&ADC1->JDOR1;

    if (ch > 17) {
        return 0xFFFF;
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

    return 0xFFFF;
}

