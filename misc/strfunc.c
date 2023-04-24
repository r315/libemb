
#include <strfunc.h>
#include <stdarg.h>

/**
 * @brief Find next character in a string
 * */
char *skipSpaces(char *str){
	while((*str == ' ' || *str == '\t') && *str != '\0') 
		str++;
	return str;
}

/** 
 * @brief Replaces spaces in a string with string terminator, aka string split
 * 
 * \param str : pointer to input string
 * \param argv : pointer to output string array
 * \return : number of strings
 * */
uint32_t strToArray(char *str, char **argv){
uint32_t argc = 0;

    if(str == NULL){
        return 0;
    }

	str = skipSpaces(str);

	if(*str == '\0')
		return 0;

	argv[argc++] = str;

    while(*str != '\0'){
        if(*str == ' '){
			*str = '\0';
			str = skipSpaces(str + 1);
			if(*str != '\0')
				argv[argc++] = str;
        }else{
			str++;
		}
    }

	argv[argc] = NULL;

	return argc;
}
/**
 * @brief Well string length...
 * */
size_t strlen(const char *str) {
	size_t count = 0;
	//if (str == NULL) return 0;
	while (*str++) {
		count++;
	}
	return count;
}

/**
 * @brief Find a string in string array and return index
 * 
 * \param str : pointer to string to be found
 * \param strarr : array of pointers to string
 * \return : index, -1 if not found
 * */
int32_t strFind(const char *str, char *strarr[], uint16_t arrsize){
    for(uint32_t i = 0; i < arrsize; i++){
        if(xstrcmp(str, strarr[i]) == 0){
            return i;
        }
    }
    return -1;
}
/**
 * Searches for the first occurrence of the character c
 * in the string pointed to by the argument str.
 *
 * \param str   - input string
 * \param c     - character to be found
 * \return      - pointer to found char if found, pointer to string terminator if not
 * */
char *chrinstr(const char *str, char c) {
	if (str == NULL) return 0;
	while (*str) {
		if (*str == c)
			break;
		str += 1;
	}
	return (char*)str;
}

/**
 * Return pointer to next separated word by ' ' in the given string
 *
 * \param line  - pointer to string
 * \return      - pointer to first character of next word, pointer to string terminator
 * */
char *nextWord(char *line) {
	line = chrinstr(line, ' ');
	if (*line != '\0')
		line++;
	return line;
}

/**
 * Try to parse an hex string to integer from parameter line
 * and move to line pointer to next parameter
 *
 * \param  line pointer to string pointer
 * \param  value pointer to output value
 * \return 1 if success and increments line pointer, 0 on failure no parameters are affected
 * */
uint8_t nextHex(char **line, uint32_t *value) {
	if (hatoi(*line, value)) {
		*line = nextWord(*line);
		return 1;
	}
	return 0;
}

/**
 * Try to get an integer number from a given string
  *
 * \param  line - pointer to string pointer
 * \param  value - pointer to output value
 * \return 1 - if success and move to next substring, 0 - on failure no parameters are affected
 * */
uint8_t nextInt(char **line, int32_t *value) {
	if (yatoi(*line, value)) {
		*line = nextWord(*line);
		return 1;
	}
	return 0;
}

/**
 * Try to parse a double number from a given string
  *
 * \param  line - pointer to string pointer
 * \param  value - pointer to output value
 * \return 1:if success and move to next substring, 0:on failure no parameters are affected
 * */
uint8_t nextDouble(char **line, double *value) {
	if (fatoi(*line, value)) {
		*line = nextWord(*line);
		return 1;
	}
	return 0;
}


/**
 * Get first char of sprint pointed by line, and advance that pointer to next char
 * by skiping space character
 * \param line		pointer string pointer
 * \return		first char of current word
 *
 * */
char nextChar(char **line) {
	char c;
	c = *line[0];
	*line = nextWord(*line);
	return c;
}

/**
 * @brief Compare if first word in a string is equal to the given word.
 *
 * @param str:    pointer to word pointer to be compared
 * @param word:   pointer to comparing word
 * 
 * @return      - 1 if match and move str pointer to next word,
 *                0 not equal and no parameter changed
 * */
uint8_t isNextWord(char **str, const char *word) {
	char *str1 = *str;

	while (*word != '\0') {
		if (*str1 != *word)
			return 0;
		str1++;
		word++;
	}	
	*str = nextWord(*str);	
	return 1;
}


/**
 * @brief get first occuring substring from a token split of a given string
 *
 * @param str:		reference to input string, on return the reference will pointer
 * 					for then remaining string.
 * @param token:	token to split the string
 * @param len:		length of the given string
 * @param saveptr:	pointer for saving the remaining string
 *
 * returns: 		pointer for found sub string or null if not splitted
 **/
