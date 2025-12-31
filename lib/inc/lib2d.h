#ifndef _lib2d_h_
#define _lib2d_h_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <font.h>

#ifndef __EMU__
	#define toupper(c) ( (c)-'a'+'A' )
#endif

enum Attributes{
	FONT_NORMAL,
	FONT_TRANSPARENT,
	FONT_DOUBLE
};

typedef struct lib2d_s{
	const font_t *font;     // Font to be used for char rasterization
	uint16_t forecolor;     // Character/shapes color
	uint16_t backcolor;     // Character/shapes background color
	uint16_t (*drawChar)(uint16_t, uint16_t, uint8_t); // Character rasterization function
	uint16_t cx;            // console x char position
	uint16_t cy;            // console y char position
    uint32_t scratch_offset; // Used for double buffering
	uint16_t sc;            // Scroll offset
    uint16_t text_size;     // Text buffer size
    uint16_t scratch_size;  // Scratch buffer size
    uint16_t *scratch;      // Scratch buffer for double buffering and DMA
    uint8_t *text;          // text buffer
	uint8_t vspace;         // Vertical space between characters on console
    uint8_t scale;          // Character scale
}lib2d_t;

void LIB2D_Init(lib2d_t *);
void LIB2D_Default(lib2d_t *);
void LIB2D_SetFont(const font_t *fnt);
void LIB2D_SetAttribute(uint8_t atr);
void LIB2D_Pixel(uint16_t x0, uint16_t y0);
uint16_t LIB2D_Char(uint16_t  x, uint16_t y, uint8_t c);
uint16_t LIB2D_Int(uint16_t x, uint16_t y, int32_t v, uint8_t radix);
uint16_t LIB2D_Float(uint16_t x, uint16_t y, float f);
uint16_t LIB2D_Text(uint16_t x, uint16_t y, const char *s);
void LIB2D_SetColors(uint16_t fc, uint16_t bc);
void LIB2D_SetFcolor(uint16_t color);
void LIB2D_SetBcolor(uint16_t color);
uint16_t LIB2D_GetFontWidth(void);
uint16_t LIB2D_GetFontHeight(void);
void LIB2D_SetCursor(uint16_t x, uint16_t y);
void LIB2D_SetPos(uint16_t x, uint16_t y);
uint16_t LIB2D_GetCursorX(void);
uint16_t LIB2D_GetCursorY(void);

void LIB2D_Clear(void);
void LIB2D_Rect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h);
void LIB2D_FillRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h);
void LIB2D_FillRoundRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h);
void LIB2D_Line(uint16_t x1, uint16_t y1,  uint16_t x2, uint16_t y2);
void LIB2D_HLine(uint16_t x, uint16_t y, uint16_t width);
void LIB2D_VLine(uint16_t x, uint16_t y, uint16_t height);

void LIB2D_Printf(const char* str, ...);
void LIB2D_String(const char *s);
void LIB2D_Putc(char c);

#ifdef __cplusplus
}
#endif

#endif
