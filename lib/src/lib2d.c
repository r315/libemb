#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include "lib2d.h"
#include "drvlcd.h"
#include "font.h"
#include "strfunc.h"

#define LIB2D_FLOAT_MAX_PRECISION 		8
#define TEXT_BUFFER_SIZE                64
#define SCRATCH_BUFFER_SIZE             (8 * 8 * 2) // double buffer, font dependent

static uint8_t slib2d_text[64];
static uint16_t slib2d_scratch[SCRATCH_BUFFER_SIZE];

static uint16_t drawCharSimple(uint16_t x, uint16_t y, uint8_t c);
static uint16_t drawCharScaled(uint16_t x, uint16_t y, uint8_t c);
static uint16_t drawCharTransparent(uint16_t x, uint16_t y, uint8_t c);

static lib2d_t slib2d = {
    .font = &defaultFont,
	.forecolor = LCD_WHITE,
	.backcolor = LCD_BLACK,
	.vspace = 0,
	.cx = 0,
	.cy = 0,
	.sc = 0,
	.drawChar = drawCharSimple,
    .text = slib2d_text,
    .text_size = sizeof(slib2d_text),
    .scratch_size = sizeof(slib2d_scratch) / 2,
    .scratch_offset = (sizeof(slib2d_scratch) / 4),
    .scratch = slib2d_scratch,
    .scale = 1
};

static lib2d_t *plib2d = &slib2d;

//-----------------------------------------------------------
//
//-----------------------------------------------------------

static uint16_t *swapBuffer(void)
{
    plib2d->scratch_offset ^= (plib2d->scratch_size / 2);
    return plib2d->scratch + plib2d->scratch_offset;
}

/**
 * @brief Draw simple character
 * Rasterize a character bitmap into a pixel buffer.
 * This can handle multiple bytes per row and can be further
 * optimized to write rows until buffer is fill before writing to display
 *
 * @param x
 * @param y
 * @param c
 * @return
 */
static uint16_t drawCharSimple(uint16_t x, uint16_t y, uint8_t c){
	uint8_t w,h;
	uint16_t *pdst;
    const font_t *fnt = plib2d->font;
    const uint8_t *pd = fnt->data + ((c - fnt->offset) * fnt->h * fnt->bpl);

    if(pd < fnt->data || pd > (fnt->data + fnt->data_size - fnt->h)){
        return x;
    }

    // Go through all character rows
    for(h = 0; h < fnt->h; h++){
        uint8_t pxdata, mask;
        pdst = swapBuffer();
        // Get pixel data and mask it for color
        for(mask = 0, w = 0; w < fnt->w; w++, mask >>= 1){
            if(mask == 0){      // Reset mask for every byte on row
                mask = 0x80;
                pxdata = *pd++;
            }
            pdst[w] = (pxdata & mask)  ? plib2d->forecolor : plib2d->backcolor;
        }
        //Write a single row to display
        LCD_WriteArea(x, y + h, fnt->w, 1, pdst);
    }

    // Fill vertical spacing with backcolor, if necessary
    if(fnt->vspace){
        pdst = swapBuffer();
        // Fill buffer with back color
        for(w = 0; w < fnt->w; w++){
            pdst[w] = plib2d->backcolor;
        }
        // Write a row to display vspace times
        for(uint8_t s = 0; s < fnt->vspace; s++, h++){
            LCD_WriteArea(x, y + h, fnt->w, 1, pdst);
        }
    }

	return x + fnt->w;
}

static uint16_t drawCharTransparent(uint16_t x, uint16_t y, uint8_t c)
{
	uint8_t w,h;

	for (h=0; h < plib2d->font->h; h++){
		for(w=0;w<plib2d->font->w; w++){
			if(c & (0x80 >> w))
				LCD_Pixel(x + w, y + h, plib2d->forecolor);
		}
		c += 1;
	}

	return x+(plib2d->font->w);
}

/**
 * @brief Integer‑Scaling Algorithm (Nearest‑Neighbor)
 * This was not tested with fonts larger than 8x8 and
 * does not handle properly fonts with vertical spacing
 * defined
 * @param x
 * @param y
 * @param c
 * @return  Next character x coordinate
 */
