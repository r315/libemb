#ifndef _drvlcd_h_
#define _drvlcd_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "spi.h"
#include "i2c.h"

#define LCD_RED         RGB565(255,0,0)
#define LCD_GREEN       RGB565(0,255,0)
#define LCD_BLUE        RGB565(0,0,255)
#define LCD_BLACK       RGB565(0,0,0)
#define LCD_WHITE       RGB565(255,255,255)
#define LCD_GRAY        RGB565(128,128,128)
#define LCD_YELLOW      RGB565(255,255,0)
#define LCD_ORANGE      RGB565(255,128,0)
#define LCD_CYAN        RGB565(0,255,255)
#define LCD_MAGENTA     RGB565(255,0,255)
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

#ifdef BGR_MODE
    #define RGB565(r,g,b)  (uint16_t)(((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r & 0xF8) >> 3)
#else
    #define RGB565(r,g,b)  (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b & 0xF8) >> 3)
#endif

typedef enum orientation_e{
    LCD_PORTRAIT = 0,
    LCD_LANDSCAPE,
    LCD_REVERSE_PORTRAIT,
    LCD_REVERSE_LANDSCAPE
}orientation_t;

typedef struct drvlcdspi_s {
    uint16_t w;
    uint16_t h;
    uint8_t cs;
    uint8_t cd;
    uint8_t rst;
    uint8_t bkl;
    spibus_t spidev;
}drvlcdspi_t;

typedef struct drvlcdi2c_s {
    uint16_t w;
    uint16_t h;
    uint8_t rst;
    uint8_t bkl;
    i2cbus_t i2cdev;
}drvlcdi2c_t;

typedef struct drvparallel_s {
    uint16_t w;
    uint16_t h;
    uint8_t rst;
    uint8_t bkl;
}drvparallel_s;

typedef struct drvlcd_s{
    void (*init)(void *param);
    void (*drawFillRect)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void (*drawArea)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data);
    void (*drawPixel)(uint16_t x, uint16_t y, uint16_t color);
    void (*scrollScreen)(uint16_t sc);
    void (*setOrientation)(orientation_t m);
    void (*setWindow)(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void (*data)(uint16_t dta);
    void (*setBacklight)(uint8_t state);
    uint16_t (*getWidth)(void);
    uint16_t (*getHeight)(void);
    uint32_t (*getSize)(void);
}drvlcd_t;

#ifdef __cplusplus
}
#endif

#endif