#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include "liblcd.h"
#include "lib2d.h"
#include "font.h"
#include "strfunc.h"

#define LIB2D_FLOAT_MAX_PRECISION 		8
#define SCRATCH_BUFFER_SIZE             (8 * 8 * 2) // double buffer, font dependent

typedef struct lib2d_s{
	uint16_t forecolor;
	uint16_t backcolor;
	const font_t *font;
	uint16_t cx;
	uint16_t cy;
	uint16_t sc;
	uint8_t vspace;
	uint16_t (*drawChar)(uint16_t  x, uint16_t y, uint8_t *char_data, uint16_t *buffer);
    uint32_t buffer_offset;
    uint8_t txt_buf[64];
    uint16_t buffer[SCRATCH_BUFFER_SIZE]; 
}lib2d_t;

static lib2d_t lib2d;
//-----------------------------------------------------------
//
//-----------------------------------------------------------
//----------------------------------------------------------
//draws a character without any attributes
//TODO: optimize
//----------------------------------------------------------
static uint16_t drawCharSimple(uint16_t x, uint16_t y, uint8_t *char_data, uint16_t *buffer){
	uint8_t w,h;
	uint16_t *pbuf = buffer;
	
	for (h=0; h < lib2d.font->h; h++){ 	
		for(w=0; w < lib2d.font->w; w++){			
			*pbuf++ = (*char_data & (0x80 >> w)) ? lib2d.forecolor : lib2d.backcolor;
		}
		char_data += 1;
 	}

	LCD_WriteArea(x, y, lib2d.font->w, lib2d.font->h, buffer);
	return x + (lib2d.font->w);
}

static uint16_t drawCharTransparent(uint16_t x, uint16_t y, uint8_t *char_data, uint16_t *buffer)
{
	uint8_t w,h;
    
    (void)buffer;

	for (h=0; h < lib2d.font->h; h++){
		for(w=0;w<lib2d.font->w; w++){
			if(*char_data & (0x80 >> w))
				LCD_Pixel(x + w, y + h, lib2d.forecolor);							
		}	
		char_data += 1;
	}

	return x+(lib2d.font->w);
}

