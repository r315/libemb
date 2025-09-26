#include <stddef.h>
#include "stm32l4xx.h"
#include "dma_stm32l4xx.h"
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

typedef struct{
    SPI_TypeDef *spi;
    dmatype_t dma_tx;
    void (*eot)(void);          // User end of transfer call back
    uint32_t trf_counter;       // Transfer counter, used when data so be transferred is greater than 65535
    uint16_t data_word;
}hspi_t;

static hspi_t hspia = {
    .spi = SPI1,
    .trf_counter = 0,
    .eot = NULL
}, hspib = {
    .spi = SPI2,
    .trf_counter = 0,
    .eot = NULL
};

/**
 * @brief DMA Interrupt handler
 * */
void SPI_DMA_IRQHandler(hspi_t *hspi)
{
    SPI_TypeDef *spi = hspi->spi;
    DMA_Channel_TypeDef *dma_channel = (DMA_Channel_TypeDef*)hspi->dma_tx.stream;

    if(hspi->trf_counter > 0x10000UL){
        hspi->trf_counter -= 0x10000UL;
        dma_channel->CNDTR = (hspi->trf_counter > 0x10000UL) ? 0xFFFFUL : hspi->trf_counter;
        dma_channel->CCR |= DMA_CCR_EN;
    }else{
        if(spi->SR & SPI_SR_OVR){
            //dummy read for clearing OVR flag
            hspi->trf_counter = spi->DR;
        }

        hspi->trf_counter = 0;

        if(hspi->eot){
            hspi->eot();
        }
    }
}

/**
 * @brief Calculate divisor values and configure spi speed
 *
 * @param spi       spi instance
 * @param freq      desired frequency in kHz
 */
static void SPI_SetFreq(SPI_TypeDef *spi, uint32_t freq)
{
    uint32_t div = (SystemCoreClock/1000)/freq;
    uint32_t br = 8;

    // Constrain divisor
    if(div > 256){ div = 256; }
    if(div < 2){ div = 2; }

    while(br){
        if(div & (uint32_t)(1 << br)){
            break;
        }
        br--;
    }

    spi->CR1 &= ~(SPI_CR1_SPE | PCLK_CLK_DIV256);
    spi->CR1 |= (br << SPI_CR1_BR_Pos);
}

static inline void spi1Eot(void){ SPI_DMA_IRQHandler(&hspia);}
static inline void spi2Eot(void){ SPI_DMA_IRQHandler(&hspib);}

/**
 * @brief Configures spi peripheral according
 * spibus parameters. GPIO pins must be configured
 * by application (I was lazy to implement here)
 * */
uint32_t SPI_Init(spibus_t *spibus){
    hspi_t *hspi;

    switch(spibus->bus){
        case SPI_BUS0:
            RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
            hspi = &hspia;
            break;

        case SPI_BUS1:
            RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;
            hspi = &hspib;
            break;

        default :
            return SPI_ERR_PARM;
    }

    hspi->spi->CR1 = SPI_CR1_MSTR;
    hspi->spi->CR2 = (7 << SPI_CR2_DS_Pos);        // Transfer 8-bit

    SPI_SetFreq(hspi->spi, spibus->freq);

    if(spibus->cfg & SPI_CFG_CS){
        hspi->spi->CR2 |= SPI_CR2_NSSP | SPI_CR2_SSOE;
    }else{
        hspi->spi->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    }

    hspi->spi->CR1 |= SPI_CR1_SPE;

    hspi->dma_tx.dst = (void*)&hspi->spi->DR;
    hspi->dma_tx.dsize = DMA_CCR_PSIZE_16;
    hspi->dma_tx.src = NULL;
    hspi->dma_tx.ssize = DMA_CCR_MSIZE_16;
    hspi->dma_tx.dir = DMA_DIR_M2P;

    if(hspi->spi == SPI1){
        hspi->dma_tx.eot = spi1Eot;
        DMA_Config(&hspi->dma_tx, DMA2_REQ_SPI1_TX); // Use DMA2 to avoid conflict with usart
    }else{
        hspi->dma_tx.eot = spi2Eot;
        DMA_Config(&hspi->dma_tx, DMA1_REQ_SPI2_TX);
    }

    spibus->handle = hspi;

    hspi->spi->CR2 |= SPI_CR2_TXDMAEN;

    return SPI_OK;
}

/**
 * @brief Write data to SPI
 *
 * \param data  : Pointer to data
 * \param count : total number of bytes to transfer
 * */
void SPI_Transfer(spibus_t *spibus, uint8_t *src, uint32_t count)
{
    SPI_TypeDef *spi = ((hspi_t*)spibus->handle)->spi;

    if(spibus->cfg & SPI_CFG_TRF_16BIT){
        spi->CR2 |= SPI_CR2_DS_3;       // 16-bit
        while(count--){
            *((__IO uint16_t *)&spi->DR) = *(uint16_t*)src++;
            while((spi->SR & SPI_SR_BSY) != 0);
        }
    }else{
         spi->CR2 &= ~SPI_CR2_DS;        // Invalid forced 8-bit
        while(count--){
            *((__IO uint8_t *)&spi->DR) = *src++;
            while((spi->SR & SPI_SR_BSY) != 0);
        }
    }
}

/**
 * @brief Write data to SPI using DMA controller
 *
 * \param data  : Pointer to data
 * \param count : total number of transfers
 * */
void SPI_TransferDMA(spibus_t *spibus, uint8_t *src, uint32_t count)
{
    hspi_t *hspi = (hspi_t*)spibus->handle;
    SPI_TypeDef *spi = hspi->spi;
    DMA_Channel_TypeDef *dma_channel = hspi->dma_tx.stream;

    if(spibus->cfg & SPI_CFG_TRF_16BIT){
        spi->CR2 |= SPI_CR2_DS_3;       // 16-bit
        hspi->data_word = *(uint16_t*)src;
    }else{
        spi->CR2 &= ~SPI_CR2_DS;        // Invalid forced 8-bit
        hspi->data_word = *(uint8_t*)src;
    }

    if(spibus->cfg & SPI_CFG_TRF_CONST){
        src = (uint8_t*)&hspi->data_word;
        dma_channel->CCR &= ~(DMA_CCR_MINC);
    }else{
        dma_channel->CCR |= DMA_CCR_MINC;
    }

    hspi->trf_counter = count;
    dma_channel->CMAR = (uint32_t)src;
    dma_channel->CNDTR = (hspi->trf_counter > 0x10000) ? 0xFFFF : hspi->trf_counter;

    dma_channel->CCR |= DMA_CCR_EN;
}

/**
 * @brief Wait for end of DMA transfer
 * */
void SPI_WaitEOT(spibus_t *spibus)
{
#if 1
    SPI_TypeDef *spi = ((hspi_t*)spibus->handle)->spi;
    while(spi->SR & SPI_SR_BSY){
#else
    while(SPIDEV_GET_FLAG(spibus, SPI_BUSY)){
#endif
    //LED_TOGGLE;
    }
}

/**
 * @brief Configure a end of transfer callback
 * @param spibus
 * @param eot
 */
void SPI_SetEOT(spibus_t *spibus, void(*eot)(void))
{
    hspi_t *hspi = spibus->handle;
    hspi->eot = eot;
}
