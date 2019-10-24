#ifndef _board_h_
#define _board_h_


#include <button.h>
#include <lcd.h>
#include <display.h>

#define LCD_W 320
#define LCD_H 240

#define LCD_PORTRAIT  0
#define LCD_LANDSCAPE 1
#define LCD_REVERSE_PORTRAIT  2
#define LCD_REVERSE_LANDSCAPE 3

void LCD_Init(void);
void LCD_Close(void);
void LCD_Rotation(uint8_t m);
void LCD_Fill(uint32_t n, uint16_t color);
void LCD_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void LCD_Pixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_Data(uint16_t color);

extern unsigned char font8x16[];

#endif

