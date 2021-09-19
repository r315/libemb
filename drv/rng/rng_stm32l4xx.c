#include "board.h"

void RNG_Init(void){
    __HAL_RCC_RNG_CLK_ENABLE();
    __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL_48M1CLK);
    __HAL_RCC_RNG_CONFIG(RCC_RNGCLKSOURCE_MSI);
    HAL_RCCEx_EnableMSIPLLMode();
    RNG->CR = (1 << 5) | RNG_CR_RNGEN;
}

uint32_t RNG_Get(){
    while((RNG->SR & RNG_SR_DRDY) == 0);
    return RNG->DR;
}