#include <stdint.h>
#include <stddef.h>
#include "at32f4xx.h"
#include "dma_at32f4xx.h"
#include "dma.h"



typedef void (*eot_t)(void);
static eot_t ch_eot[DMA_NUM_CHANNELS];

/**
 * @brief Request dma controller/channel
 * \param ctrl    : pointer to hold controller and channel
 * \param request : Request in format
 *                   [9:8] -> DMA number
 *                   [7:4] -> Channel number
 *                   [3:0] -> Request source
 * */
void DMA_Config(dmatype_t *dma, uint32_t request){
    DMA_Channel_Type *stream;
    uint8_t ch_num;
    IRQn_Type irqn;
    
    ch_num = (request & DMA_CHANNEL_MASK) >> DMA_CHANNEL_POS;
    

    if((request & DMA_NUMBER_MASK) == 0){
        RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
        dma->per = DMA1;
        irqn = DMA1_Channel1_IRQn + ch_num;
        stream = (DMA_Channel_Type*)((uint32_t)DMA1_Channel1 + (ch_num * 0x14));
    }else{
        RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA2, ENABLE);
        dma->per = DMA2;
        irqn = DMA2_Channel1_IRQn + ((ch_num == 4) ? 3 : ch_num);
        stream = (DMA_Channel_Type*)((uint32_t)DMA2_Channel1 + (ch_num * 0x14));
    }

    uint32_t config = 0;

    switch(dma->dir){
        case DMA_DIR_P2P:
            config |=   DMA_CCR_MSIZE_CFG(dma->ssize) | DMA_CCR_PSIZE_CFG(dma->dsize);
            stream->CPBA = (uint32_t)dma->src;
            stream->CMBA = (uint32_t)dma->dst;
            break;

        case DMA_DIR_P2M:
            config |=   DMA_CCR_PSIZE_CFG(dma->ssize) | 
                        DMA_CCR_MSIZE_CFG(dma->dsize) |
                        DMA_CHCTRL1_PINC;                   // Increment destination
            stream->CPBA = (uint32_t)dma->src;
            stream->CMBA = (uint32_t)dma->dst;
            break;

        case DMA_DIR_M2P:
            config |=   DMA_CCR_PSIZE_CFG(dma->dsize) | 
                        DMA_CCR_MSIZE_CFG(dma->ssize) | 
                        DMA_CHCTRL1_MINC | DMA_CHCTRL1_DIR; // Increment source
            stream->CMBA = (uint32_t)dma->src;
            stream->CPBA = (uint32_t)dma->dst;
            break;

        case DMA_DIR_M2M:
            config |=   DMA_CCR_PSIZE_CFG(dma->dsize) | 
                        DMA_CCR_MSIZE_CFG(dma->ssize) | 
                        DMA_CHCTRL1_MEMTOMEM | DMA_CHCTRL1_MINC | DMA_CHCTRL1_PINC;
            stream->CPBA = (uint32_t)dma->src;
            stream->CMBA = (uint32_t)dma->dst;
            break;
    }

    if(dma->eot){
        config |= DMA_CHCTRL1_TCIE;
        NVIC_EnableIRQ(irqn);
    }

    if(!dma->single){
        config |= DMA_CHCTRL1_CIRM;
    }

    stream->CHCTRL = config;        
    dma->stream = stream;
    
    ch_eot[ch_num] = dma->eot;
}

void DMA_Start(dmatype_t *dma)
{
    DMA_Channel_Type *stream = dma->stream;

    stream->TCNT = dma->len;
    stream->CHCTRL |= DMA_CHCTRL1_CHEN;
}

void DMA_Cancel(dmatype_t *dma)
{
    DMA_Channel_Type *stream = dma->stream;

    stream->CHCTRL &= ~DMA_CHCTRL1_CHEN;
}

static inline void dma_irq_handler(uint8_t ch_num)
{
    if(ch_eot[ch_num] != NULL){
        ch_eot[ch_num]();
    }
}

