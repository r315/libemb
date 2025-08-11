#ifndef _debug_h_
#define _debug_h_

#include <stdint.h>
#include "stdinout.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DBG_PRINT_MAX_LEN
#define DBG_PRINT_MAX_LEN   64
#endif

void dbg_HexDump(const uint8_t *mem, uint32_t len);
void dbg_HexDumpLine(const uint8_t *mem, uint32_t len, uint8_t print_ascii);
void dbg_putchar(char);
int  dbg_printf(const char*, ...);
int  dbg_println(const char*);
void dbg_init(const stdinout_t *);

#ifdef __cplusplus
}
#endif
#endif /* _DBG_H_ */
