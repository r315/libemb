#include <stddef.h>
#include "stm32f1xx.h"
#include "dma_stm32f1xx.h"
#include "spi.h"
#include "dma.h"
#include "clock.h"

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
static void SPI_DMA_IRQHandler(hspi_t *hspi)
{
    DMA_Channel_TypeDef *dma = hspi->dma_tx.per;

    if(hspi->trf_counter > 0x10000UL){
        hspi->trf_counter -= 0x10000UL;
        dma->CNDTR = (hspi->trf_counter > 0x10000UL) ? 0xFFFFUL : hspi->trf_counter;
        dma->CCR |= DMA_CCR_EN;
    }else{
        if(hspi->spi->SR & SPI_SR_OVR){
            //dummy read for clearing OVR flag
            hspi->trf_counter = hspi->spi->DR;
        }

        hspi->spi->CR2 &= ~(SPI_CR2_TXDMAEN);

        hspi->trf_counter = 0;

        if(hspi->eot){
            hspi->eot();
        }
    }
}

static inline void spi1Eot(void){ SPI_DMA_IRQHandler(&hspia);}
static inline void spi2Eot(void){ SPI_DMA_IRQHandler(&hspib);}

/**
 * @brief Calculate divisore values and configure spi speed
 *
 * @param spi       spi instance
 * @param pclk      spi peripheral clock
 * @param freq      desired frequency in kHz
 */
static void SPI_SetFreq(SPI_TypeDef *spi, uint32_t pclk, uint32_t freq)
{
    uint32_t div = (pclk/1000)/freq;  // Get estimated divisor
    uint32_t br = 8;

    // Constrain divisor
    if(div > 256){ div = 256; }
    if(div < 2){ div = 2; }

    // Caculate correct power, should be one
    // in {2, 4, 8, 16, 32, 64, 128 256] list
    do{
        if(div >= (uint32_t)(2 << br)){
            break;
        }
        br--;
    }while(br);

    spi->CR1 &= ~(SPI_CR1_SPE | PCLK_CLK_DIV256);
    spi->CR1 |= (br << SPI_CR1_BR_Pos);
}

/**
 * @brief Configures spi peripheral according
 * spibus parameters. GPIO pins must be configured
 * by application (I was lazy to implement here)
 * */
uint32_t SPI_Init(spibus_t *spibus)
{
    hspi_t *hspi;
    uint32_t pclk;

    switch(spibus->bus){
        case SPI_BUS0:
            RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
            RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
            RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
            pclk = CLOCK_Get(CLOCK_CLK2);
            hspi = &hspia;
            break;

        case SPI_BUS1:
            RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
            RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST;
            RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;
            pclk = CLOCK_Get(CLOCK_CLK1);
            hspi = &hspib;
            break;

        default :
            return SPI_ERR_PARM;
    }

    hspi->spi->CR1 = SPI_CR1_MSTR;

    SPI_SetFreq(hspi->spi, pclk, spibus->freq);

    if((spibus->cfg & SPI_CFG_CS) == 0){
        // In master mode SSOE must be enable for NSS
        // software control
        hspi->spi->CR2 |=  SPI_CR2_SSOE;
    }

    hspi->spi->CR1 |= SPI_CR1_SPE;

    if(spibus->cfg & SPI_CFG_DMA){
        hspi->dma_tx.dst = (void*)&hspi->spi->DR;
        hspi->dma_tx.dsize = DMA_CCR_PSIZE_16;
        hspi->dma_tx.src = NULL;
        hspi->dma_tx.ssize = DMA_CCR_MSIZE_16;
        hspi->dma_tx.dir = DMA_DIR_M2P;

        if(hspi->spi == SPI1){
            hspi->dma_tx.eot = spi1Eot;
            DMA_Config(&hspi->dma_tx, DMA1_REQ_SPI1_TX);
        }else{
            hspi->dma_tx.eot = spi2Eot;
            DMA_Config(&hspi->dma_tx, DMA1_REQ_SPI2_TX);
        }
    }

    spibus->handle = hspi;

    return SPI_OK;
}

/**
 * @brief Make single data exchange on spi bus
 *
 * \param spibus : Pointer to spi device to be used
 * \param data  : Data to be transmitted
 *
 * \return Received data
 * */
uint32_t SPI_Xchg(spibus_t *spibus, uint8_t *buffer, uint32_t count)
{
    SPI_TypeDef *spi = ((hspi_t*)spibus->handle)->spi;
    uint32_t total = count;

    if(spibus->cfg & SPI_CFG_TRF_16BIT){
        spi->CR1 |= SPI_CR1_DFF;
        while(count--){
            while((spi->SR & SPI_SR_TXE) == 0);
            *((__IO uint16_t *)&spi->DR) = *(uint16_t*)buffer;
            while((spi->SR & SPI_SR_BSY) != 0);
            *buffer = *((__IO uint16_t *)&spi->DR);
            buffer++;
        }
    }else{
        spi->CR1 &= ~SPI_CR1_DFF;
        while(count--){
            while((spi->SR & SPI_SR_TXE) == 0);
            *((__IO uint8_t *)&spi->DR) = *buffer;
            while((spi->SR & SPI_SR_BSY) != 0);
            *buffer = *((__IO uint8_t *)&spi->DR);
        }
    }

    return total - count;
}

/**
 * @brief Write data to SPI, blocking
 *
 * \param src   : Pointer to source data
 * \param count : total number of bytes to transfer
 * */
void SPI_Transfer(spibus_t *spibus, const uint8_t *src, uint32_t count)
{
    SPI_TypeDef *spi = ((hspi_t*)spibus->handle)->spi;

    if(spibus->cfg & SPI_CFG_TRF_16BIT){
        spi->CR1 |= SPI_CR1_DFF;
        while(count--){
            *((__IO uint16_t *)&spi->DR) = *(uint16_t*)src++;
            while((spi->SR & SPI_SR_TXE) == 0);
            while((spi->SR & SPI_SR_BSY) != 0);
        }
    }else{
        spi->CR1 &= ~SPI_CR1_DFF;
        while(count--){
            *((__IO uint8_t *)&spi->DR) = *src++;
            while((spi->SR & SPI_SR_TXE) == 0);
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
void SPI_TransferDMA(spibus_t *spibus, const uint8_t *src, uint32_t count)
{
    hspi_t *hspi = (hspi_t*)spibus->handle;
    SPI_TypeDef *spi = hspi->spi;
    DMA_Channel_TypeDef *dma = (DMA_Channel_TypeDef*)hspi->dma_tx.stream;

    if(spibus->cfg & SPI_CFG_TRF_16BIT){
        spi->CR1 |= SPI_CR1_DFF;
    }else{
        spi->CR1 &= ~SPI_CR1_DFF;
    }

    if(spibus->cfg & SPI_CFG_TRF_CONST){
        hspi->data_word = *(uint16_t*)src;  // get constant value
        src = (uint8_t*)&hspi->data_word;   // set source pointer to constant value variable
        dma->CCR &= ~(DMA_CCR_MINC);
    }else{
        dma->CCR |= DMA_CCR_MINC;
    }

    spi->CR2 |= SPI_CR2_TXDMAEN;

    hspi->trf_counter = count;

    dma->CMAR = (uint32_t)src;
    dma->CNDTR = (hspi->trf_counter > 0x10000) ? 0xFFFF : hspi->trf_counter;

    dma->CCR |= DMA_CCR_EN;
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
    while(spibus_GET_FLAG(spibus, SPI_BUSY)){
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
    if(spibus){
        ((hspi_t*)spibus->handle)->eot = eot;
    }
}
