#ifndef _dma_h_
#define _dma_h_

enum{   //DMA direction
    mem_to_mem,
    mem_to_dev,
    dev_to_mem,
    dev_to_dev
};

typedef struct {
	uint32_t chnum;				
	uint32_t size;
	uint32_t datasize;
	uint32_t src;				
	uint32_t dst;				
	uint32_t dir;
} Gpdma;


#endif /* _dma_h_ */