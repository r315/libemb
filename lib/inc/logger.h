#ifndef _logger_h_
#define _logger_h_

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#define LOG_USE_TAG_COLOR   1
#define LOG_USE_BUF_DUMP    0

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
//https://espterm.github.io/docs/VT100%20escape%20codes.html
#define VT100_EL1   "\e[1K"
#define VT100_EL2   "\e[2K"
#define VT100_ED2   "\e[2J"

#ifdef LOG_PRINTF_FUNC
#define LOG_PRINT(...) LOG_PRINTF_FUNC(__VA_ARGS__)
#define LOG_PRINTLN(...) do { DBG_PRINT(__VA_ARGS__); DBG_PRINT("\n"); } while(0)
int LOG_PRINTF_FUNC(const char* fmt, ...);
#else
#define LOG_PRINT(...) printf(__VA_ARGS__)
#define LOG_PRINTLN(...) do { printf(__VA_ARGS__); putchar('\n'); } while(0)
#endif


#define LOG_INF(...) LOG_PRINTLN(VT100_GREEN "[INFO]\t" VT100_NORMAL __VA_ARGS__)
#define LOG_WRN(...) LOG_PRINTLN(VT100_YELLOW "[WARN]\t" VT100_NORMAL __VA_ARGS__)
#define LOG_ERR(...) LOG_PRINTLN(VT100_RED "[ERROR]\t" VT100_NORMAL __VA_ARGS__)

static void __attribute__((unused)) LOG_HEX_DUMP(uint8_t *buf, uint16_t count)
{
    for(uint16_t i = 0; i < count; i ++){
        if( (i & 15) == 0)
            LOG_PRINT("\n%02X: ", i & 0xF0);
        LOG_PRINT("%02X ", buf[i]);
    }
    LOG_PRINT("\n");
}

#endif