#ifndef _dma_stm32l4xx_h_
#define _dma_stm32l4xx_h_

#define DMA_NUMBER_POS     8
#define DMA_CHANNEL_POS    4
#define DMA_SOURCE_POS     0

#define DMA_NUMBER_MASK    (3 << DMA_NUMBER_POS)
#define DMA_CHANNEL_MASK   (15 << DMA_CHANNEL_POS)
#define DMA_SOURCE_MASK    (15 << DMA_SOURCE_POS)

#define DMA1_MAX_CHANNELS   7
#define DMA2_MAX_CHANNELS   7
#define DMA_MAX_CHANNELS    (DMA1_MAX_CHANNELS + DMA2_MAX_CHANNELS)

/**
 * DMA Requests
 * */
#define DMA1_REQ_TIM1_CH1               ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM1_CH2               ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM1_CH3               ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM1_CH4               ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM2_CH1               ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
#define DMA2_REQ_SPI1_TX                ((1 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))

#define DMA_CCR_PSIZE_8                 0
#define DMA_CCR_PSIZE_16                1
#define DMA_CCR_PSIZE_32                2
#define DMA_CCR_MSIZE_8                 0
#define DMA_CCR_MSIZE_16                1
#define DMA_CCR_MSIZE_32                2

#define DMA_CCR_PSIZE_CFG(x)            (x << 8)
#define DMA_CCR_MSIZE_CFG(x)            (x << 10)

#endif