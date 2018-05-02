#include <display.h>

#define LINESIZE 0x8


void hexDumpLine(char *mem, char print_ascii, int len){
int i;	

    for(i=0; i<len; i++){
		DISPLAY_printf("%02X",*(mem + i));
	}
    if(!print_ascii)
        return;

	for(i=0;i<len;i++){
		if(*mem > (' '-1) && *mem < 0x7F)
			DISPLAY_printf("%c",*mem);
		else{
			DISPLAY_putc('.');
		}
		mem++;
	}
}


void hexDump(char *mem, int len){
    DISPLAY_printf("%p:\n",mem);
	for(int i=0; i<len ;i+=LINESIZE){
		DISPLAY_printf("%02X: ",i);
		hexDumpLine(mem, 1, LINESIZE);
		DISPLAY_putc('\n');
		mem += LINESIZE;
	}
}

