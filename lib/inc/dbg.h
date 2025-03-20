#ifndef _DBG_H_
#define _DBG_H_
#include <stdint.h>

#ifndef DBG_PRINT_MAX_LEN
#define DBG_PRINT_MAX_LEN   64
#endif

void dbg_HexDump(uint8_t *mem, uint32_t len);
void dbg_HexDumpLine(uint8_t *mem, uint32_t len, uint8_t print_ascii);
int  dbg_printf(const char*, ...);
int  dbg_puts(const char*);
void dbg_init(void *stdo);
#endif /* _DBG_H_ */
