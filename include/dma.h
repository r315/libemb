#ifndef _dma_h_
#define _dma_h_

#include <stdint.h>

typedef struct dmactrl{
   void *ctrl;
   void *channel;
   uint32_t irq;
}dmactrl_t;

/**
 * @brief Request dma controller/channel
 * \param ctrl    : pointer to hold controller and channel
 * \param request : Request number
 * */
void DMA_Request(dmactrl_t *ctrl, uint32_t request);

#endif /* _dma_h_ */