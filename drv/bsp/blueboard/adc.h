#ifndef _ADC_h_
#define _ADC_h_

#include <stdint.h>

void ADC_Init(void);
uint16_t ADC_Convert(uint8_t ch);

#endif
