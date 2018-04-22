#ifndef __adc_h__
#define __adc_h__

#include <stdint.h>

#define ADC_MAX_CLK  13000000
#define ADC_MIN_CLK  15625       //4Mhz / 256
#define ADC_MAX_DIV  256
#define ADC_MAX_VAL  0xFFF

#define PCADC        12
#define SBIT_PDN     21
#define SBIT_CLKDIV  8
#define SBIT_DONE    31
#define SBIT_START   24
#define SBIT_EDGE    27

#define ADGINTEN     (1<<8)

#define START_NOW    1

#define ADC_CHANNELS 8

#define ADC_0        0      //P0.23
#define ADC_1        1      //P0.24
#define ADC_2        2      //P0.25
#define ADC_3        3      //P0.26
#define ADC_4        4      //P1.30 -> Vbus
#define ADC_5        5      //P1.31
#define ADC_6        6      //P0.3
#define ADC_7        7      //P0.2



/**
 * @brief MAX adc clock is 13Mhz, to archive this the ADC
 * divides PCLK by the given clkdiv value, ADC_0 is selected by default
 **/  
void ADC_Init(uint32_t clk);

/**
 * @brief set PCLK divider for ADC
 **/
void ADC_SetClockDivider(uint8_t ckdiv);

/**
 *@brief Configue a Channel as analog input
 **/
void ADC_ConfigureChannel(uint8_t ch);

/**
 *@brief Select a to convert
 **/
void ADC_SelectChannel(uint8_t ch);

/**
 *@brief Manual start of convertion
 **/
void ADC_Start(void);

/**
 * @brief get ADC value for current channel
 * this call is blocking       
 **/
uint16_t ADC_GetValue(void);

/**
 * @brief Get single convertion from channel
 **/
uint16_t ADC_ConvertSingle(uint8_t ch);

#endif