char *strsub(char *str, const char token, uint8_t len, char **saveptr) {
	char *ptr, i = 0;

	if (str == NULL) {
		return NULL;
	}

	// save pointer for return
	ptr = str;

	// search token, if finds it 
	// replace it with string terminator character
	while (*ptr && i < len) {
		if (*ptr == token) {
			*ptr = '\0';
			break;
		}
		ptr += 1;
		i++;
	}

	// got to the end of the string and token was not found return null
	// or string is empty
	if (i == len || ptr == str) {
		return NULL;
	}

	// if specified, return pointer to remaining string
	if (saveptr != NULL) {
		*saveptr = ptr + 1;
	}

	// return pointer to substring
	return str;
}

/**
 * @brief Compares two if strings are equal until str1 end
 * 
 * @param str1: first string
 * @param str2: second string
 * 
 * @return 0 if equal, difference of first non equal char
 * */
char xstrcmp(char const *str1, char const *str2) {
	while (*str1 == *str2) {
		if (*str1 == '\0')
			return 0;
		str1++;
		str2++;
	}
	return (*str1 - *str2);
}

void xstrcpy(char *dst, const char *src, uint8_t maxLen) {
	while(maxLen--){
		*dst++ = *src;
		if(*src == '\0'){
			break;
		}
		src++;
	}
}

/**
 * Try to parse a string representing a integer
 *
 * \param  str - pointer to input string
 * \param  value - pointer to output value
 * \return number of converted digits
 * */
uint8_t yatoi(char *str, int32_t *value) {
	int val = 0;
	char c = *str;
	uint8_t s = 0;

	if(str == NULL){
		return 0;
	}

	if (c == '-') {
		s = (1 << 7); // Set signal flag
		str++;
		c = *str;
	}

	do{
		if (c > '/' && c < ':') {
			c -= '0';
			val = val * 10 + c;
			s++;
		}
		else {
			return 0;
		}
		c = *(++str);
	}while (c != ' ' && c != '\n' && c != '\r' && c != '\0');
		
	// check signal flag
	*value = (s & (1 << 7)) ? -val : val;

	return s & 0x7F;
}

/**
 * Try to parse a string representing a hex number to integer
 * \param  str	pointer to input string
 * \param  value  pointer to output value
 * \return 1 if success, 0 if failed
 * */
uint8_t hatoi(char *str, uint32_t *value) {
	uint32_t val = 0;
	char c = *str;

	if(str == NULL){
		return 0;
	}

	do {
		val <<= 4;
		if (c > '`' && c < 'g') {
			c -= 'W';
		}
		else if ((c > '@' && c < 'G')) {
			c -= '7';
		}
		else if (c > '/' && c < ':') {
			c -= '0';
		}
		else {
			return 0;
		}

		val |= c;
		c = *(++str);

	} while (c != '\0' && c != ' ' && c != '\n' && c != '\r');

	*value = val;
	return 1;
}

/**
 * Try to parse a string representing a double
 *
 * \param  str - pointer to input string
 * \param  value - pointer to output value
 * \return 1:success, 0:failed
 * */
uint8_t fatoi(char *str, double *value) {
	uint8_t s = 0;
	double val = 0;
	char c = *str;
	int decimal = 1;

	if(str == NULL){
		return 0;
	}

	if (c == '-') {
		s = (1 << 7); // Set signal flag
		str++;
		c = *str;
	}

	do {
		if (c > '/' && c < ':') {
			c -= '0';
			val = val * 10 + c;
			if (s & (1 << 6))
				decimal *= 10;
			s++;
		}else if(c == '.'){
			s |= (1 << 6);	// Set decimal point flag
		}else {
			return 0;
		}
		c = *(++str);
	} while (c != ' ' && c != '\n' && c != '\r' && c != '\0');

	val = val / decimal;

	// check signal flag
	*value = (s & (1 << 7)) ? -val : val;

	return s & 0x3F;
}

/**
 * @brief String formater
 *   %nu, %nd, %nb, %c, %s, %l, %x, %.nf
 * 
 * TODO: fix print percent sign (%)
 * */
uint32_t strformater(char *dst, const char* fmt, va_list arp){
	
	int d, r, w, s, l, f;
	char *p,*a;
	a = dst;

	while ((d = *fmt++) != '\0') {

		if (d != '%') {
			*(dst++) = d; 
			continue;
		}

		d = *fmt++;		
			
		f = w = r = s = l = 0;

		if (d == '.') {
			d = *fmt++; f = 1;
		}

		if (d == '0') {
			d = *fmt++; s = 1;
		}

		while ((d >= '0') && (d <= '9')) {
			w = w * 10 + (d - '0');
			d = *fmt++;
		}

		if (d == 'l') {
			l = 1;
			d = *fmt++;
		}

		if (d == '\0'){
			break;
		}
		
		if (d == 's') {
			p = va_arg(arp, char*);
			while(*p){
				*(dst++) = *(p++);
			}
			continue;
		}

		if (d == 'c') {
			*(dst++) = (char)va_arg(arp, int);
			continue;
		}

		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16;
		if (d == 'b') r = 2;
		if (d == 'f') {
			if (!f)
				w = FLOAT_MAX_PRECISION;
			dst += xpftoa(dst, va_arg(arp, double), w);
			continue;
		}
		
		if (r == 0){
			break;	
		} 

		if (s) w = -w;

		if (l) {
			dst += xpitoa(dst, (long)va_arg(arp, long), r, w);
		}
		else {
			if (r > 0)
				dst += xpitoa(dst, (unsigned long)va_arg(arp, int), r, w);
			else
				dst += xpitoa(dst, (long)va_arg(arp, int), r, w);
		}
	}

	*dst = '\0';
	return dst - a;
}

