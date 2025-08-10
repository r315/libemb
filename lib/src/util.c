// general purpose utilities
#include <display.h>

void hexDumpLine(char *mem, char hex, int len){
int i;	
	if(hex){
		for(i=0;i<len;i++){
			DISPLAY_printf("%2X ",*mem++);
		}
	}

	for(i=0;i<len;i++){
		if(*mem > (' '-1) && *mem < 0x7F)
			DISPLAY_printf("%c",*mem);
		else{
			DISPLAY_putc('.');
		}
		mem++;
	}
}

#define LINESIZE 0x10
void hexDump(char *mem, int len){
	for(int i=0; i<len ;i+=LINESIZE){
		DISPLAY_printf("%2X: ",i);
		hexDumpLine(mem, 1, LINESIZE);
		hexDumpLine(mem, 0, LINESIZE);
		DISPLAY_putc('\n');
		mem += LINESIZE;
	}
}

