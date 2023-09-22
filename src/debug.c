
#ifdef ENABLE_DEBUG
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <strfunc.h>
#include <dbg.h>

#define LINE_NCOLS 0x8		// Number of columens per line


void dummy_init(void){ }
char dummy_getchar(void){ return 255; }
void dummy_putchar(char c){ }
int dummy_puts(const char *str){ return 0; }
int dummy_nb(char *c){ return 0; }
int dummy_kbhit(void){ return 0;}

stdout_t dummy_out = {
	dummy_init,
	dummy_getchar,
	dummy_putchar,
	dummy_puts,
	dummy_nb,
	dummy_kbhit
};

static stdout_t *sto = &dummy_out;

void dbg_init(stdout_t *stdo){
	if(stdo != NULL){
		sto = stdo;
	}	
}

void dbg_HexDumpLine(uint8_t *mem, uint32_t len, uint8_t print_ascii ){
int i;	

    for(i=0; i<len; i++){
		dbg_printf("%02X ",*(mem + i));
	}

    if(!print_ascii){
        return;
	}

	for(i=0;i<len;i++){
		if(*mem > (' '-1) && *mem < 0x7F)
			dbg_printf("%c", *mem);
		else{
			sto->xputchar('.');
		}
		mem++;
	}
	sto->xputchar('\n');
}


void dbg_HexDump(uint8_t *mem, uint32_t len){
    //dbg_printf("\nDump address: 0x%X \n\n",(uint32_t)&mem[0]);
	for(int i=0; i<len ;i+=LINE_NCOLS){
		dbg_printf("%02X: ",i);
		dbg_HexDumpLine(mem, LINE_NCOLS, 1);		
		mem += LINE_NCOLS;
	}
}

void dbg_puts(char *str){	
	sto->xputs(str);
}

void dbg_printf(const char* fmt, ...){
	char dbg_out[DBG_PRINT_MAX_LEN];
	va_list arp;
	va_start(arp, fmt);
	strformater(dbg_out, fmt, arp);
	va_end(arp);
	sto->xputs(dbg_out);
}
#endif