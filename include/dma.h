#ifndef _dma_h_
#define _dma_h_

#include <stdint.h>

typedef struct{
   void *gpdma;
   uint32_t src;
   uint32_t dst;
   uint32_t len;
   uint8_t number;
   uint8_t circular;
   uint8_t priority;
}Dmachannel;

/**
 * @brief
 * */
void DMA_Setup(Dmachannel *ch);
void DMA_Start(Dmachannel *ch);
void DMA_Stop(Dmachannel *ch);
void DMA_Init(void);


#endif /* _dma_h_ */