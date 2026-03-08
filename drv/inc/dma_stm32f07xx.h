#ifndef _dma_stm32f0xx_h_
#define _dma_stm32f0xx_h_

#define DMA_NUMBER_POS          4
#define DMA_CHANNEL_POS         0

#define DMA_NUMBER_MASK         (1 << DMA_NUMBER_POS)
#define DMA_CHANNEL_MASK        (15 << DMA_CHANNEL_POS)

#define DMA1_MAX_CHANNELS       7
#define DMA2_MAX_CHANNELS       0
#define DMA_NUM_CHANNELS        (DMA1_MAX_CHANNELS + DMA2_MAX_CHANNELS)

#define DMA_CCR_PSIZE_CFG(x)    (x << 8)
#define DMA_CCR_MSIZE_CFG(x)    (x << 10)

#define DMA_CCR_PSIZE_8                 0
#define DMA_CCR_PSIZE_16                1
#define DMA_CCR_PSIZE_32                2
#define DMA_CCR_MSIZE_8                 0
#define DMA_CCR_MSIZE_16                1
#define DMA_CCR_MSIZE_32                2
/**
 * DMA Requests
 * */
#define DMA1_CH1_REQ            ((0 << DMA_NUMBER_POS) | (0 << DMA_CHANNEL_POS))
#define DMA1_CH2_REQ            ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS))
#define DMA1_CH3_REQ            ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS))
#define DMA1_CH4_REQ            ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS))
#define DMA1_CH5_REQ            ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS))
#define DMA1_CH6_REQ            ((0 << DMA_NUMBER_POS) | (5 << DMA_CHANNEL_POS))
#define DMA1_CH7_REQ            ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS))

#define DMA1_REQ_ADC1           DMA1_CH1_REQ
#define DMA1_REQ_TIM2_CH3       DMA1_CH1_REQ
#define DMA1_REQ_TIM17_CH1      DMA1_CH1_REQ
#define DMA1_REQ_TIM17_UP       DMA1_CH1_REQ
#define DMA1_REQ_SPI1_RX        DMA1_CH2_REQ
#define DMA1_REQ_USART1_TX      DMA1_CH2_REQ
#define DMA1_REQ_I2C1_TX        DMA1_CH2_REQ
#define DMA1_REQ_TIM1_CH1       DMA1_CH2_REQ
#define DMA1_REQ_TIM2_UP        DMA1_CH2_REQ
#define DMA1_REQ_TIM3_CH3       DMA1_CH2_REQ
#define DMA1_REQ_SPI1_TX        DMA1_CH3_REQ
#define DMA1_REQ_USART1_RX      DMA1_CH3_REQ
#define DMA1_REQ_I2C1_RX        DMA1_CH3_REQ
#define DMA1_REQ_TIM1_CH2       DMA1_CH3_REQ
#define DMA1_REQ_TIM2_CH2       DMA1_CH3_REQ
#define DMA1_REQ_TIM3_CH4       DMA1_CH3_REQ
#define DMA1_REQ_TIM3_UP        DMA1_CH3_REQ
#define DMA1_REQ_TIM6_UP        DMA1_CH3_REQ
#define DMA1_REQ_DAC_CH1        DMA1_CH3_REQ
#define DMA1_REQ_TIM16_CH1      DMA1_CH3_REQ
#define DMA1_REQ_TIM16_UP       DMA1_CH3_REQ
#define DMA1_REQ_SPI2_RX        DMA1_CH4_REQ
#define DMA1_REQ_USART2_TX      DMA1_CH4_REQ
#define DMA1_REQ_I2C2_TX        DMA1_CH4_REQ
#define DMA1_REQ_TIM1_CH4       DMA1_CH4_REQ
#define DMA1_REQ_TIM1_TRIG      DMA1_CH4_REQ
#define DMA1_REQ_TIM1_COM       DMA1_CH4_REQ
#define DMA1_REQ_TIM2_CH4       DMA1_CH4_REQ
#define DMA1_REQ_TIM3_CH1       DMA1_CH4_REQ
#define DMA1_REQ_TIM3_TRIG      DMA1_CH4_REQ
#define DMA1_REQ_TIM7_UP        DMA1_CH4_REQ
#define DMA1_REQ_DAC_CH2        DMA1_CH4_REQ
#define DMA1_REQ_SPI2_TX        DMA1_CH5_REQ
#define DMA1_REQ_USART2_RX      DMA1_CH5_REQ
#define DMA1_REQ_I2C2_RX        DMA1_CH5_REQ
#define DMAREQ_TIM1_CH3         DMA1_CH5_REQ
#define DMA1_REQ_TIM1_UP        DMA1_CH5_REQ
#define DMA1_REQ_TIM2_CH1       DMA1_CH5_REQ
#define DMA1_REQ_TIM15_CH1      DMA1_CH5_REQ
#define DMA1_REQ_TIM15_UP       DMA1_CH5_REQ
#define DMA1_REQ_TIM15_TRIG     DMA1_CH5_REQ
#define DMA1_REQ_TIM15_COM      DMA1_CH5_REQ
#define DMA1_REQ_USART3_RX      DMA1_CH6_REQ
#define DMA1_REQ_USART4_RX      DMA1_CH6_REQ
#define DMA1_REQ_USART3_TX      DMA1_CH7_REQ
#define DMA1_REQ_USART4_TX      DMA1_CH7_REQ

#endif