#include <stdint.h>
#include <stddef.h>
#include "stm32l4xx.h"
#include "dma_stm32l4xx.h"
#include "dma.h"

static dmatype_t *hdma[DMA_NUM_CHANNELS];

/**
 * @brief Request dma controller/channel
 * \param ctrl    : pointer to hold controller and channel
 * \param request : Request in format
 *                   [9:8] -> DMA number
 *                   [7:4] -> Channel number
 *                   [3:0] -> Request source
 * */
uint32_t DMA_Config(dmatype_t *dma, uint32_t request)
{
    DMA_Channel_TypeDef *stream;
    // DMA number
    uint8_t dma_num = (request &  DMA_NUMBER_MASK) >> DMA_NUMBER_POS;
    // Channel1 to 7
    uint8_t ch_num = (request & DMA_CHANNEL_MASK) >> DMA_CHANNEL_POS;
    // CxS[3:0]
    uint8_t source = (request & DMA_SOURCE_MASK) >> DMA_SOURCE_POS;
    uint8_t irq;

    if(hdma[ch_num] && hdma[ch_num] != dma){
        // request is already in use
        return 0;
    }

    if(dma->handle == NULL){
        if(dma_num == 0){
            RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
            stream = (DMA_Channel_TypeDef*)((uint32_t)DMA1_Channel1 + (ch_num * 0x14));
            irq = DMA1_Channel1_IRQn + ch_num;
            DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~(15 << (ch_num << 2))) | (source << (ch_num << 2));
        }else{
            RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
            stream = (DMA_Channel_TypeDef*)((uint32_t)DMA2_Channel1 + (ch_num * 0x14));
            irq = (ch_num < 5) ? DMA2_Channel1_IRQn + ch_num : DMA2_Channel6_IRQn + ch_num;
            DMA2_CSELR->CSELR = (DMA2_CSELR->CSELR & ~(15 << (ch_num << 2))) | (source << (ch_num << 2));
        }

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
                            DMA_CCR_PINC;
                stream->CPAR = (uint32_t)dma->src;
                stream->CMAR = (uint32_t)dma->dst;
                break;

            case DMA_DIR_M2P:
                config |=   DMA_CCR_PSIZE_CFG(dma->dsize) |
                            DMA_CCR_MSIZE_CFG(dma->ssize) |
                            DMA_CCR_MINC | DMA_CCR_DIR;
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

        // enable terminal count
        config |= DMA_CCR_TCIE;

        if(dma->half){
            config |= DMA_CCR_HTIE;
        }

        NVIC_EnableIRQ(irq);

        stream->CCR = config;
        dma->handle = stream;
    }

    hdma[(dma_num * DMA1_MAX_CHANNELS) + ch_num] = dma;

    return 1;
}

void DMA_Start(dmatype_t *dma)
{
    DMA_Channel_TypeDef *stream = dma->handle;
    uint32_t cfg = stream->CCR & ~(DMA_CCR_MINC | DMA_CCR_CIRC);

    stream->CNDTR = dma->len;

    if(!dma->single){
        cfg |= DMA_CCR_CIRC;
    }

    if(dma->dir != DMA_DIR_P2P){
        cfg |= DMA_CCR_MINC;
    }

    stream->CCR = cfg | DMA_CCR_EN;
}

void DMA_Cancel(dmatype_t *dma)
{
    DMA_Channel_TypeDef *stream = dma->handle;

    stream->CCR &= ~DMA_CCR_EN;
}

static void dma_irq_handler(dmatype_t *dma)
{
    DMA_Channel_TypeDef *stream = dma->handle;
    uint32_t ccr = stream->CCR;

    if(!(ccr & DMA_CCR_CIRC)){
        // Disable stream if circular mode is disabled
        stream->CCR = ccr & ~DMA_CCR_EN;
    }

    if(dma->eot != NULL){
        dma->eot();
    }
}

void DMA1_Channel1_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_GIF1){
        dma_irq_handler(hdma[0]);
        DMA1->IFCR = (DMA_IFCR_CGIF1 | DMA_IFCR_CTCIF1 | DMA_IFCR_CTEIF1);
    }
}

void DMA1_Channel2_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_GIF2){
        dma_irq_handler(hdma[1]);
        DMA1->IFCR = (DMA_IFCR_CGIF2 | DMA_IFCR_CTCIF2 | DMA_IFCR_CTEIF2);
    }
}

