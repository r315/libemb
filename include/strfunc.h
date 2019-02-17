#ifndef _strfunc_h_
#define _strfunc_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

    char *nextWord(char *line);
    int8_t nextInt(char **line, int32_t *value);
    uint8_t nextHex(char **line, uint32_t *value);
    char nextChar(char **line);
    uint8_t isNextWord(char **str, const char *word);
    char *strsub(char *str, const char token, uint8_t len, char **saveptr);
	void xstrcpy(char *dst, char *src, uint8_t maxLen);
    char xstrcmp(char const *str1, char const *str2);
    size_t strlen(const char *str);
    uint8_t yatoi(char *str, int32_t *value);
    uint8_t hatoi(char *str, uint32_t *value);
    int xatoi(char **str, long *res);
    char *pftoa(double f, char places);
    char* pitoa(long val, int radix, int len);
    char * chrinstr( const char *str, char c);

#ifdef __cplusplus
}
#endif
#endif

