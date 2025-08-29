
#if defined(DEBUG) || defined(ENABLE_DEBUG)
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <strfunc.h>
#include "debug.h"

#define LINE_NCOLS 0x8		// Number of columens per line

static int dummy_write(const char *str, int len){(void)str; (void)len; return len; }
static int dummy_read(char *str, int len){(void)str; (void)len; return 0; }
static int dummy_available(void) { return 0; }

static const stdinout_t dummy_out = {
    .available = dummy_available,
	.read = dummy_read,
	.write = dummy_write,
};

static const stdinout_t *sto = &dummy_out;

void dbg_init(const stdinout_t *stdo)
{
	if(stdo != NULL){
		sto = stdo;
	}
}

void dbg_hexprint(const uint8_t *mem, uint32_t len, uint8_t ascii)
{
    uint32_t i;

    for(i= 0 ; i < len; i++){
		dbg_printf("%02X ",*(mem + i));
	}

    if(ascii){
        for(i = 0; i < len; i++){
            if(*mem > (' '-1) && *mem < 0x7F)
			dbg_printf("%c", *mem);
            else{
                dbg_putchar('.');
            }
            mem++;
        }
	}

	dbg_putchar('\n');
}


void dbg_hexdump(const uint8_t *addr, uint32_t len, uint8_t ncols, uint8_t ascii)
{
    //dbg_printf("\nDump address: 0x%X \n\n",(uint32_t)&mem[0]);
	for(uint32_t i = 0; i < len; i += ncols){
		dbg_printf("%02X: ",i);
		dbg_hexprint(addr, ncols, ascii);
		addr += ncols;
	}
}

void dbg_putchar(char c)
{
    sto->write((const char*)&c, 1);
}

int dbg_println(const char *str)
{
    int len = 0;
    char dbg_out[DBG_PRINT_MAX_LEN];

    while(*str){
        dbg_out[len++] = *str++;
    }

    dbg_out[len++] ='\n';
	return sto->write(dbg_out, len);
}

int dbg_printf(const char* fmt, ...)
{
	char dbg_out[DBG_PRINT_MAX_LEN];
	va_list arp;
	va_start(arp, fmt);
	int len = strformater(dbg_out, fmt, arp, DBG_PRINT_MAX_LEN);
	va_end(arp);
	return sto->write(dbg_out, len);
}
#endif