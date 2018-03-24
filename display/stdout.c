#include <stdarg.h>   // va_arg()
#include "stdout.h"

static Stdout *device;

void stdoutInit(Stdout *outdevice){
	device = outdevice;
}

char xgetchar(void){
	return device->xgetchar();
}

void xputc (char c){	
	device->xputc(c);
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void xitoa (long val, signed char radix, signed char len)
{
	unsigned char c, r, sgn = 0, pad = ' ';
	unsigned char s[20], i = 0;
	unsigned int v;


	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}
	v = val;
	r = radix;
	if (len < 0) {
		len = -len;
		pad = '0';
	}
	if (len > 20) return;
	do {
		c = (unsigned char)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	if (sgn) s[i++] = sgn;
	while (i < len)
		s[i++] = pad;
	do
		xputc(s[--i]);
	while (i);
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void xputs (const char* str)
{
	while (*str) 
		xputc(*str++);
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void xprintf (const char* str, ...)
{
	va_list arp;
	char d, w, s, l;
	signed char r;

	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			xputc(d); continue;
		}
		d = *str++; w = r = s = l = 0;
		if (d == '0') {
			d = *str++; s = 1;
		}
		while ((d >= '0')&&(d <= '9')) {
			w += w * 10 + (d - '0');
			d = *str++;
		}
		if (s) w = -w;
		if (d == 'l') {
			l = 1;
			d = *str++;
		}
		if (!d) break;
		if (d == 's') {
			xputs(va_arg(arp, char*));
			continue;
		}
		if (d == 'c') {
			xputc(va_arg(arp, int));
			continue;
		}
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X') r = 16;
		if (d == 'b') r = 2;
		if (!r) break;
		if (l) {
			xitoa(va_arg(arp, long), r, w);
		} else {
			if (r > 0)
				xitoa((long)va_arg(arp, unsigned int), r, w);
			else
				xitoa((long)va_arg(arp, int), r, w);
		}
	}

	va_end(arp);
}
