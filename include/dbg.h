#ifndef _DBG_H_
#define _DBG_H_
#include <stdint.h>

void dbgHexDump(uint8_t *mem, uint32_t len);
void dbg_printf(const char*, ...);
void dbg_puts(char*);
#endif /* _DBG_H_ */
