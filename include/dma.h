#ifndef _dma_h_
#define _dma_h_

#include <stdint.h>


typedef struct dmactrl{
   void *ctrl;
   void *channel;
   uint32_t irq;
}dmactrl_t;

#define DMA_NUMBER_POS     8
#define DMA_CHANNEL_POS    4
#define DMA_SOURCE_POS     0

#define DMA_NUMBER_MASK    (3 << DMA_NUMBER_POS)
#define DMA_CHANNEL_MASK   (15 << DMA_CHANNEL_POS)
#define DMA_SOURCE_MASK    (15 << DMA_SOURCE_POS)

/**
 * @brief Request dma controller/channel
 * \param ctrl    : pointer to hold controller and channel
 * \param request : Request in format
 *                   [9:8] -> DMA number
 *                   [7:4] -> Channel number
 *                   [3:0] -> Request source
 * */
void DMA_Request(dmactrl_t *ctrl, uint32_t request);

#endif /* _dma_h_ */