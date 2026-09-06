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
#define DMA_NUM_CHANNELS    (DMA1_MAX_CHANNELS + DMA2_MAX_CHANNELS)

/**
 * DMA Requests
 * */
//  DMA1 Channel 1
#define DMA1_REQ_ADC1                   ((0 << DMA_NUMBER_POS) | (0 << DMA_CHANNEL_POS) | (0 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM2_CH3               ((0 << DMA_NUMBER_POS) | (0 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
// DMA1 Channel 2
#define DMA1_REQ_ADC2                   ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (0 << DMA_SOURCE_POS))
#define DMA1_REQ_SPI1_RX                ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (1 << DMA_SOURCE_POS))
#define DMA1_REQ_USART3_TX              ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (2 << DMA_SOURCE_POS))
#define DMA1_REQ_I2C3_TX                ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (3 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM2_UP                ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM3_CH3               ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (5 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM1_CH1               ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
// DMA1 Channel 3
#define DMA1_REQ_SPI1_TX                ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (1 << DMA_SOURCE_POS))
#define DMA1_REQ_USART3_RX              ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (2 << DMA_SOURCE_POS))
#define DMA1_REQ_I2C3_RX                ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (3 << DMA_SOURCE_POS))
//#define DMA1_REQ_TIM16_CH1              ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
//#define DMA1_REQ_TIM16_UP               DMA1_REQ_TIM16_CH1
#define DMA1_REQ_TIM3_CH4               ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (5 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM3_UP                DMA1_REQ_TIM3_CH4
#define DMA1_REQ_TIM6_UP                ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (6 << DMA_SOURCE_POS))
#define DMA1_REQ_DAC_CH1                DMA1_REQ_TIM6_UP
#define DMA1_REQ_TIM1_CH2               ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
// DMA1 Channel 4
#define DMA1_REQ_DFSDM1_FLT0            ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (0 << DMA_SOURCE_POS))
#define DMA1_REQ_SPI2_RX                ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (1 << DMA_SOURCE_POS))
#define DMA1_REQ_USART1_TX              ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (2 << DMA_SOURCE_POS))
#define DMA1_REQ_I2C2_TX                ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (3 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM7_UP                ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (5 << DMA_SOURCE_POS))
#define DMA1_REQ_DAC_CH2                DMA1_REQ_TIM7_UP
#define DMA1_REQ_TIM1_CH4               ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM1_TRIG              DMA1_REQ_TIM1_CH4
#define DMA1_REQ_TIM1_COM               DMA1_REQ_TIM1_CH4
// DMA1 Channel 5
#define DMA1_REQ_DFSDM1_FLT1            ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (0 << DMA_SOURCE_POS))
#define DMA1_REQ_SPI2_TX                ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (1 << DMA_SOURCE_POS))
#define DMA1_REQ_USART1_RX              ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (2 << DMA_SOURCE_POS))
#define DMA1_REQ_I2C2_RX                ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (3 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM2_CH1               ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
#define DMA1_REQ_QUADSPI                ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (5 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM15_CH1              ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM15_UP               DMA1_REQ_TIM15_CH1
#define DMA1_REQ_TIM15_TRIG             DMA1_REQ_TIM15_CH1
#define DMA1_REQ_TIM15_COM              DMA1_REQ_TIM15_CH1
// DMA1 Channel 6
#define DMA1_REQ_SAI1_A                 ((0 << DMA_NUMBER_POS) | (5 << DMA_CHANNEL_POS) | (1 << DMA_SOURCE_POS))
#define DMA1_REQ_USART2_RX              ((0 << DMA_NUMBER_POS) | (5 << DMA_CHANNEL_POS) | (2 << DMA_SOURCE_POS))
#define DMA1_REQ_I2C1_TX                ((0 << DMA_NUMBER_POS) | (5 << DMA_CHANNEL_POS) | (3 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM16_CH1              ((0 << DMA_NUMBER_POS) | (5 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM16_UP               DMA1_REQ_TIM16_CH1
#define DMA1_REQ_TIM3_CH1               ((0 << DMA_NUMBER_POS) | (5 << DMA_CHANNEL_POS) | (5 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM3_TRIG              DMA1_REQ_TIM3_CH1
#define DMA1_REQ_TIM1_UP                ((0 << DMA_NUMBER_POS) | (5 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
// DMA1 Channel 7
#define DMA1_REQ_SAI1_1                 ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (1 << DMA_SOURCE_POS))
#define DMA1_REQ_USART2_TX              ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (2 << DMA_SOURCE_POS))
#define DMA1_REQ_I2C1_RX                ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (3 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM2_CH2               ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
#define DMA1_REQ_TIM2_CH4               DMA1_REQ_TIM16_CH1
#define DMA1_REQ_TIM1_CH3               ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
// DMA2 Channel 1
#define DMA2_REQ_I2C4_RX                ((0 << DMA_NUMBER_POS) | (0 << DMA_CHANNEL_POS) | (0 << DMA_SOURCE_POS))
#define DMA2_REQ_SAI1_A                 ((0 << DMA_NUMBER_POS) | (0 << DMA_CHANNEL_POS) | (1 << DMA_SOURCE_POS))
#define DMA2_REQ_SPI3_RX                ((0 << DMA_NUMBER_POS) | (0 << DMA_CHANNEL_POS) | (3 << DMA_SOURCE_POS))
#define DMA2_REQ_SWPMI_RX               ((0 << DMA_NUMBER_POS) | (0 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
#define DMA2_REQ_AES_IN                 ((0 << DMA_NUMBER_POS) | (0 << DMA_CHANNEL_POS) | (6 << DMA_SOURCE_POS))
// DMA2 Channel 2
#define DMA2_REQ_I2C4_TX                ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (0 << DMA_SOURCE_POS))
#define DMA2_REQ_SAI1_B                 ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (1 << DMA_SOURCE_POS))
#define DMA2_REQ_SPI3_TX                ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (3 << DMA_SOURCE_POS))
#define DMA2_REQ_SWPMI_TX               ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
#define DMA2_REQ_AES_OUT                ((0 << DMA_NUMBER_POS) | (1 << DMA_CHANNEL_POS) | (6 << DMA_SOURCE_POS))
// DMA2 Channel 3
#define DMA2_REQ_ADC1                   ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (0 << DMA_SOURCE_POS))
#define DMA2_REQ_UART4_TX               ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (2 << DMA_SOURCE_POS))
#define DMA2_REQ_SPI1_RX                ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
//#define DMA2_REQ_AES_OUT                ((0 << DMA_NUMBER_POS) | (2 << DMA_CHANNEL_POS) | (6 << DMA_SOURCE_POS))
// DMA2 Channel 4
#define DMA2_REQ_ADC2                   ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (0 << DMA_SOURCE_POS))
#define DMA2_REQ_TIM6_UP                ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (3 << DMA_SOURCE_POS))
#define DMA2_REQ_DAC_CH1                DMA2_REQ_TIM6_UP
#define DMA2_REQ_SPI1_TX                ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
#define DMA2_REQ_SDMMC1                 ((0 << DMA_NUMBER_POS) | (3 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
// DMA2 Channel 5
#define DMA2_REQ_UART4_RX               ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (2 << DMA_SOURCE_POS))
#define DMA2_REQ_TIM7_UP                ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (3 << DMA_SOURCE_POS))
#define DMA2_REQ_DAC_CH2                DMA2_REQ_TIM7_UP
//#define DMA2_REQ_AES_IN                 ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (6 << DMA_SOURCE_POS))
//#define DMA2_REQ_SDMMC1                 ((0 << DMA_NUMBER_POS) | (4 << DMA_CHANNEL_POS) | (7 << DMA_SOURCE_POS))
// DMA2 Channel 6
//#define DMA2_REQ_SAI1_A                 ((0 << DMA_NUMBER_POS) | (5 << DMA_CHANNEL_POS) | (1 << DMA_SOURCE_POS))
#define DMA2_REQ_USART1_TX              ((0 << DMA_NUMBER_POS) | (5 << DMA_CHANNEL_POS) | (2 << DMA_SOURCE_POS))
#define DMA2_REQ_LPUSART1_TX            ((0 << DMA_NUMBER_POS) | (5 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
#define DMA2_REQ_I2C1_RX                ((0 << DMA_NUMBER_POS) | (5 << DMA_CHANNEL_POS) | (5 << DMA_SOURCE_POS))
// DMA2 Channel 7
//#define DMA2_REQ_SAI1_B                 ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (1 << DMA_SOURCE_POS))
#define DMA2_REQ_USART1_RX              ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (2 << DMA_SOURCE_POS))
#define DMA2_REQ_QUADSPI                ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (3 << DMA_SOURCE_POS))
#define DMA2_REQ_LPUSART1_RX            ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (4 << DMA_SOURCE_POS))
#define DMA2_REQ_I2C1_TX                ((0 << DMA_NUMBER_POS) | (6 << DMA_CHANNEL_POS) | (5 << DMA_SOURCE_POS))


#define DMA_CCR_PSIZE_8                 0
#define DMA_CCR_PSIZE_16                1
#define DMA_CCR_PSIZE_32                2
#define DMA_CCR_MSIZE_8                 0
#define DMA_CCR_MSIZE_16                1
#define DMA_CCR_MSIZE_32                2

#define DMA_CCR_PSIZE_CFG(x)            (x << 8)
#define DMA_CCR_MSIZE_CFG(x)            (x << 10)

#endif