#ifndef _strfunc_h_
#define _strfunc_h_

#ifdef __cplusplus
extern "C" {
#endif

#define XPITOA_BUF_SIZE		20
#define FLOAT_MAX_PRECISION 6

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

	char *nextWord(char *line);
	uint8_t nextInt(char **line, int32_t *value);
	uint8_t nextHex(char **line, uint32_t *value);
	uint8_t nextDouble(char **line, double *value);
	char nextChar(char **line);
	uint8_t isNextWord(char **str, const char *word);
	void xstrcpy(char *dst, const char *src, uint8_t maxLen);
	char xstrcmp(char const *str1, char const *str2);
	//size_t strlen(const char *str);
	uint8_t ia2i(const char *str, int32_t *value);
	uint8_t ha2u(const char *str, uint32_t *value);
	uint8_t da2d(const char *str, double *value);
	uint32_t d2da(char *dst, double f, uint8_t places);
	uint32_t i2ia(char *dst, int32_t val, int radix, int ndig);
	char *chrinstr(const char *str, char c);
	char *strsub(char *str, const char token, uint8_t len, char **saveptr);
	uint32_t strformater(char *dst, const char* fmt, int len, va_list arp);
	void memset16(uint16_t *dst, uint16_t c, uint32_t n);
	void memcpy16(uint16_t *dst, uint16_t *src, uint32_t n);
	void memset32(uint32_t *dst, uint32_t c, uint32_t n);
	void memcpy32(uint32_t *dst, uint32_t *src, uint32_t n);
    void *memmove(void * dst, const void * src, size_t len);
	char *skipSpaces(char *str);
	uint32_t strToArray(char *str, char **argv, int max_arg);
	void xsprintf(char *out, const char* fmt, ...);
	int32_t strFind(const char *str, char *strarr[], uint16_t arrsize);
#ifdef __cplusplus
}
#endif
#endif

