#ifndef _adc_lpc17xx_h_
#define _adc_lpc17xx_h_


#define ADC_MAX_CLK             13000000
#define ADC_DEFAULT_CLK         4000000
#define ADC_MIN_CLK             15625       //4Mhz / 256
#define ADC_MAX_DIV             256
#define ADC_MAX_VAL             0xFFF

// ADC Control register
#define ADC_CR_SEL_Pos          0
#define ADC_CR_CLKDIV_Pos       8
#define ADC_CR_START_Pos        24
#define ADC_CR_BURST            (1 << 16)
#define ADC_CR_PDN              (1 << 21)
#define ADC_CR_EDGE             (1 << 27)
#define ADC_CR_CLKDIV_SET(c)    ((c & 255) << ADC_CR_CLKDIV_Pos)
#define ADC_CR_SEL_SET(c)       ((c & 255) << ADC_CR_SEL_Pos)
#define ADC_CR_SEL_MSK          (255 << 0)
#define ADC_CR_START_MSK        (7 << ADC_CR_START_Pos)
#define ADC_CR_START_NOW        (1 << ADC_CR_START_Pos)
#define ADC_CR_START_EDGE_P210  (2 << ADC_CR_START_Pos)
#define ADC_CR_START_EDGE_P127  (3 << ADC_CR_START_Pos)
#define ADC_CR_START_EDGE_MAT01 (4 << ADC_CR_START_Pos)
#define ADC_CR_START_EDGE_MAT03 (5 << ADC_CR_START_Pos)
#define ADC_CR_START_EDGE_MAT10 (6 << ADC_CR_START_Pos)
#define ADC_CR_START_EDGE_MAT11 (7 << ADC_CR_START_Pos)

// ADC Data Register
#define ADC_GR_OVERRUN          (1 << 30)
#define ADC_DR_DONE             (1 << 31)

// ADC Interrupt Enable
#define ADC_INTEN_ADINT0        (1 << 0)
#define ADC_INTEN_ADINT1        (1 << 1)
#define ADC_INTEN_ADINT2        (1 << 2)
#define ADC_INTEN_ADINT3        (1 << 3)
#define ADC_INTEN_ADINT4        (1 << 4)
#define ADC_INTEN_ADINT5        (1 << 5)
#define ADC_INTEN_ADINT6        (1 << 6)
#define ADC_INTEN_ADINT7        (1 << 7)
#define ADC_INTEN_ADGINTEN      (1 << 8)

// ADC Status Register
#define ADC_SR_DONE0            (1 << 0)
#define ADC_SR_DONE1            (1 << 1)
#define ADC_SR_DONE2            (1 << 2)
#define ADC_SR_DONE3            (1 << 3)
#define ADC_SR_DONE4            (1 << 4)
#define ADC_SR_DONE5            (1 << 5)
#define ADC_SR_DONE6            (1 << 6)
#define ADC_SR_DONE7            (1 << 7)
#define ADC_SR_OVERRUN0         (1 << 8)
#define ADC_SR_OVERRUN1         (1 << 9)
#define ADC_SR_OVERRUN2         (1 << 10)
#define ADC_SR_OVERRUN3         (1 << 11)
#define ADC_SR_OVERRUN4         (1 << 12)
#define ADC_SR_OVERRUN5         (1 << 13)
#define ADC_SR_OVERRUN6         (1 << 14)
#define ADC_SR_OVERRUN7         (1 << 15)
#define ADC_SR_ADINT            (1 << 16)

// ADC Trim
#define ADC_TRIM_OFFS_Pos       4
#define ADC_TRIM_TRIM_Pos       8

#define ADC_NUM_OF_CHANNELS     8

#define ADC_CH0                 (1 << 0) //P0.23
#define ADC_CH1                 (1 << 1) //P0.24
#define ADC_CH2                 (1 << 2) //P0.25
#define ADC_CH3                 (1 << 3) //P0.26
#define ADC_CH4                 (1 << 4) //P1.30 -> Vbus
#define ADC_CH5                 (1 << 5) //P1.31
#define ADC_CH6                 (1 << 6) //P0.3
#define ADC_CH7                 (1 << 7) //P0.2

#endif