static uint16_t drawCharScaled(uint16_t x, uint16_t y, uint8_t ch)
{
    const font_t *fnt = plib2d->font;
    const uint8_t *glyph = fnt->data + ((ch - fnt->offset) * fnt->h * fnt->bpl);
    // Safety: clamp to ASCII range
    if(glyph < fnt->data || glyph > (fnt->data + fnt->data_size - fnt->h)){
        return x;
    }

    uint16_t scaledW = fnt->w * plib2d->scale;
    uint16_t lineY = y;

    for (uint8_t sy = 0; sy < fnt->h; sy++) {
        uint8_t row = glyph[sy];
        // --- Build horizontally scaled line (FAST) ---
        uint16_t *lineBuf = swapBuffer();

        for (uint16_t sx = 0, *tempBuf = lineBuf; sx < fnt->w; sx++) {
            uint16_t color = (row & (0x80 >> sx)) ? plib2d->forecolor : plib2d->backcolor;
            // Write SCALE pixels with wide writes
            for (uint8_t i = 0; i < plib2d->scale; i++) {
                *tempBuf++ = color;
            }
        }

        // --- Output this line SCALE times vertically ---
        for (uint8_t vy = 0; vy < plib2d->scale; vy++) {
            // DMA the prepared line
            LCD_WriteArea(x, lineY, scaledW, 1, lineBuf);
            lineY++;
        }
    }

    return x + scaledW;
}

