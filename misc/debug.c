
#ifdef ENABLE_DEBUG
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <strfunc.h>
#include <dbg.h>

extern void dbg_putc(char c);

#define LINESIZE 0x8

void dbgHexDumpLine(uint8_t *mem, uint32_t len, uint8_t print_ascii ){
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
			dbg_putc('.');
		}
		mem++;
	}
	dbg_putc('\n');
}


void dbgHexDump(uint8_t *mem, uint32_t len){
    dbg_printf("\n Dump address: 0x%p\n\n",mem);
	for(int i=0; i<len ;i+=LINESIZE){
		dbg_printf("%02X: ",i);
		dbgHexDumpLine(mem, LINESIZE, 1);		
		mem += LINESIZE;
	}
}

void dbg_puts(char *str){
	while(*str){
		dbg_putc(*str++);
	}
}

void dbg_printf(const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l, f;

	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			dbg_putc(d); continue;
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
			dbg_puts(va_arg(arp, char*));
			continue;
		}
		if (d == 'c') {
			dbg_putc((char)va_arg(arp, int));
			continue;
		}
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
		if (d == 'b') r = 2;
		if (d == 'f') {
			if (!f)
				w = 6;						// dafault 6 decimal places
			dbg_puts(pftoa(va_arg(arp, double), w));
			continue;
		}
		if (!r) break;
		if (s) w = -w;
		if (l) {
			dbg_puts(pitoa((long)va_arg(arp, long), r, w));
		}
		else {
			if (r > 0)
				dbg_puts(pitoa((unsigned long)va_arg(arp, int), r, w));
			else
				dbg_puts(pitoa((long)va_arg(arp, int), r, w));
		}
	}

	va_end(arp);
}
#endif