void DMA1_Channel1_IRQHandler(void)
{ 
    if(DMA1->ISTS & DMA_ISTS_GIF1){
        dma_irq_handler(0);
        DMA1->ICLR = (DMA_ICLR_CGIF1 | DMA_ICLR_CTCIF1 | DMA_ICLR_CERRIF1);
    }
}

void DMA1_Channel2_IRQHandler(void)
{ 
    if(DMA1->ISTS & DMA_ISTS_GIF2){
        dma_irq_handler(1);
        DMA1->ICLR = (DMA_ICLR_CGIF2 | DMA_ICLR_CTCIF2 | DMA_ICLR_CERRIF2);
    }
}

void DMA1_Channel3_IRQHandler(void)
{ 
    if(DMA1->ISTS & DMA_ISTS_GIF3){
        dma_irq_handler(2);
        DMA1->ICLR = (DMA_ICLR_CGIF3 | DMA_ICLR_CTCIF3 | DMA_ICLR_CERRIF3);
    }
}

void DMA1_Channel4_IRQHandler(void)
{ 
    if(DMA1->ISTS & DMA_ISTS_GIF4){
        dma_irq_handler(3);
        DMA1->ICLR = (DMA_ICLR_CGIF4 | DMA_ICLR_CTCIF4 | DMA_ICLR_CERRIF4);
    }
}

void DMA1_Channel5_IRQHandler(void)
{ 
    if(DMA1->ISTS & DMA_ISTS_GIF5){
        dma_irq_handler(4);
        DMA1->ICLR = (DMA_ICLR_CGIF5 | DMA_ICLR_CTCIF5 | DMA_ICLR_CERRIF5);
    }
}

void DMA1_Channel6_IRQHandler(void)
{ 
    if(DMA1->ISTS & DMA_ISTS_GIF6){
        dma_irq_handler(5);
        DMA1->ICLR = (DMA_ICLR_CGIF6 | DMA_ICLR_CTCIF6 | DMA_ICLR_CERRIF6);
    }
}

void DMA1_Channel7_IRQHandler(void)
{ 
    if(DMA1->ISTS & DMA_ISTS_GIF7){
        dma_irq_handler(6);
        DMA1->ICLR = (DMA_ICLR_CGIF7 | DMA_ICLR_CTCIF7 | DMA_ICLR_CERRIF7);
    }
}

void DMA2_Channel1_IRQHandler(void)
{
    if(DMA2->ISTS & DMA_ISTS_GIF1){
        dma_irq_handler(0);
        DMA2->ICLR = (DMA_ICLR_CGIF1 | DMA_ICLR_CTCIF1 | DMA_ICLR_CERRIF1);
    }
}

void DMA2_Channel2_IRQHandler(void)
{
    if(DMA2->ISTS & DMA_ISTS_GIF2){
        dma_irq_handler(1);
        DMA2->ICLR = (DMA_ICLR_CGIF2 | DMA_ICLR_CTCIF2 | DMA_ICLR_CERRIF2);
    }
}

void DMA2_Channel3_IRQHandler(void)
{
    if(DMA2->ISTS & DMA_ISTS_GIF3){
        dma_irq_handler(2);
        DMA2->ICLR = (DMA_ICLR_CGIF3 | DMA_ICLR_CTCIF3 | DMA_ICLR_CERRIF3);
    }
}

void DMA2_Channel4_5_IRQHandler(void)
{
    if(DMA2->ISTS & DMA_ISTS_GIF4){
        dma_irq_handler(3);
        DMA2->ICLR = (DMA_ICLR_CGIF4 | DMA_ICLR_CTCIF4 | DMA_ICLR_CERRIF4);
    }

    if(DMA2->ISTS & DMA_ISTS_GIF5){
        dma_irq_handler(4);
        DMA2->ICLR = (DMA_ICLR_CGIF5 | DMA_ICLR_CTCIF5 | DMA_ICLR_CERRIF5);
    }
}