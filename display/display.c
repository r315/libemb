#include <stdarg.h>
#include <stdlib.h>
#include "fonts.c"

int drawCharSimple(int x, int y, unsigned char *d_char);

static Display _display = {
	.xputc = DISPLAY_putc,
	.xgetchar = (char (*)(void))0, 
	.forecolor = WHITE,
	.backcolor = BLACK,
	.font = {8, 8, (const uint8_t*)defaultFontData},
	.vspace = 0,
	.cx = 0,
	.cy = 0,
	.sc = 0,
	.drawChar = drawCharSimple
};

//-----------------------------------------------------------
//
//-----------------------------------------------------------
void DISPLAY_SetColors(uint16_t fc, uint16_t bc){ _display.forecolor = fc; _display.backcolor = bc;}
void DISPLAY_SetFcolor(uint16_t color){ _display.forecolor = color;}
void DISPLAY_SetBcolor(uint16_t color){ _display.backcolor = color;}
int DISPLAY_GetFontWidth(void){ return _display.font.w; }
int DISPLAY_GetFontHeight(void){ return _display.font.h; }

void DISPLAY_GotoAbsolute(uint16_t x, uint16_t y){
   _display.cx = x;
	_display.cy = y;
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void DISPLAY_Goto(uint16_t x, uint16_t y){
	_display.cx = x * _display.font.w;
	_display.cy = y * _display.font.h;
}
//----------------------------------------------------------
//stdout calls this function
//----------------------------------------------------------
void DISPLAY_putc(char c)
{
	if (c == '\n' || _display.cx == LCD_GetWidth()){
		_display.cx = 0;
		_display.cy += _display.font.h + _display.vspace;
		if(_display.cy >= LCD_GetHeight()){
			_display.cy = 0;
			if(!_display.sc)
				_display.sc = 1;
		}
		if(_display.sc){
			LCD_Scroll(_display.cy + _display.font.h + _display.vspace);
			LCD_FillRect(0, _display.cy, LCD_GetWidth(), _display.font.h + _display.vspace, _display.backcolor);		
			_display.cx = 0;
		}
		if(c == '\n')
			return;	
	}
	if(c == '\r'){
		_display.cx = 0;
		return;
	}	
	_display.cx = DISPLAY_Char(_display.cx, _display.cy, c);
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void DISPLAY_puts(const char *s){   
    while(*s)
        DISPLAY_putc(*s++);	
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void xitoa (long val, signed char radix, signed char len){
	unsigned char c, r, sgn = 0, pad = ' ';
	unsigned char s[16], i = 0;
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
	
	if (len > 16) len = 16;
	
	do {
		c = (unsigned char)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	
	if(sgn) 
		s[i++] = sgn;
	
	while (i < len)
		s[i++] = pad;
		
	do{
		DISPLAY_putc(s[--i]);
	}while (i);
}
//-----------------------------------------------------------
//https://en.wikipedia.org/wiki/Single-precision_floating-point_format
//https://wirejungle.wordpress.com/2011/08/06/displaying-floating-point-numbers
//-----------------------------------------------------------
void xftoa(double f, char places){
long int_part, frac_part;
char prec;
  
    int_part = (long)(f);  
    
	if(places > FLOAT_MAX_PRECISION)
		places = FLOAT_MAX_PRECISION;
		
	frac_part = 0;
	prec = 0;
	while ((prec++) < places){
			f *= 10;
			frac_part = (frac_part * 10) + (long)f - ((long)f / 10) * 10;  //((long)f%10);			
	}

    xitoa(int_part, -10, 0);
    DISPLAY_putc('.');   
    xitoa(abs(frac_part), 10, -places);  
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
void DISPLAY_printf(const char* str, ...){
	va_list arp;
	signed char d, r, w, s, l, f;

	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			DISPLAY_putc(d); 
			continue;
		}
		
		d = *str++; 
		w = r = s = l = f = 0;
		
		if(d == '.'){
            f = 1;
			d = *str++; 
		}		
		
		if (d == '0') {  // printf("%2X",3) produces  3, printf("%02X",3) produces in 03
            s = 1;
			d = *str++; 
		}
		
		while ((d >= '0')&&(d <= '9')) {
			w += (w * 10) + (d - '0');
			d = *str++;
		}		
		
		if (d == 'l') {
			l = 1;
			d = *str++;
		}
		
		if (!d) break;
		
		if (d == 's') {
			DISPLAY_puts(va_arg(arp, char*));
			continue;
		}
		
		if (d == 'c') {
			DISPLAY_putc(va_arg(arp, int));
			continue;
		}
		
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16;
		if (d == 'b') r = 2;

		if(d == 'p'){
			DISPLAY_puts("0x");
			r = 16;
		}		
		
		if(d == 'f'){
			if(!f)
				w = 6;						// dafault 6 decimal places
			xftoa(va_arg(arp, double), w);			
			continue;
		}		
		
		if (!r) break;
		
		if (s) w = -w;
		
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
//----------------------------------------------------------
//draws a character without any attributes
//TODO: optimize
//----------------------------------------------------------
int drawCharSimple(int x, int y, unsigned char *d_char){
unsigned char w,h;
	for (h=0; h < _display.font.h; h++){ 	
		for(w=0; w < _display.font.w; w++){
			if(*d_char & (0x80 >> w))
				LCD_Data(_display.forecolor);			
			else
				LCD_Data(_display.backcolor);			
		}
		d_char += 1;
 	}
	return x+(_display.font.w);
}

int drawCharInverted(int x, int y, unsigned char *d_char){
unsigned char w,h;
	for (h=0; h < _display.font.h; h++){ 	
		for(w=0; w < _display.font.h; w++){
			if(*d_char & (0x80 >> w))
				LCD_Data(_display.backcolor);
			else
				LCD_Data(_display.forecolor);
		}
		d_char += 1;		
 	}
	return x+(_display.font.w);
}
int drawCharTransparent(int x, int y, unsigned char *d_char)
{
char w,h;
	for (h=0; h < _display.font.h; h++){
		for(w=0;w<_display.font.w; w++){
			if(*d_char & (0x80 >> w))
				DISPLAY_Pixel(x+w,y+h);							
		}	
		d_char += 1;
	}
	return x+(_display.font.w);
}
int drawCharDouble(int x, int y, unsigned char *d_char)
{
unsigned char w,h;
    LCD_Window(x,y,_display.font.w * 2, _display.font.h * 2);
	for (h=0;h<_display.font.h;h++){			    // altura
		for(w=0;w<_display.font.w;w++){			// primeira linha
			if(*d_char & (0x80 >> w))	// se pixel
			{
				LCD_Data(_display.forecolor);			// desenha 2 px w de FC
				LCD_Data(_display.forecolor);
			}
	        else
			{
    	        LCD_Data(_display.backcolor);			// desenha 2 px w de BC
				LCD_Data(_display.backcolor);
			}
		}
		for(w=0;w<_display.font.w;w++)			// segunda linha igual a primeira
		{
			if(*d_char & (0x80 >> w))
			{
				LCD_Data(_display.forecolor);
				LCD_Data(_display.forecolor);
			}
	        else
			{
    	        LCD_Data(_display.backcolor);					
				LCD_Data(_display.backcolor);
			}
		}
		d_char += 1;
 	}	
	return x+(_display.font.w * 2);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
int DISPLAY_Char(int x, int y, unsigned char c)
{
    c -= 0x20;    
    LCD_Window(x, y, _display.font.w, _display.font.h);	
	return _display.drawChar(x, y, (uint8_t*)_display.font.data + (c * _display.font.h));
}
//----------------------------------------------------------
//
//----------------------------------------------------------
int DISPLAY_Text(int x, int y, const char *s)
{   
    while(*s){
        x = DISPLAY_Char(x,y,*s++);
    }
	return x;
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void *DISPLAY_GetReference(void){
	DISPLAY_Init(OFF);
	return &_display;
}

void DISPLAY_Init(uint8_t initlcd){

	_display.font = defaultFont;
	_display.forecolor = WHITE;
	_display.backcolor = BLACK;
	_display.vspace = 0;
	_display.cx = 0;
	_display.cy = 0;
	_display.sc = 0;
	_display.xputc = DISPLAY_putc;
	_display.drawChar = drawCharSimple;

	LCD_Scroll(_display.cy);

	if(initlcd){
		LCD_Init();
		LCD_Clear(_display.backcolor);
		LCD_Bkl(ON);
	}
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void DISPLAY_SetFont(Font fnt)
{

#ifndef SINGLE_FONT
	uint8_t mod, h;
	_display.font = fnt;

	h = _display.font.h;

	do{
		mod = LCD_GetHeight()  - (LCD_GetHeight() / h) * h;
		h++;
	}while(mod);

	_display.vspace = (h - 1) - _display.font.h;

#endif			
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void DISPLAY_SetAttribute(uint8_t atr){
	switch(atr){
		default:
		case FONT_NORMAL: _display.drawChar = drawCharSimple;break;
		case FONT_INVERTED: _display.drawChar = drawCharInverted;break;
		case FONT_DOUBLE: _display.drawChar = drawCharDouble;break;
		case FONT_TRANSPARENT: _display.drawChar = drawCharTransparent;break;
	}
}
//----------------------------------------------------------
// setwrap(sx,sy,w,h) tem de ser chamado anteriormente
// a ESTE drawPixel(x,y) pixel com cor Foreground color
//----------------------------------------------------------
void DISPLAY_Pixel(int x0, int y0)
{
    LCD_Pixel(x0,y0,_display.forecolor);
}