void DMA1_Channel3_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_GIF3){
        dma_irq_handler(hdma[2]);
        DMA1->IFCR = (DMA_IFCR_CGIF3 | DMA_IFCR_CTCIF3 | DMA_IFCR_CTEIF3);
    }
}

void DMA1_Channel4_IRQHandler(void){
    if(DMA1->ISR & DMA_ISR_GIF4){
        dma_irq_handler(hdma[3]);
        DMA1->IFCR = (DMA_IFCR_CGIF4 | DMA_IFCR_CTCIF4 | DMA_IFCR_CTEIF4);
    }
}

void DMA1_Channel5_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_GIF5){
        dma_irq_handler(hdma[4]);
        DMA1->IFCR = (DMA_IFCR_CGIF5 | DMA_IFCR_CTCIF5 | DMA_IFCR_CTEIF5);
    }
}

void DMA1_Channel6_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_GIF6){
        dma_irq_handler(hdma[5]);
        DMA1->IFCR = (DMA_IFCR_CGIF6 | DMA_IFCR_CTCIF6 | DMA_IFCR_CTEIF6);
    }
}

void DMA1_Channel7_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_GIF7){
        dma_irq_handler(hdma[6]);
        DMA1->IFCR = (DMA_IFCR_CGIF7 | DMA_IFCR_CTCIF7 | DMA_IFCR_CTEIF7);
    }
}

void DMA2_Channel1_IRQHandler(void)
{
    if (DMA2->ISR & DMA_ISR_TCIF1){
        dma_irq_handler(hdma[(1 * DMA1_MAX_CHANNELS) + 0]);
    }
    DMA2->IFCR = (DMA_IFCR_CGIF1 | DMA_IFCR_CTCIF1 | DMA_IFCR_CTEIF1);
}

void DMA2_Channel2_IRQHandler(void)
{
    if (DMA2->ISR & DMA_ISR_TCIF2){
        dma_irq_handler(hdma[(1 * DMA1_MAX_CHANNELS) + 1]);
    }
    DMA2->IFCR = (DMA_IFCR_CGIF2 | DMA_IFCR_CTCIF2 | DMA_IFCR_CTEIF2);
}

void DMA2_Channel3_IRQHandler(void)
{
    if (DMA2->ISR & DMA_ISR_TCIF3){
        dma_irq_handler(hdma[(1 * DMA1_MAX_CHANNELS) + 2]);
    }
    DMA2->IFCR = (DMA_IFCR_CGIF3 | DMA_IFCR_CTCIF3 | DMA_IFCR_CTEIF3);
}

void DMA2_Channel4_IRQHandler(void)
{
    if (DMA2->ISR & DMA_ISR_TCIF4){
        dma_irq_handler(hdma[(1 * DMA1_MAX_CHANNELS) + 3]);
    }
    DMA2->IFCR = (DMA_IFCR_CGIF4 | DMA_IFCR_CTCIF4 | DMA_IFCR_CTEIF4);;
}

void DMA2_Channel5_IRQHandler(void)
{
    if (DMA2->ISR & DMA_ISR_TCIF5){
        dma_irq_handler(hdma[(0 * DMA1_MAX_CHANNELS) + 4]);
    }
    DMA2->IFCR = (DMA_IFCR_CGIF5 | DMA_IFCR_CTCIF5 | DMA_IFCR_CTEIF5);
}

void DMA2_Channel6_IRQHandler(void)
{
    if (DMA2->ISR & DMA_ISR_TCIF6){
        dma_irq_handler(hdma[(0 * DMA1_MAX_CHANNELS) + 5]);
    }
    DMA2->IFCR = (DMA_IFCR_CGIF6 | DMA_IFCR_CTCIF6 | DMA_IFCR_CTEIF6);
}

void DMA2_Channel7_IRQHandler(void)
{
    if (DMA2->ISR & DMA_ISR_TCIF7){
        dma_irq_handler(hdma[(0 * DMA1_MAX_CHANNELS) + 6]);
    }
    DMA2->IFCR = (DMA_IFCR_CGIF7 | DMA_IFCR_CTCIF7 | DMA_IFCR_CTEIF7);
}