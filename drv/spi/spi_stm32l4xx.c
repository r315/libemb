#include "stm32l4xx.h"
#include "spi.h"
#include "dma.h"

#define PCLK_CLK_DIV2           (0)
#define PCLK_CLK_DIV4           (1 << 3)
#define PCLK_CLK_DIV8           (2 << 3)
#define PCLK_CLK_DIV16          (3 << 3)
#define PCLK_CLK_DIV32          (4 << 3)
#define PCLK_CLK_DIV64          (5 << 3)
#define PCLK_CLK_DIV128         (6 << 3)
#define PCLK_CLK_DIV256         (7 << 3)
#define SPIDEV_SET_FLAG(_D, _F) _D->flags |= _F
#define SPIDEV_CLR_FLAG(_D, _F) _D->flags &= ~(_F)
#define SPIDEV_GET_FLAG(_D, _F) !! (_D->flags & _F)

/**
 * @brief DMA Interrupt handler
 * */
void SPI_DMA_IRQHandler(spibus_t *spidev){
    SPI_TypeDef *spi = (SPI_TypeDef*)spidev->ctrl;
    DMA_Channel_TypeDef *dma_channel = (DMA_Channel_TypeDef*)spidev->dma.channel;
    
    dma_channel->CCR &= ~(DMA_CCR_EN);
        
    if(spidev->trf_counter > 0x10000UL){
        spidev->trf_counter -= 0x10000UL;
        dma_channel->CNDTR = (spidev->trf_counter > 0x10000UL) ? 0xFFFFUL : spidev->trf_counter;
        dma_channel->CCR |= DMA_CCR_EN;
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
        spidev->trf_counter = 0;
        if(spidev->eot_cb){
            spidev->eot_cb();
        }
        SPIDEV_CLR_FLAG(spidev, SPI_BUSY | SPI_DMA_NO_MINC);
    }
}

/**
 * @brief Configures baud rate by dividing Fpckl
 * by 2, 4, 8, 16, 32, 64, 128 or 256.
 * 
 * spi peripheral must be enabled afterwards 
 * 
 * */
static void SPI_SetFreq(SPI_TypeDef *spi, uint32_t freq){

    uint32_t div = (SystemCoreClock/1000)/freq;
    uint32_t br = 8;

    if(div > 256){
        div = 256;
    }

    if(div < 2){
        div = 2;
    }

    while(br){
        if((div & (1 << br)) != 0){
            break;
        }
        br--;
    }    

    spi->CR1 &= ~(SPI_CR1_SPE | PCLK_CLK_DIV256);
    spi->CR1 |= (br << SPI_CR1_BR_Pos);
}
/**
 * Public API
 * */

/**
 * @brief Init
 * */
void SPI_Init(spibus_t *spidev){
    SPI_TypeDef *spi;
    DMA_Channel_TypeDef *dma_channel;

    switch(spidev->bus){
        case SPI_BUS0:
            __HAL_RCC_SPI1_CLK_ENABLE();
            spi = SPI1;
            break;

        case SPI_BUS1:
            __HAL_RCC_SPI2_CLK_ENABLE();
            spi = SPI2;
            break;

        default : 
            return;
    }

    spi->CR1 = SPI_CR1_MSTR;
    spi->CR2 = (7 << SPI_CR2_DS_Pos);        // Transfer 8-bit

    SPI_SetFreq(spi, spidev->freq);

    if(spidev->flags & SPI_SW_CS){
        spi->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;                            
    }else{
        spi->CR2 |= SPI_CR2_NSSP | SPI_CR2_SSOE;
    }            

    spi->CR1 |= SPI_CR1_SPE;

    if(spidev->dma.ctrl != NULL){
        dma_channel = (DMA_Channel_TypeDef*)spidev->dma.channel;

        spi->CR2 |= SPI_CR2_TXDMAEN;
    
        dma_channel->CCR =
            DMA_CCR_MSIZE_0 |                           // 16bit Dst size
			DMA_CCR_PSIZE_0 |                           // 16bit src size
            DMA_CCR_DIR |                               // Write to peripheral
			DMA_CCR_TCIE;                               // Enable Transfer Complete interrupt
        dma_channel->CPAR = (uint32_t)&spi->DR;         // Peripheral source
        dma_channel->CCR |= DMA_CCR_MINC;               // Enable memory increment

        HAL_NVIC_EnableIRQ(spidev->dma.irq);
    }

    spidev->flags |= SPI_ENABLED;
    spidev->ctrl = spi;
}

/**
 * @brief Write data to SPI
 * 
 * \param data  : Pointer to data
 * \param count : total number of bytes to transfer
 * */
void SPI_Write(spibus_t *spidev, uint8_t *src, uint32_t count){
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
void SPI_WriteDMA(spibus_t *spidev, uint16_t *src, uint32_t count){
    static uint16_t _data;
    SPI_TypeDef *spi = (SPI_TypeDef*)spidev->ctrl;
    DMA_Channel_TypeDef *dma_channel = (DMA_Channel_TypeDef*)spidev->dma.channel;

    spi->CR2 |= SPI_CR2_DS_3;       // 16-bit

    if(spidev->flags & SPI_DMA_NO_MINC){
        _data = src[0];
        src = &_data;
        dma_channel->CCR &= ~(DMA_CCR_MINC);
    }else{
        dma_channel->CCR |= DMA_CCR_MINC;
    }

    spidev->trf_counter = count;    
    dma_channel->CMAR = (uint32_t)src;
    dma_channel->CNDTR = (spidev->trf_counter > 0x10000) ? 0xFFFF : spidev->trf_counter;
    
    dma_channel->CCR |= DMA_CCR_EN;
    SPIDEV_SET_FLAG(spidev, SPI_BUSY);
}

/**
 * @brief Wait for end of DMA transfer
 * */
void SPI_WaitEOT(spibus_t *spidev){    
    #if 1
    SPI_TypeDef *spi = (SPI_TypeDef*)spidev->ctrl;
    while(spi->SR & SPI_SR_BSY){
    #else
    while(SPIDEV_GET_FLAG(spidev, SPI_BUSY)){
    #endif
        //LED_TOGGLE;
    }
}