//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_SetColors(uint16_t fc, uint16_t bc){ plib2d->forecolor = fc; plib2d->backcolor = bc;}
void LIB2D_SetFcolor(uint16_t color){ plib2d->forecolor = color;}
void LIB2D_SetBcolor(uint16_t color){ plib2d->backcolor = color;}
uint16_t LIB2D_GetFontWidth(void){ return plib2d->font->w; }
uint16_t LIB2D_GetFontHeight(void){ return plib2d->font->h; }
uint16_t LIB2D_GetCursorX(void){ return plib2d->cx; }
uint16_t LIB2D_GetCursorY(void){ return plib2d->cy; }
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_SetPos(uint16_t x, uint16_t y)
{
   	plib2d->cx = x;
	plib2d->cy = y;
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void LIB2D_SetCursor(uint16_t x, uint16_t y)
{
	plib2d->cx = x * plib2d->font->w;
	plib2d->cy = y * plib2d->font->h;
}
//----------------------------------------------------------
//
//----------------------------------------------------------
uint16_t LIB2D_Char(uint16_t x, uint16_t y, uint8_t c)
{
	return plib2d->drawChar(x, y, c);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
uint16_t LIB2D_Text(uint16_t x, uint16_t y, const char *s)
{
    while(*s){
        x = LIB2D_Char(x, y, *s++);
    }
	return x;
}

//----------------------------------------------------------
//
//----------------------------------------------------------
uint16_t  LIB2D_Int(uint16_t x, uint16_t y, int32_t v, uint8_t radix)
{
    i2ia((char*)plib2d->text, v, (int)radix, 0);
    return LIB2D_Text(x, y, (const char*)plib2d->text);
}

//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_Putc(char c)
{
	if (c == '\n' || plib2d->cx == LCD_GetWidth()){
		plib2d->cx = 0;
		plib2d->cy += (plib2d->font->h + plib2d->vspace) * plib2d->scale;

        if(plib2d->cy >= LCD_GetHeight()){
			plib2d->cy = 0;
			if(!plib2d->sc)
				plib2d->sc = 1;
		}

        if(plib2d->sc){
			LCD_Scroll(plib2d->cy + plib2d->font->h + plib2d->vspace);
			LCD_FillRect(0, plib2d->cy, LCD_GetWidth(), plib2d->font->h + plib2d->vspace, plib2d->backcolor);
			plib2d->cx = 0;
		}

        if(c == '\n')
			return;
	}

	if(c == '\r'){
		plib2d->cx = 0;
		return;
	}

	plib2d->cx = LIB2D_Char(plib2d->cx, plib2d->cy, c);
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
void LIB2D_String(const char *s)
{
    while(*s)
        LIB2D_Putc(*s++);
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void LIB2D_Printf(const char* fmt, ...){
	va_list arp;

    va_start(arp, fmt);
	strformater((char*)plib2d->text, plib2d->text_size, fmt, arp);
	va_end(arp);

    LIB2D_String((const char*)plib2d->text);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_SetFont(const font_t *fnt)
{
	uint8_t mod, h;
	h = fnt->h + fnt->vspace;
    // Find highest common factor
	do{
        mod = LCD_GetHeight() - ((LCD_GetHeight() / h) * h); // round to nearest integer
		h++;
	}while(mod);
    // Adjust vertical space in order to make character height a
    // display height factor
	plib2d->vspace = (h - 1) - fnt->h;
    plib2d->font = fnt;
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_SetAttribute(uint8_t atr){
	switch(atr){
		default:
		case FONT_NORMAL:
            plib2d->scale = 1;
            plib2d->drawChar = drawCharSimple;
            break;
		case FONT_DOUBLE:
            plib2d->scale = 2;
            plib2d->drawChar = drawCharScaled;
            break;
		case FONT_TRANSPARENT:
            plib2d->scale = 1;
            plib2d->drawChar = drawCharTransparent;
            break;
	}
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_Clear(void)
{
    LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), plib2d->backcolor);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_Pixel(uint16_t x0, uint16_t y0)
{
    LCD_Pixel(x0, y0, plib2d->forecolor);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_HLine(uint16_t x, uint16_t y, uint16_t width)
{
	LCD_FillRect(x, y, width, 1, plib2d->forecolor);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_VLine(uint16_t x, uint16_t y, uint16_t height)
{
	LCD_FillRect(x, y, 1, height, plib2d->forecolor);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_Line(uint16_t x1, uint16_t y1,  uint16_t x2, uint16_t y2)
{
	signed int dy = y2 - y1;
    signed int dx = x2 - x1;
    signed int stepx, stepy;
    signed int fraction;

    if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
    if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }

    if ( x1 == x2 ){
    	LIB2D_VLine(x1, (y1 < y2)? y1 : y2, dy);
    	return;
    }

    if ( y1 == y2 ){
		LIB2D_HLine((x1 < x2)? x1 : x2, y1, dx);
		return;
    }

    dy <<= 1;
    dx <<= 1;

    LCD_Pixel(x1, y1, plib2d->forecolor);

    if (dx > dy)
    {
        fraction = dy - (dx >> 1);
        while (x1 != x2)
        {
            if (fraction >= 0)
            {
                y1 += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;
            LCD_Pixel(x1, y1, plib2d->forecolor);
        }
    }
    else
    {
        fraction = dx - (dy >> 1);
        while (y1 != y2)
        {
            if (fraction >= 0)
            {
                x1 += stepx;
                fraction -= dy;
            }
            y1 += stepy;
            fraction += dx;
            LCD_Pixel(x1, y1, plib2d->forecolor);
        }
    }
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_FillRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h)
{
	LCD_FillRect(x, y, w, h, plib2d->forecolor);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_Rect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h)
{
	LCD_FillRect(x, y, w, 1, plib2d->forecolor);          // Top
	LCD_FillRect(x + w - 1, y + 1, 1, h - 2, plib2d->forecolor);     // Right
	LCD_FillRect(x, y + h - 1, w, 1, plib2d->forecolor);  // Bottom
	LCD_FillRect(x, y + 1, 1, h - 2, plib2d->forecolor);  // Left
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_FillRoundRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h)
{
 	LCD_FillRect(x + 1, y, w-2, h, plib2d->forecolor);
    h -= 2;
    LCD_FillRect(x, y+1, 1, h, plib2d->forecolor);
    LCD_FillRect(x+w-1, y+1, 1, h, plib2d->forecolor);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_Init(lib2d_t *lib2d)
{
    if(lib2d){
        plib2d = lib2d;
    }
}

void LIB2D_Default(lib2d_t *lib2d)
{
    if(lib2d){
        lib2d->font = &defaultFont;
        lib2d->forecolor = LCD_WHITE;
        lib2d->backcolor = LCD_BLACK;
        lib2d->vspace = defaultFont.vspace;
        lib2d->cx = 0;
        lib2d->cy = 0;
        lib2d->sc = 0;
        lib2d->drawChar = drawCharSimple;
        lib2d->scratch_offset = (sizeof(slib2d_scratch) / 4); // buffer will be swapped on first call to LIB2D_Char
        lib2d->text = slib2d_text;
        lib2d->scratch = slib2d_scratch;
        lib2d->scratch_size = sizeof(slib2d_scratch) / 2;
        lib2d->text_size = sizeof(slib2d_text);
        lib2d->scale = 1;
    }
}