static uint16_t drawCharDouble(uint16_t x, uint16_t y, uint8_t *char_data, uint16_t *buffer)
{
	uint32_t w, h, z;
    uint16_t *p1, *p2;

    (void)buffer;

    lib2d.buffer_offset = 0; // Access full buffer

    for(z = 0; z < 4; z++){  // But use half each iteration (4 * 16 pixel)
        p1 = lib2d.buffer + lib2d.buffer_offset;
        p2 = p1 + lib2d.font->w * 2;

        for (h = 0; h < lib2d.font->h / 4; h++){
            for(w = 0; w < lib2d.font->w; w++){
                if(*char_data & (0x80 >> w))
                {
                    *p1++ = lib2d.forecolor;
                    *p1++ = lib2d.forecolor;
                    *p2++ = lib2d.forecolor;
                    *p2++ = lib2d.forecolor;
                }else{
                    *p1++ = lib2d.backcolor;
                    *p1++ = lib2d.backcolor;
                    *p2++ = lib2d.backcolor;
                    *p2++ = lib2d.backcolor;
                }
            }
            p1 += lib2d.font->w * 2;
            p2 += lib2d.font->w * 2;
            char_data += 1;
        }

        LCD_WriteArea(x, y, lib2d.font->w * 2, lib2d.font->h / 2, lib2d.buffer + lib2d.buffer_offset);
        y += 4;        
        lib2d.buffer_offset ^= (SCRATCH_BUFFER_SIZE / 2); // swap buffer
    }

	return x + (lib2d.font->w * 2);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_SetColors(uint16_t fc, uint16_t bc){ lib2d.forecolor = fc; lib2d.backcolor = bc;}
void LIB2D_SetFcolor(uint16_t color){ lib2d.forecolor = color;}
void LIB2D_SetBcolor(uint16_t color){ lib2d.backcolor = color;}
uint16_t LIB2D_GetFontWidth(void){ return lib2d.font->w; }
uint16_t LIB2D_GetFontHeight(void){ return lib2d.font->h; }
uint16_t LIB2D_GetCursorX(void){ return lib2d.cx; }
uint16_t LIB2D_GetCursorY(void){ return lib2d.cy; }
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_SetPos(uint16_t x, uint16_t y)
{
   	lib2d.cx = x;
	lib2d.cy = y;
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void LIB2D_SetCursor(uint16_t x, uint16_t y)
{
	lib2d.cx = x * lib2d.font->w;
	lib2d.cy = y * lib2d.font->h;
}
//----------------------------------------------------------
//
//----------------------------------------------------------
uint16_t LIB2D_Char(uint16_t x, uint16_t y, uint8_t c)
{
    c -= 0x20;
	lib2d.buffer_offset ^= (SCRATCH_BUFFER_SIZE / 2); // swap buffer
	return lib2d.drawChar(x, y, (uint8_t*)lib2d.font->data + (c * lib2d.font->h), lib2d.buffer + lib2d.buffer_offset);
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
void LIB2D_Putc(char c)
{
	if (c == '\n' || lib2d.cx == LCD_GetWidth()){
		lib2d.cx = 0;
		lib2d.cy += lib2d.font->h + lib2d.vspace;
		if(lib2d.cy >= LCD_GetHeight()){
			lib2d.cy = 0;
			if(!lib2d.sc)
				lib2d.sc = 1;
		}
		if(lib2d.sc){
			LCD_Scroll(lib2d.cy + lib2d.font->h + lib2d.vspace);
			LCD_FillRect(0, lib2d.cy, LCD_GetWidth(), lib2d.font->h + lib2d.vspace, lib2d.backcolor);		
			lib2d.cx = 0;
		}
		if(c == '\n')
			return;	
	}
	if(c == '\r'){
		lib2d.cx = 0;
		return;
	}	
	lib2d.cx = LIB2D_Char(lib2d.cx, lib2d.cy, c);
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
	strformater((char*)lib2d.txt_buf, fmt, arp);
	va_end(arp);

    LIB2D_String((const char*)lib2d.txt_buf);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_SetFont(font_t *fnt)
{

#ifndef SINGLE_FONT
	uint8_t mod, h;
	lib2d.font = fnt;

	h = lib2d.font->h;

	do{
		mod = LCD_GetHeight()  - (LCD_GetHeight() / h) * h;
		h++;
	}while(mod);

	lib2d.vspace = (h - 1) - lib2d.font->h;

#endif			
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_SetAttribute(uint8_t atr){
	switch(atr){
		default:
		case FONT_NORMAL: lib2d.drawChar = drawCharSimple;break;
		case FONT_DOUBLE: lib2d.drawChar = drawCharDouble;break;
		case FONT_TRANSPARENT: lib2d.drawChar = drawCharTransparent;break;
	}
}
//----------------------------------------------------------
// 
//----------------------------------------------------------
void LIB2D_Clear(void)
{	
    LCD_FillRect(0, 0, LCD_GetWidth(), LCD_GetHeight(), lib2d.backcolor); 
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_Pixel(uint16_t x0, uint16_t y0)
{
    LCD_Pixel(x0, y0, lib2d.forecolor);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_HLine(uint16_t x, uint16_t y, uint16_t width)
{
	LCD_FillRect(x, y, width, 1, lib2d.forecolor);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_VLine(uint16_t x, uint16_t y, uint16_t height)
{
	LCD_FillRect(x, y, 1, height, lib2d.forecolor);
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
    	LCD_VLine(x1, (y1 < y2)? y1 : y2, dy, lib2d.forecolor);
    	return;
    }

    if ( y1 == y2 ){
		LCD_HLine((x1 < x2)? x1 : x2, y1, dx, lib2d.forecolor);
		return;
    }

    dy <<= 1;
    dx <<= 1;
    
    LCD_Pixel(x1, y1, lib2d.forecolor);

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
            LCD_Pixel(x1, y1, lib2d.forecolor);
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
            LCD_Pixel(x1, y1, lib2d.forecolor);            
        }
    }  
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_FillRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h)
{
	LCD_FillRect(x, y, w, h, lib2d.forecolor);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_Rect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h)
{
	LCD_FillRect(x, y, w, 1, lib2d.forecolor);	
	LCD_FillRect(x + w , y, 1, h+1, lib2d.forecolor);	
	LCD_FillRect(x, y + h , w, 1, lib2d.forecolor);	
	LCD_FillRect(x ,y ,1, h, lib2d.forecolor);   
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_FillRoundRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h)
{
 	LCD_FillRect(x + 1, y, w-2, h, lib2d.forecolor);
    h -= 2;
    LCD_FillRect(x, y+1, 1, h, lib2d.forecolor);
    LCD_FillRect(x+w-1, y+1, 1, h, lib2d.forecolor);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void LIB2D_Init(void)
{
	lib2d.font = &defaultFont;
	lib2d.forecolor = LCD_WHITE;
	lib2d.backcolor = LCD_BLACK;
	lib2d.vspace = 0;
	lib2d.cx = 0;
	lib2d.cy = 0;
	lib2d.sc = 0;
	lib2d.drawChar = drawCharSimple;

    lib2d.buffer_offset = (SCRATCH_BUFFER_SIZE / 2);  // buffer will be swapped on first call to LIB2D_Char
}



