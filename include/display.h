#ifndef _display_h_
#define _display_h_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <lcd.h>
#include <font.h>

#ifndef __EMU__
	#define toupper(c) ( (c)-'a'+'A' )
#endif 

enum Attributes{
	FONT_NORMAL,
	FONT_INVERTED,
	FONT_TRANSPARENT,
	FONT_DOUBLE
};

typedef struct{
	void (*xputc)(char);		//for use on xstdout
	char (*xgetchar)(void);
	uint16_t forecolor;
	uint16_t backcolor;
	font_t *font;
	uint8_t vspace;
	uint16_t cx;
	uint16_t cy;
	uint16_t sc;
	int (*drawChar)(int  x, int y, unsigned char *d_char);
}Display;


void DISPLAY_Init(uint8_t initlcd);
void DISPLAY_SetFont(font_t *fnt);
void DISPLAY_SetAttribute(uint8_t atr);
int  DISPLAY_Char(int  x, int y, unsigned char c); 
int  DISPLAY_Text(int x, int y, const char *s);
void DISPLAY_Pixel(int x0, int y0);
int  DISPLAY_Uint(int x, int y, unsigned int v, unsigned char radix);
int  DISPLAY_Float(int x, int y, float f);
void *DISPLAY_GetReference(void);
void DISPLAY_SetColors(uint16_t fc, uint16_t bc);
void DISPLAY_SetFcolor(uint16_t color);
void DISPLAY_SetBcolor(uint16_t color);
int DISPLAY_GetFontWidth(void);
int DISPLAY_GetFontHeight(void);

void DISPLAY_Goto(uint16_t x, uint16_t y);
void DISPLAY_GotoAbsolute(uint16_t x, uint16_t y);
void DISPLAY_printf(const char* str, ...);
void DISPLAY_puts(const char *s);
void DISPLAY_putc(char c);

#ifdef __cplusplus
}
#endif

#endif
