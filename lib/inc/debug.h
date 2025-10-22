#ifndef _debug_h_
#define _debug_h_

#include <stdint.h>
#include "stdinout.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_USE_TAG_COLOR
#define LOG_USE_TAG_COLOR   1
#endif

#ifndef DBG_PRINT_MAX_LEN
#define DBG_PRINT_MAX_LEN   64
#endif

#if LOG_USE_TAG_COLOR
//https://gist.github.com/viniciusdaniel/53a98cbb1d8cac1bb473da23f5708836
#define VT100_NORMAL "\e[0m"
#define VT100_BOLD   "\e[1m"
#define VT100_RED    "\e[31m"
#define VT100_GREEN  "\e[32m"
#define VT100_YELLOW "\e[33m"
#define VT100_BLUE   "\e[34m"
#else
#define VT100_BOLD
#define VT100_NORMAL
#endif

#if defined(DEBUG) || defined(ENABLE_DEBUG)
#ifdef DBG_PRINTF_FUNC
#define DBG_PRINTF(...) do { DBG_PRINT_FUNC(__VA_ARGS__); } while(0)
#define DBG_PRINTLN(...) do { DBG_PRINTF(__VA_ARGS__); DBG_PRINTF("\n"); } while(0)
#else
#define DBG_PRINTF dbg_printf
#define DBG_PRINT(...) do { DBG_PRINTF(__VA_ARGS__); } while(0)
#define DBG_PRINTLN(...) do { DBG_PRINTF(__VA_ARGS__); dbg_putchar('\n'); } while(0)
#endif
#else
#define DBG_PRINT(...)
#endif

#if defined(DEBUG) || defined(ENABLE_DEBUG)
#define DBG_INF(...) DBG_PRINTLN(VT100_GREEN "[INFO] " VT100_NORMAL __VA_ARGS__)
#define DBG_WRN(...) DBG_PRINTLN(VT100_YELLOW "[WARN] " VT100_NORMAL __VA_ARGS__)
#define DBG_ERR(...) DBG_PRINTLN(VT100_RED "[ERROR] " VT100_NORMAL __VA_ARGS__)
#define DBG_HEXDUMP dbg_hexdump
#define DBG_HEXPRINT dbg_hexprint
#else
#define DBG_INF(...)
#define DBG_WRN(...)
#define DBG_ERR(...)
#define DBG_HEXDUMP(...)
#define DBG_HEXPRINT(...)
#endif

void dbg_hexdump(const uint8_t *mem, uint32_t len, uint8_t ncols, uint8_t ascii);
void dbg_hexprint(const uint8_t *mem, uint32_t len, uint8_t ascii);
void dbg_putchar(char);
int  dbg_printf(const char*, ...);
int  dbg_println(const char*);
void dbg_init(const stdinout_t *);

#ifdef __cplusplus
}
#endif
#endif /* _DBG_H_ */
