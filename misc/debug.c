
#ifdef ENABLE_DEBUG
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <strfunc.h>
#include <dbg.h>

#define LINESIZE 0x8

void dummy_init(void){ }
char dummy_getchar(void){ return 255; }
void dummy_putchar(char c){ }
void dummy_puts(const char *str){ }
uint8_t dummy_nb(char *c){ return 0; }
uint8_t dummy_kbhit(void){ return 0;}

 static stdout_t dummy_out = {
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
    dbg_printf("\n Dump address: 0x%p\n\n",mem);
	for(int i=0; i<len ;i+=LINESIZE){
		dbg_printf("%02X: ",i);
		dbg_HexDumpLine(mem, LINESIZE, 1);		
		mem += LINESIZE;
	}
}

void dbg_puts(char *str){	
	sto->xputs(str);
}

void dbg_printf(const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l, f;

	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			sto->xputchar(d); continue;
		}
		d = *str++; f = w = r = s = l = 0;
		if (d == '.') {
			d = *str++; f = 1;
		}
		if (d == '0') {
			d = *str++; s = 1;
		}
		while ((d >= '0') && (d <= '9')) {
			w += w * 10 + (d - '0');
			d = *str++;
		}
		if (d == 'l') {
			l = 1;
			d = *str++;
		}
		if (!d) break;
		if (d == 's') {
			sto->xputs(va_arg(arp, char*));
			continue;
		}
		if (d == 'c') {
			sto->xputchar((char)va_arg(arp, int));
			continue;
		}
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
		if (d == 'b') r = 2;
		if (d == 'f') {
			if (!f)
				w = 6;						// dafault 6 decimal places
			sto->xputs(pftoa(va_arg(arp, double), w));
			continue;
		}
		if (!r) break;
		if (s) w = -w;
		if (l) {
			sto->xputs(pitoa((long)va_arg(arp, long), r, w));
		}
		else {
			if (r > 0)
				sto->xputs(pitoa((unsigned long)va_arg(arp, int), r, w));
			else
				sto->xputs(pitoa((long)va_arg(arp, int), r, w));
		}
	}

	va_end(arp);
}
#endif