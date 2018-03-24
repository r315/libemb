#include <mem.h>


//TODO use DMA
void MEM_cpy(void *dst, void *src, uint32_t size){
  while(size--)
    *((uint32_t*)dst++) = *((uint32_t*)src++);
}

void MEM_set(void *dst, uint8_t value, uint32_t size){
	while(size--)
		*((uint8_t*)(dst++)) = value;
}
