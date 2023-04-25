#ifndef _lcd_h_
#define _lcd_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef BGR_MODE
    #define RGB565(r,g,b)  (uint16_t)((b<<11) | (g<<5) | r)
#else
    #define RGB565(r,g,b)  (uint16_t)(((r>>3)<<11) | ((g>>2)<<5) | (b>>3))
#endif

//-------------------------------------------------------------------
//	basic colors 
//			16bit
//		BBBBBGGGGGGRRRRR
//-------------------------------------------------------------------
/*****************************************************
// color names
// http://www.w3schools.com/html/html_colornames.asp
*****************************************************/
#define LCD_RED         RGB565(255,0,0)
#define LCD_GREEN       RGB565(0,255,0)
#define LCD_BLUE        RGB565(0,0,255)
#define LCD_BLACK       RGB565(0,0,0)
#define LCD_WHITE       RGB565(255,255,255)
#define LCD_GRAY        RGB565(128,128,128)
#define LCD_YELLOW      RGB565(255,255,0)
#define LCD_ORANGE      RGB565(255,128,0)
#define LCD_CYAN        RGB565(0,255,255)
#define LCD_PINK        RGB565(255,0,255)
#define LCD_FBBLUE      RGB565(56,88,152)
#define LCD_SILVER      0xC618
#define LCD_SKYBLUE     0x867D
#define LCD_ROYALBLUE   0x435C
#define LCD_TOMATO      0xFB08
#define LCD_GOLD        0xFEA0
#define LCD_GREENYELLOW 0xAFE5
#define LCD_DIMGRAY     0x6B4D
#define LCD_DARKORANGE  0xFC60
#define LCD_TEAL        0x0410


/**
 * @brief 
 **/
void LCD_Init(void *param);

/**
 * @brief Draws a filled rectangle
 **/
void LCD_FillRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief Draws a filled rectangle with round corners
 **/
void LCD_FillRoundRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief Draws single line rectangle
 **/
void LCD_DrawRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief Opens a window for writing data
 **/
void LCD_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

/**
 * @brief Open a 1x1 px window, addresses the pixel location and write data
 **/
void LCD_Pixel(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief Write single color data.
 * LCD_Window should be called previously
 * */
void LCD_Data(uint16_t data);

/**
 * @brief Write color data buffer
 * LCD_Window should be called previously
 * */
void LCD_Write(uint16_t *data, uint32_t count);
void LCD_WriteArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data);

/**
 * @brief Draws a line
 **/
void LCD_DrawLine(uint16_t x1, uint16_t y1,  uint16_t x2, uint16_t y2, uint16_t color);

/**
 * @brief Draws a Horizontal line
 **/
void LCD_DrawHLine(uint16_t x, uint16_t y, uint16_t width, uint16_t color);

/**
 * @brief Draws a Vertical line
 **/
void LCD_DrawVLine(uint16_t x, uint16_t y, uint16_t height, uint16_t color);

/**
 * @brief 
 **/
void LCD_Clear(uint16_t color);

/**
 * @brief 
 **/
void LCD_Bkl(uint8_t state);

/**
 * @brief
 **/
void LCD_Scroll(uint16_t sc);

/**
 * @brief Get current display Width
 **/
uint16_t LCD_GetWidth(void);

/**
 * @brief Get current display Height
 **/
uint16_t LCD_GetHeight(void);

/**
 * @brief Get total number of pixels
 **/
uint32_t LCD_GetSize(void);

/**
 * @brief Change display orientation
 */
void LCD_SetOrientation(uint8_t m);


#ifdef __cplusplus
};
#endif

#endif