#include <stdint.h>
#include "stm32l4xx.h"
#include "dma_stm32l4xx.h"
#include "dma.h"

typedef void (*eot_t)(void);
static eot_t ch_eot[DMA_MAX_CHANNELS];

/**
 * @brief Request dma controller/channel
 * \param ctrl    : pointer to hold controller and channel
 * \param request : Request in format
 *                   [9:8] -> DMA number
 *                   [7:4] -> Channel number
 *                   [3:0] -> Request source
 * */
void DMA_Config(dmatype_t *dma, uint32_t request){
    DMA_Channel_TypeDef *stream; 
    uint8_t ch_num = (request & DMA_CHANNEL_MASK) >> DMA_CHANNEL_POS;
    uint8_t source = (request & DMA_SOURCE_MASK) >> DMA_SOURCE_POS;
    uint8_t irq;

    request = (request &  DMA_NUMBER_MASK) >> DMA_NUMBER_POS;

    if(dma->stream == NULL){    
        if(request == 0){
            __HAL_RCC_DMA1_CLK_ENABLE();
            dma->per = DMA1;
            stream = (DMA_Channel_TypeDef*)((uint32_t)DMA1_Channel1 + (ch_num * 0x14));
            irq = DMA1_Channel1_IRQn + ch_num;
            DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~(15 << (ch_num << 2))) | (source << (ch_num << 2));
        }else{
            __HAL_RCC_DMA2_CLK_ENABLE();
            dma->per = DMA2;
            stream = (DMA_Channel_TypeDef*)((uint32_t)DMA2_Channel1 + (ch_num * 0x14));
            irq = (ch_num < 5) ? DMA2_Channel1_IRQn + ch_num : DMA2_Channel6_IRQn + ch_num;
            DMA2_CSELR->CSELR = (DMA2_CSELR->CSELR & ~(15 << (ch_num << 2))) | (source << (ch_num << 2));
        }

        uint32_t config = 0;

        switch(dma->dir){
            case DMA_DIR_P2P:
                config |=   DMA_CCR_MSIZE_CFG(dma->ssize) | DMA_CCR_PSIZE_CFG(dma->dsize);
                break;

            case DMA_DIR_P2M:
                config |=   DMA_CCR_PSIZE_CFG(dma->ssize) | 
                            DMA_CCR_MSIZE_CFG(dma->dsize) |
                            DMA_CCR_PINC;                   // Increment destination
                break;

            case DMA_DIR_M2P:
                config |=   DMA_CCR_PSIZE_CFG(dma->dsize) | 
                            DMA_CCR_MSIZE_CFG(dma->ssize) | 
                            DMA_CCR_MINC | DMA_CCR_DIR;     // Increment source
                break;

            case DMA_DIR_M2M:
                config |=   DMA_CCR_PSIZE_CFG(dma->dsize) | 
                            DMA_CCR_MSIZE_CFG(dma->ssize) | 
                            DMA_CCR_MEM2MEM | DMA_CCR_MINC | DMA_CCR_PINC;
                break;
        }

        if(dma->eot){
            config |= DMA_CCR_TCIE;  // enable terminal count
            HAL_NVIC_EnableIRQ(irq);
        }

        stream->CCR = config;
        stream->CPAR = (uint32_t)dma->dst;
        dma->stream = stream;
    }

    ch_eot[(request * DMA1_MAX_CHANNELS) + ch_num] = dma->eot;
}

static inline void callHandler(uint8_t ch_num){
    if(ch_eot[ch_num] != NULL){
        ch_eot[ch_num]();
    }
}

void DMA1_Channel2_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_GIF2){
        callHandler(1);
        DMA1->IFCR = (DMA_IFCR_CGIF2 | DMA_IFCR_CTCIF2 | DMA_IFCR_CTEIF2);
    }
}

void DMA1_Channel4_IRQHandler(void){
    if(DMA1->ISR & DMA_ISR_GIF4){
        callHandler(3);
        DMA1->IFCR = (DMA_IFCR_CGIF4 | DMA_IFCR_CTCIF4 | DMA_IFCR_CTEIF4);
    }
}

void DMA2_Channel4_IRQHandler(void)
{
    if (DMA2->ISR & DMA_ISR_TCIF4){
        callHandler((1 * DMA1_MAX_CHANNELS) + 3);
    }
    DMA2->IFCR = DMA_IFCR_CGIF4;    
}