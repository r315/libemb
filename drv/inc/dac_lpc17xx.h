#ifndef _dac_lpc17xx_h_
#define _dac_lpc17xx_h_


#define DAC_MAX_VALUE       0x3FF
#define DAC_CFG_PIN         


// DACCR
#define DAC_CR_BIAS         (1 << 16)

// DACCTRL
#define CTRL_INT_DMA_REQ    (1 << 0)
#define CTRL_DBL_BUF        (1 << 1)
#define CTRL_CNT_ENA        (1 << 2)
#define CTRL_DMA_ENA        (1 << 3)

// DACCNTVAL
#define CNT_VAL_MSK         0xFFFF

#endif