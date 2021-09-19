#include "board.h"
#include "spi.h"

#define SPI_DMA                 DMA2
#define SPI_DMA_CH              DMA2_Channel4
#define SPI_DMA_CSELR_SPI_TX    DMA2_CSELR->CSELR = (DMA2_CSELR->CSELR & ~DMA_CSELR_C4S_Msk) | (4U << DMA_CSELR_C4S_Pos)
#define SPI_DMA_IRQn            DMA2_Channel4_IRQn
#define PCLK_CLK_DIV2           (0)
#define PCLK_CLK_DIV4           (1 << 3)
#define PCLK_CLK_DIV8           (2 << 3)
#define PCLK_CLK_DIV16          (3 << 3)
#define PCLK_CLK_DIV32          (4 << 3)
#define PCLK_CLK_DIV64          (5 << 3)
#define PCLK_CLK_DIV128         (6 << 3)
#define PCLK_CLK_DIV256         (7 << 3)
#define SPIDEV_SET_FLAG(_D, _F) _D->cfg |= _F
#define SPIDEV_CLR_FLAG(_D, _F) _D->cfg &= ~(_F)

/**
 * @brief DMA Interrupt handler
 * */
void SPI_DMA_IRQHandler(void){
    spidev_t *spidev = &spi1;
    SPI_TypeDef *spi = (SPI_TypeDef*)spidev->ctrl;
    
    SPI_DMA_CH->CCR &= ~(DMA_CCR_EN);
        
    if(spidev->trf_counter > 0x10000){
        spidev->trf_counter -= 0x10000;
        SPI_DMA_CH->CNDTR = (spidev->trf_counter > 0x10000UL) ? 0xFFFFUL : spidev->trf_counter;
        SPI_DMA_CH->CCR |= DMA_CCR_EN;
    }else{
        // wait for the last byte to be transmitted
        while(spi->SR & SPI_SR_BSY){
            if(spi->SR & SPI_SR_OVR){
                //dummy read for clearing OVR flag
                spidev->trf_counter = spi->DR;
            }
        }
        /* Restore 8bit Spi */        
        spi->CR2 &= ~(SPI_CR2_DS_3);       // 8-bit
        //spi->CR2 &= ~(SPI_CR2_TXDMAEN);
        spidev->trf_counter = 0;
        if(spidev->eot_cb){
            spidev->eot_cb();
        }
        SPIDEV_CLR_FLAG(spidev, SPI_IDLE);
    }
}

/**
 * Public API
 * */

/**
 * @brief Init
 * */
void SPI_Init(spidev_t *spidev){
    SPI_TypeDef *spi;

    switch(spidev->bus){
        case SPI_BUS0:
            __HAL_RCC_SPI1_CLK_ENABLE();
            spidev->ctrl = SPI1;
            break;

        case SPI_BUS1:
            __HAL_RCC_SPI2_CLK_ENABLE();
            spidev->ctrl = SPI2;
            break;

        default : return;
    }
    
    spi = (SPI_TypeDef*)spidev->ctrl;

    spi->CR1 = SPI_CR1_MSTR | PCLK_CLK_DIV4;
    spi->CR2 = (7 << 8);                        // Transfer 8-bit
#ifdef SPI_SW_CS
    spi->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;                            
#else
    spi->CR2 |= SPI_CR2_NSSP | SPI_CR2_SSOE;
#endif                

    spi->CR1 |= SPI_CR1_SPE;
    spi->CR2 |= SPI_CR2_TXDMAEN;
    
    SPI_DMA_CH->CCR =
            DMA_CCR_MSIZE_0 |                           // 16bit Dst size
			DMA_CCR_PSIZE_0 |                           // 16bit src size
            DMA_CCR_DIR |                               // Write to peripheral
			DMA_CCR_TCIE;                               // Enable Transfer Complete interrupt
    SPI_DMA_CH->CPAR = (uint32_t)&spi->DR;      // Peripheral source
    SPI_DMA_CH->CCR |= DMA_CCR_MINC;                    // Enable memory increment
    SPI_DMA_CSELR_SPI_TX;
    HAL_NVIC_EnableIRQ(SPI_DMA_IRQn);
}

/**
 * @brief Write data to SPI
 * 
 * \param data  : Pointer to data
 * \param count : total number of bytes to transfer
 * */
void SPI_Write(spidev_t *spidev, uint8_t *src, uint32_t count){
    SPI_TypeDef *spi = (SPI_TypeDef*)spidev->ctrl;
    while(count--){
        *((__IO uint8_t *)&spi->DR) = *src++;
        while((spi->SR & SPI_SR_BSY) != 0);
    }
}

/**
 * @brief Write data to SPI using DMA controller
 * 
 * \param data  : Pointer to data
 * \param count : total number of transfers
 * */
void SPI_WriteDMA(spidev_t *spidev, uint16_t *src, uint32_t count){
    SPI_TypeDef *spi = (SPI_TypeDef*)spidev->ctrl;

    spi->CR2 |= SPI_CR2_DS_3;       // 16-bit

    if(spidev->cfg & SPI_DMA_NO_MINC){
        SPI_DMA_CH->CCR &= ~(DMA_CCR_MINC);
    }else{
        SPI_DMA_CH->CCR |= DMA_CCR_MINC;
    }

    //spi->CR2 |= SPI_CR2_TXDMAEN;

    spidev->trf_counter = count;    
    SPI_DMA_CH->CMAR = (uint32_t)src;
    SPI_DMA_CH->CNDTR = (spidev->trf_counter > 0x10000) ? 0xFFFF : spidev->trf_counter;
    
    SPI_DMA_CH->CCR |= DMA_CCR_EN;
    SPIDEV_SET_FLAG(spidev, SPI_BUSY);
}

/**
 * @brief Write constant data to SPI using DMA controller
 * 
 * \param data  : data
 * \param count : total number of transfers
 * */
void SPI_WriteIntDMA(spidev_t *spidev, uint16_t data, uint32_t count){
    static uint16_t _data = 0;
    _data = data;
    SPIDEV_SET_FLAG(spidev, SPI_DMA_NO_MINC);
    SPI_WriteDMA(spidev, &_data, count);
}

/**
 * @brief Wait for end of DMA transfer
 * */
void SPI_WaitEOT(spidev_t *spidev){
    SPI_TypeDef *spi = (SPI_TypeDef*)spidev->ctrl;
    while(spi->SR & SPI_SR_BSY){
        LED_TOGGLE;
    }
}