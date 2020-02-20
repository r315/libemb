#ifndef _DBG_H_
#define _DBG_H_
#include <stdint.h>
#include <stdout.h>

void dbg_HexDump(uint8_t *mem, uint32_t len);
void dbg_HexDumpLine(uint8_t *mem, uint32_t len, uint8_t print_ascii);
void dbg_printf(const char*, ...);
void dbg_puts(char*);
void dbg_init(stdout_t *stdo);
#endif /* _DBG_H_ */
