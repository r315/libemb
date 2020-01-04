
#include <stdint.h>
#if 0
#include <display.h>
#define DBG_PRINTF	DISPLAY_printf
#define DBG_PUTC	DISPLAY_putc

#else
#include <stdio.h>
#define DBG_PRINTF	printf
#define DBG_PUTC	putchar
#endif

#define LINESIZE 0x8

void hexDumpLine(uint8_t *mem, uint8_t print_ascii, uint32_t len){
int i;	

    for(i=0; i<len; i++){
		DBG_PRINTF("%02X ",*(mem + i));
	}

    if(!print_ascii)
        return;

	for(i=0;i<len;i++){
		if(*mem > (' '-1) && *mem < 0x7F)
			DBG_PRINTF("%c",*mem);
		else{
			DBG_PUTC('.');
		}
		mem++;
	}
}


void hexDump(uint8_t *mem, uint32_t len){
    DBG_PRINTF("\n Dump address: 0x%p\n\n",mem);
	for(int i=0; i<len ;i+=LINESIZE){
		DBG_PRINTF("%02X: ",i);
		hexDumpLine(mem, 1, LINESIZE);
		DBG_PUTC('\n');
		mem += LINESIZE;
	}
}

