#include <stdint.h>
#include "lpc17xx_hal.h"
#include "adc.h"

#if 0
static adctype_t *s_adc;
static void configAnPin(uint8_t ch){
    uint8_t bitpos;

    if(ch < 4){                                    // configure channels 3:0 (P0_26:P0_23)
        bitpos = (14 + (2 * ch));                  // calculate bit position on pinsel1
        LPC_PINCON->PINSEL1 &= ~(0x03 << bitpos);
        LPC_PINCON->PINSEL1 |=  (0x01 << bitpos);  // Function1
    }else if(ch > 3 && ch < 6){                   // configure channels 5:4 (P1_31:P1_30)
        bitpos = (28 + (2 * (ch & 1)));
        LPC_PINCON->PINSEL3 |= (0x03 << bitpos);   // Function3
    }else{                                        // configure channels 7:6 (P0_3:P0_2)
        bitpos = (6 - (2 * (ch & 1)));
        LPC_PINCON->PINSEL0 &= ~(0x03 << bitpos);
        LPC_PINCON->PINSEL0 |=  (0x02 << bitpos);  // Function2
    }
}
#endif

uint16_t ADC_Init(adctype_t *adc)
{
#if 0 // TODO: Fix
    PCONP_ADC_ENABLE;
    /**
     * Maximum clock is 13MHz, lower clock
     * should be used for high impedance sources
     * */
    CLOCK_SetPCLK(PCLK_ADC, PCLK_1);
    LPC_ADC->CR = ADC_CR_PDN |
                    ADC_CR_CLKDIV_SET(CLOCK_GetPCLK(PCLK_ADC) / ADC_DEFAULT_CLK) |
                    ADC_CR_SEL_SET(adc->ch);

    for(uint8_t ch = 0; ch < ADC_NUM_OF_CHANNELS; ch++){
        if(adc->ch & (1 << ch)){
            configAnPin(ch);
        }
    }

    LPC_ADC->INTEN &= ~ADC_INTEN_ADGINTEN;

    adc->per = LPC_ADC;
    s_adc = adc;
    NVIC_EnableIRQ(ADC_IRQn);
#endif
    return 0;
}

void ADC_Config(adctype_t *adc){
#if 0 // TODO: Fix
    uint32_t intr = 0;
    for (uint8_t ch = 0; ch < ADC_NUM_OF_CHANNELS; ch++){
        if (adc->ch & (1 << ch)){
            if(adc->eoc) {
                intr |= (1 << ch);
            }
        }
    }

    LPC_ADC->CR = (LPC_ADC->CR & ~ADC_CR_SEL_MSK) | ADC_CR_SEL_SET(intr);
    LPC_ADC->INTEN = intr;
#endif
}

void ADC_Start(adctype_t *adc){
    ADC_Config(adc);
    LPC_ADC->CR = (LPC_ADC->CR & ~ADC_CR_START_MSK) | ADC_CR_BURST;
}

void ADC_Stop(adctype_t *adc) {
    LPC_ADC->CR = LPC_ADC->CR & ~(ADC_CR_START_MSK | ADC_CR_BURST);
}

uint16_t ADC_ConvertSingle(adctype_t *adc, uint8_t ch)
{
#if 0 // TODO: Fix
    for (uint8_t ch = 0; ch < ADC_NUM_OF_CHANNELS; ch++) {
        if (adc->ch & (1 << ch)){
            LPC_ADC->CR &= ~ADC_CR_SEL_MSK;
            LPC_ADC->CR |= (1 << (adc->ch & 7));

            if(adc->eoc) {
                LPC_ADC->INTEN = ADC_INTEN_ADGINTEN;
                LPC_ADC->CR |= ADC_CR_START_NOW;
                break;
            }else{
                LPC_ADC->INTEN = 0;
                LPC_ADC->CR |= ADC_CR_START_NOW;
                while(!(LPC_ADC->GDR & ADC_DR_DONE));
                return (LPC_ADC->GDR >> 4) & 0xFFF;
            }
        }
    }
#endif
    return 0xFFFF;
}

void ADC_IRQHandler(void)
{
    // TODO: FIX when it is necessary
    #if 0
    uint32_t stat = LPC_ADC->STAT;
    uint16_t *dst = s_adc->buf;
    uint16_t *src = (uint16_t*)&LPC_ADC->DR[0];

    for (uint32_t i = 1; i < 0x100 ; i <<= 1, src += 2) {
        if(stat & i) {
            *(dst++) = *src & 0xFFF;
        }
    }
    //s_adc->eoc();
    #endif
}