/**
 * @brief
 * */
void xsprintf(char *out, const char* fmt, ...){
	va_list arp;
	va_start(arp, fmt);
	strformater(out, fmt, arp);
	va_end(arp);
}


/**
 * @brief Convert 32-bit integer number to string string
 *
 * \param dst 	:	pointer to destination buffer
 * \param val	:	value to be converted
 * \param radix	:	base of convertion [-10,10,16]
 * \param ndig 	:	minimum number of digits, ndig > 0 pad with ' ', ndig < 0 pad with '0'
 * \return 		:	number of digits written to dst
 * */

uint32_t xpitoa(char *dst, int32_t val, int radix, int ndig){
	char buf[XPITOA_BUF_SIZE];
	uint8_t i, c, r, sgn = 0, pad = ' ';
	uint32_t v;

	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}

	v = val;
	r = radix;

	if (ndig < 0) {
		ndig = -ndig;
		pad = '0';
	}

	if (ndig > XPITOA_BUF_SIZE) {
		ndig = XPITOA_BUF_SIZE;
	}

	ndig = XPITOA_BUF_SIZE - 1 - ndig;
	i = XPITOA_BUF_SIZE;
	buf[--i] = '\0';

	do {
		c = (uint8_t)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		buf[--i] = c;
		v /= r;
	} while (v);

	if (sgn) buf[--i] = sgn;

	while (i > ndig) {
		buf[--i] = pad;
	}

	ndig = XPITOA_BUF_SIZE - 1 - i;

	while(buf[i]){
		*dst++ = buf[i++];
	}

	*dst = '\0';

	return ndig;
}

/**
 * @brief Convert float to string
 *
 * https://en.wikipedia.org/wiki/Single-precision_floating-point_format
 * https://wirejungle.wordpress.com/2011/08/06/displaying-floating-point-numbers
 *
 * \param dst 		: pointer to destination buffer
 * \param f			: value to be converted
 * \param places	: number of decimal places
 * \return 			: number of digits written to dst
 * */
uint32_t xpftoa(char *dst, float f, uint8_t places){
	int32_t int_part, frac_part;
	uint8_t prec;
	char *s = dst;

	int_part = (long)(f);

	if (places > FLOAT_MAX_PRECISION)
		places = FLOAT_MAX_PRECISION;

	frac_part = 0;
	prec = 0;

	while ((prec++) < places) {
		f *= 10;
		frac_part = (frac_part * 10) + (long)f - ((long)f / 10) * 10;  //((long)f%10);			
	}

	dst += xpitoa(dst, int_part, -10, 0);
	*dst++ = '.';
	dst += xpitoa(dst, abs(frac_part), 10, -places);

	return (dst - s);
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
void *memcpy(void * destination, const void * source, size_t num) {
	for (size_t i = 0; i < num; i++) {
		*((uint8_t*)destination + i) = *((uint8_t*)source);
		source = (uint8_t*)source + 1;
	}
	return destination;
}

void *memmove(void * dst, const void * src, size_t len) {
    uint8_t *pdst, *psrc;
   
    if(dst < src){
        pdst = (uint8_t*)dst;
        psrc = (uint8_t*)src;
        while(len--){
            *pdst++ = *psrc++;
        }
    }else{
        pdst = (uint8_t*)dst + len;
        psrc = (uint8_t*)src + len;
        while(len--){
            *pdst-- = *psrc--;
        }
    }
    return dst;
}

void *memset(void * ptr, int value, size_t count) {
    for (size_t i = 0; i < count; i++) {
	    *((volatile uint8_t*)ptr + i) = (uint8_t)value;
    }
	return ptr;
}

void memset16(uint16_t *dst, uint16_t c, uint32_t n){
    while(n--){ *dst++ = c;  }
}

void memcpy16(uint16_t *dst, uint16_t *src, uint32_t n){
    while(n--){ *dst++ = *src++;  }
}

void memset32(uint32_t *dst, uint32_t c, uint32_t n){
    while(n--){ *dst++ = c;  }
}

void memcpy32(uint32_t *dst, uint32_t *src, uint32_t n){
    while(n--){ *dst++ = *src++;  }
}