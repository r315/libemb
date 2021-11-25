#include "board.h"
#include "spi.h"

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
#define SPIDEV_GET_FLAG(_D, _F) !!(_D->cfg & _F)

/**
 * @brief DMA Interrupt handler
 * */
void SPI_DMA_IRQHandler(spidev_t *spidev){
    SPI_TypeDef *spi = (SPI_TypeDef*)spidev->ctrl;
    DMA_Channel_TypeDef *dma = (DMA_Channel_TypeDef*)spidev->dma;
    
    dma->CCR &= ~(DMA_CCR_EN);
        
    if(spidev->trf_counter > 0x10000UL){
        spidev->trf_counter -= 0x10000UL;
        dma->CNDTR = (spidev->trf_counter > 0x10000UL) ? 0xFFFFUL : spidev->trf_counter;
        dma->CCR |= DMA_CCR_EN;
    }else{
        // wait for the last byte to be transmitted
        while(spi->SR & SPI_SR_BSY){
            if(spi->SR & SPI_SR_OVR){
                //dummy read for clearing OVR flag
                spidev->trf_counter = spi->DR;
            }
        }
        /* Restore 8bit Spi */        
        spi->CR1 &= ~(SPI_CR1_SPE | SPI_CR1_DFF);
	    spi->CR1 |= SPI_CR1_SPE;
        spi->CR2 &= ~(SPI_CR2_TXDMAEN);

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
void SPI_Init(spidev_t *spidev){
    SPI_TypeDef *spi;
    DMA_Channel_TypeDef *dma;
    IRQn_Type irq;

    switch(spidev->bus){
        case SPI_BUS0:
            __HAL_RCC_SPI1_CLK_ENABLE();
            __HAL_RCC_SPI1_FORCE_RESET();
            __HAL_RCC_SPI1_RELEASE_RESET();
            spidev->ctrl = SPI1;
            spidev->dma = DMA1_Channel3;
            irq = DMA1_Channel3_IRQn;
            break;

        case SPI_BUS1:
            __HAL_RCC_SPI2_CLK_ENABLE();
            __HAL_RCC_SPI2_FORCE_RESET();
            __HAL_RCC_SPI2_RELEASE_RESET();
            spidev->ctrl = SPI2;
            spidev->dma = DMA1_Channel5;
            irq = DMA1_Channel5_IRQn;
            break;

        default : return;
    }
    
    __HAL_RCC_DMA1_CLK_ENABLE();

    spi = (SPI_TypeDef*)spidev->ctrl;

    spi->CR1 = SPI_CR1_MSTR;
    
    SPI_SetFreq(spi, spidev->freq);

    if((spidev->cfg & SPI_SW_CS) != 0){
        spi->CR2 |=  SPI_CR2_SSOE;
    }            

    spi->CR1 |= SPI_CR1_SPE;
    
    spidev->trf_counter = 0;

    dma = (DMA_Channel_TypeDef*)spidev->dma;

    dma->CPAR = (uint32_t)&spi->DR;     // Peripheral source

    dma->CCR =
            DMA_CCR_PL_0 |              // Medium priority
            DMA_CCR_MSIZE_0 |           // 16bit Dst size
			DMA_CCR_PSIZE_0 |           // 16bit src size
            DMA_CCR_MINC |              // Enable memory increment
            DMA_CCR_DIR |               // Write to peripheral
			DMA_CCR_TCIE;               // Enable Transfer Complete interrupt

    NVIC_EnableIRQ(irq);
}

/**
 * @brief Make single data extange on spi bus
 *
 * \param spidev : Pointer to spi device to be used
 * \param data  : Data to be transmitted
 *
 * \return Received data
 * */
uint16_t SPI_Single_Transfer(spidev_t *spidev, uint16_t data){
    SPI_TypeDef *spi = (SPI_TypeDef*)spidev->ctrl;

    *((__IO uint8_t *)&spi->DR) = data;
    while((spi->SR & SPI_SR_TXE) == 0);
    while((spi->SR & SPI_SR_BSY) != 0);

    return spi->DR;
}

/**
 * @brief Write data to SPI, blocking
 * 
 * \param src   : Pointer to source data
 * \param count : total number of bytes to transfer
 * */
void SPI_Write(spidev_t *spidev, uint8_t *src, uint32_t count){
    SPI_TypeDef *spi = (SPI_TypeDef*)spidev->ctrl;
    
    while(count--){
        *((__IO uint8_t *)&spi->DR) = *src++;
        while((spi->SR & SPI_SR_TXE) == 0);
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
    DMA_Channel_TypeDef *dma = (DMA_Channel_TypeDef*)spidev->dma;

    // Configure Spi for 16bit DMA
    spi->CR1 &= ~SPI_CR1_SPE;
    spi->CR1 |= SPI_CR1_DFF | SPI_CR1_SPE;

    if(spidev->cfg & SPI_DMA_NO_MINC){
        dma->CCR &= ~(DMA_CCR_MINC);
    }else{
        dma->CCR |= DMA_CCR_MINC;
    }

    spi->CR2 |= SPI_CR2_TXDMAEN;

    spidev->trf_counter = count;

    dma->CMAR = (uint32_t)src;
    dma->CNDTR = (spidev->trf_counter > 0x10000) ? 0xFFFF : spidev->trf_counter;
    
    dma->CCR |= DMA_CCR_EN;

    SPIDEV_SET_FLAG(spidev, SPI_BUSY);
}

/**
 * @brief Write repeated integer data to SPI using DMA
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
    #if 0
    SPI_TypeDef *spi = (SPI_TypeDef*)spidev->ctrl;
    while(spi->SR & SPI_SR_BSY){
    #else
    while(SPIDEV_GET_FLAG(spidev, SPI_BUSY)){
    #endif
        //LED_TOGGLE;
    }
}

