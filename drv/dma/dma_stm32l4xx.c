#include <stdint.h>
#include "stm32l4xx.h"
#include "dma.h"


/**
 * @brief Request dma controller/channel
 * \param ctrl    : pointer to hold controller and channel
 * \param request : Request in format
 *                   [9:8] -> DMA number
 *                   [7:4] -> Channel number
 *                   [3:0] -> Request source
 * */
void DMA_Request(dmactrl_t *ctrl, uint32_t request){
    
    uint8_t ch = (request & DMA_CHANNEL_MASK) >> DMA_CHANNEL_POS;
    uint8_t source = (request & DMA_SOURCE_MASK) >> DMA_SOURCE_POS;    
    
    if(request & DMA_NUMBER_MASK){
        ctrl->ctrl = DMA2;
        ctrl->channel = (void*)((uint32_t)DMA2_Channel1 + (ch * 0x14));
        ctrl->irq = (ch < 5) ? DMA2_Channel1_IRQn + ch : DMA2_Channel6_IRQn + ch;
        DMA2_CSELR->CSELR = (DMA2_CSELR->CSELR & ~(15 << (ch << 2))) | (source << (ch << 2));
    }else{
        ctrl->ctrl = DMA1;
        ctrl->channel = (void*)((uint32_t)DMA1_Channel1 + (ch * 0x14));
        ctrl->irq = DMA1_Channel1_IRQn + ch;
        DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~(15 << (ch << 2))) | (source << (ch << 2));
    }
}
