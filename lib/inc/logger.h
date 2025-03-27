#ifndef _logger_h_
#define _logger_h_

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

#ifdef LOG_PRINTF_FUNC
#define LOG_PRINT(...) \
    do { LOG_PRINTF_FUNC(__VA_ARGS__); } while(0)
#else
#define LOG_PRINT(...) \
    do { printf(__VA_ARGS__); putchar('\n'); } while(0)
#endif


#define LOG_INF(...) LOG_PRINT(VT100_GREEN "INFO: " VT100_NORMAL __VA_ARGS__)
#define LOG_WRN(...) LOG_PRINT(VT100_YELLOW "WARN: " VT100_NORMAL __VA_ARGS__)
#define LOG_ERR(...) LOG_PRINT(VT100_RED "ERROR: " VT100_NORMAL __VA_ARGS__)

#if DEBUG
#define DBG_INF(...) LOG_INF(__VA_ARGS__)
#define DBG_WRN(...) LOG_WRN(__VA_ARGS__)
#define DBG_ERR(...) LOG_ERR(__VA_ARGS__)
#else
#define DBG_INF(...)
#define DBG_WRN(...)
#define DBG_ERR(...)
#endif

#if LOG_USE_BUF_DUMP
static void DBG_DUMP(uint8_t *buf, uint32_t count)
{
    for(int i = 0; i < count; i ++){
        if( (i & 15) == 0)
            printf("\n%02X: ", i & 0xF0);
        printf("%02X ", buf[i]);
    }
    putchar('\n');
}
#endif

#endif