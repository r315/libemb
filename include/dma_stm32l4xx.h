#ifndef _dma_stm32l4xx_h_
#define _dma_stm32l4xx_h_

#define DMA_NUMBER_POS     8
#define DMA_CHANNEL_POS    4
#define DMA_SOURCE_POS     0

#define DMA_NUMBER_MASK    (3 << DMA_NUMBER_POS)
#define DMA_CHANNEL_MASK   (15 << DMA_CHANNEL_POS)
#define DMA_SOURCE_MASK    (15 << DMA_SOURCE_POS)

/**
 * DMA Requests
 * */
#define DMA1_TIM1_CH1               ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
#define DMA1_TIM1_CH2               ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
#define DMA1_TIM1_CH3               ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
#define DMA1_TIM1_CH4               ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))

#define DMA1_TIM2_CH1               ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
#define DMA2_SPI1_TX                ((1 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))

#endif