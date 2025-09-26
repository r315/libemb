#include <stdint.h>
#include <stddef.h>
#include "stm32f1xx.h"
#include "dma_stm32f1xx.h"
#include "dma.h"

static dmatype_t *hdma[DMA_NUM_CHANNELS];

/**
 * @brief Request dma controller/channel
 *
 * \param dma     : dma instance
 * \param request : Request in format
 *                   [7:5] -> unused
 *                   [4:4] -> DMA number
 *                   [3:0] -> Channel number
 * \return 1: if request suceeded, 0: otherwise
 * */
void DMA_Config(dmatype_t *dma, uint32_t request){
    DMA_Channel_TypeDef *stream;
    uint8_t ch_num;

    if((request & DMA_NUMBER_MASK)){
        return 0; // only DMA1
    }

        RCC->AHBENR |= RCC_AHBENR_DMA1EN;
        dma->per = DMA1;

    ch_num = (request & DMA_CHANNEL_MASK) >> DMA_CHANNEL_POS;

    if(hdma[ch_num] && hdma[ch_num] != dma){
        return 0; // request is already in use
    }

    stream = (DMA_Channel_TypeDef*)((uint32_t)DMA1_Channel1 + (ch_num * (sizeof(DMA_Channel_TypeDef) + 4)));

    uint32_t config = 0;

    switch(dma->dir){
        case DMA_DIR_P2P:
            config |=   DMA_CCR_MSIZE_CFG(dma->ssize) | DMA_CCR_PSIZE_CFG(dma->dsize);
            stream->CPAR = (uint32_t)dma->src;
            stream->CMAR = (uint32_t)dma->dst;
            break;

        case DMA_DIR_P2M:
            config |=   DMA_CCR_PSIZE_CFG(dma->ssize) |
                        DMA_CCR_MSIZE_CFG(dma->dsize) |
                        DMA_CCR_MINC;                   // Increment destination
            stream->CPAR = (uint32_t)dma->src;
            stream->CMAR = (uint32_t)dma->dst;
            break;

        case DMA_DIR_M2P:
            config |=   DMA_CCR_PSIZE_CFG(dma->dsize) |
                        DMA_CCR_MSIZE_CFG(dma->ssize) |
                        DMA_CCR_MINC | DMA_CCR_DIR;    // Increment source
            stream->CMAR = (uint32_t)dma->src;
            stream->CPAR = (uint32_t)dma->dst;
            break;

        case DMA_DIR_M2M:
            config |=   DMA_CCR_PSIZE_CFG(dma->dsize) |
                        DMA_CCR_MSIZE_CFG(dma->ssize) |
                        DMA_CCR_MEM2MEM | DMA_CCR_MINC | DMA_CCR_PINC;
            stream->CPAR = (uint32_t)dma->src;
            stream->CMAR = (uint32_t)dma->dst;
            break;
    }
    // Enable interrupt for eot
        config |= DMA_CCR_TCIE;
        NVIC_EnableIRQ(DMA1_Channel1_IRQn + ch_num);

    stream->CCR = config;
    dma->stream = stream;

    hdma[ch_num] = dma;

}

void DMA_Start(dmatype_t *dma)
{
    DMA_Channel_TypeDef *stream = dma->stream;
    stream->CNDTR = dma->len;
    stream->CCR |= DMA_CCR_EN;
}

void DMA_Cancel(dmatype_t *dma)
{
    DMA_Channel_TypeDef *stream = dma->stream;
    stream->CCR &= ~DMA_CCR_EN;
}

uint32_t DMA_GetTransfers(dmatype_t *dma)
{
    DMA_Channel_TypeDef *stream = dma->stream;
    return dma->len - stream->CNDTR;
}


static void dma_irq_handler(dmatype_t *dma)
{
    DMA_Channel_TypeDef *stream = dma->stream;
    uint32_t ccr = stream->CCR;

    if(!(ccr & DMA_CCR_CIRC)){
        // Disable stream if circular mode is disabled
        stream->CCR = ccr & ~DMA_CCR_EN;
    }

    if(dma->eot != NULL){
        dma->eot();
    }
}

void DMA1_Channel1_IRQHandler(void){
    dma_irq_handler(hdma[0]);
    DMA1->IFCR = DMA1->ISR & (DMA_IFCR_CGIF1 | DMA_IFCR_CTCIF1 | DMA_IFCR_CHTIF1);
}
void DMA1_Channel2_IRQHandler(void){
    dma_irq_handler(hdma[1]);
    DMA1->IFCR = DMA1->ISR & (DMA_IFCR_CGIF2 | DMA_IFCR_CTCIF2 | DMA_IFCR_CHTIF2);
}
void DMA1_Channel3_IRQHandler(void){
    dma_irq_handler(hdma[2]);
    DMA1->IFCR = DMA1->ISR & (DMA_IFCR_CGIF3 | DMA_IFCR_CTCIF3 | DMA_IFCR_CHTIF3);
}
void DMA1_Channel4_IRQHandler(void){
    dma_irq_handler(hdma[3]);
    DMA1->IFCR = DMA1->ISR & (DMA_IFCR_CGIF4 | DMA_IFCR_CTCIF4 | DMA_IFCR_CHTIF4);
}
void DMA1_Channel5_IRQHandler(void){
    dma_irq_handler(hdma[4]);
    DMA1->IFCR = DMA1->ISR & (DMA_IFCR_CGIF5 | DMA_IFCR_CTCIF5 | DMA_IFCR_CHTIF5);
}
void DMA1_Channel6_IRQHandler(void){
    dma_irq_handler(hdma[5]);
    DMA1->IFCR = DMA1->ISR & (DMA_IFCR_CGIF6 | DMA_IFCR_CTCIF6 | DMA_IFCR_CHTIF6);
}
void DMA1_Channel7_IRQHandler(void){
    dma_irq_handler(hdma[6]);
    DMA1->IFCR = DMA1->ISR & (DMA_IFCR_CGIF7 | DMA_IFCR_CTCIF7 | DMA_IFCR_CHTIF7);
}

