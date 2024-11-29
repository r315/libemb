
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
	.readchar = dummy_getchar,
	.writechar = dummy_putchar,
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
			sto->writechar('.');
		}
		mem++;
	}
	sto->writechar('\n');
}


void dbg_HexDump(uint8_t *mem, uint32_t len){
    //dbg_printf("\nDump address: 0x%X \n\n",(uint32_t)&mem[0]);
	for(int i=0; i<len ;i+=LINE_NCOLS){
		dbg_printf("%02X: ",i);
		dbg_HexDumpLine(mem, LINE_NCOLS, 1);		
		mem += LINE_NCOLS;
	}
}

int dbg_puts(const char *str){
    int len = 0;
    char dbg_out[DBG_PRINT_MAX_LEN];

    while(*str){
        dbg_out[len++] = *str++;
    }
    
    dbg_out[len++] ='\n';
	return sto->write(dbg_out, len);
}

int dbg_printf(const char* fmt, ...){
	char dbg_out[DBG_PRINT_MAX_LEN];
	va_list arp;
	va_start(arp, fmt);
	int len = strformater(dbg_out, fmt, arp);
	va_end(arp);
	return sto->write(dbg_out, len);
}
#endif