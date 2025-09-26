
#include <stddef.h>
#include "ssd1306.h"
#include "drvlcd.h"
#include "i2c.h"


typedef struct ssd13xx_frame{
    uint8_t ctrl;
    uint8_t data[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8];
}ssd13xx_frame_t;


extern void DelayMs(uint32_t ms);

const drvlcd_t ssd13xx_drv = {
    .init = LCD_Init,
    .drawPixel = LCD_Pixel,
    .getWidth = LCD_GetWidth,
    .getHeight = LCD_GetHeight,
    .drawFillRect = LCD_FillRect,
    .setOrientation = LCD_SetOrientation,
    .drawArea = LCD_WriteArea,
    .dataEnd = LCD_Update,
};

static drvlcdi2c_t *drvlcdi2c;
static ssd13xx_frame_t ssd13xx_fb;

static uint8_t ssd1306_waitPowerUp(void)
{
    uint16_t data = (SSD1306_NOP << 8);
    uint32_t count = SSD1306_ACK_RETRIES;

    while(count--){
        DelayMs(100);
        if(I2C_Write(drvlcdi2c->i2cdev, SSD1306_I2C_ADDRESS, (uint8_t*)&data, 2) == 2)
            return 1;
    }
    return 0;
}

static void ssd13xx_command(uint8_t c)
{
    uint8_t data[2];

	data[0] = 0x00;   // Co = 0, D/C = 0
	data[1] = c;

    I2C_Write(drvlcdi2c->i2cdev, SSD1306_I2C_ADDRESS, data, 2);
}


uint8_t LCD_Init(void *param){

    drvlcdi2c = (drvlcdi2c_t*)param;

    if(drvlcdi2c == NULL || !ssd1306_waitPowerUp()){
        return 0;
    }

    drvlcdi2c->fb = (uint8_t*)&ssd13xx_fb.data;

    // Common init commands 1306/1315
    ssd13xx_command(SSD1306_SETMULTIPLEX);
    ssd13xx_command(drvlcdi2c->h - 1);

    ssd13xx_command(SSD1306_CHARGEPUMP);
    ssd13xx_command(0x14);                         // 7.5V

    ssd13xx_command(SSD1306_MEMORYMODE);
    ssd13xx_command(0x00);                         // 0x0 Horizontal addresssing mode (act like ks0108)

    ssd13xx_command(SSD1306_SETSTARTLINE | 0x0);   // line #0

    ssd13xx_command(SSD1306_SEGREMAP | 0x0);       // 0: SEG0=COL0, 1:SEG0=COL127

    ssd13xx_command(SSD1306_COMSCAN | 0x0);        // 0: COM0->COM63 8: COM63->COM0

    ssd13xx_command(SSD1306_SETCONTRAST);
    ssd13xx_command(0x8F);                         // 0x01 to 0xFF

    ssd13xx_command(SSD1306_DEACTIVATE_SCROLL);

    for(uint16_t i = 0; i < drvlcdi2c->w * drvlcdi2c->h / 8; i++){
        drvlcdi2c->fb[i] = 0x0;
    }

    ssd13xx_command(SSD1306_DISPLAYON);

    return 1;
}

void LCD_Pixel(uint16_t x, uint16_t y, uint16_t color)
{
    uint32_t offset = (y >> 3) * drvlcdi2c->w + x;
    uint8_t mask = 0x01 << (y & 7);
    uint8_t data = drvlcdi2c->fb[offset];

    switch (color) {
        case WHITE:   data |=  mask; break;
        case BLACK:   data &=  ~mask; break;
        case INVERSE: data ^=  mask; break;
    }

    drvlcdi2c->fb[offset] = data;
}

void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    for(uint16_t i = y; i < y + h; i++){
        uint16_t page = (i >> 3) * drvlcdi2c->w;
        uint8_t mask = 1 << (i & 7);
        for(uint16_t j = x; j < x + w; j++){
            uint8_t data = drvlcdi2c->fb[page + j];
            switch (color) {
                case WHITE:   data |=  mask; break;
                case BLACK:   data &=  ~mask; break;
                case INVERSE: data ^=  mask; break;
            }
            drvlcdi2c->fb[page + j] = data;
        }
    }
}

void LCD_WriteArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *col_data)
{
    uint8_t *bitmap = (uint8_t*)col_data;

    for(uint16_t i = y; i < y + h; i++){
        uint16_t page = (i >> 3) * drvlcdi2c->w;
        uint8_t page_mask = 1 << (i & 7);
        uint8_t pixel_mask = 0;
        for(uint16_t j = x; j < x + w; j++){
            if((pixel_mask >>= 1) == 0){
                pixel_mask = 0x80;
                bitmap++;
            }
            uint8_t page_data = drvlcdi2c->fb[page + j];
            drvlcdi2c->fb[page + j] = (*bitmap & pixel_mask) ?
                        page_data | page_mask :
                        page_data & ~page_mask;;
        }
    }
}

void LCD_SetOrientation(drvlcdorientation_t m)
{
    switch (m) {
    case LCD_LANDSCAPE:
        ssd13xx_command(SSD1306_SEGREMAP | 0x0);       // 0: SEG0=COL0, 1:SEG0=COL127
        ssd13xx_command(SSD1306_COMSCAN | 0x0);        // 0: COM0->COM63 8: COM63->COM0
        break;
    case LCD_REVERSE_LANDSCAPE:
        ssd13xx_command(SSD1306_SEGREMAP | 0x1);       // 0: SEG0=COL0, 1:SEG0=COL127
        ssd13xx_command(SSD1306_COMSCAN | 0x8);        // 0: COM0->COM63 8: COM63->COM0
        break;
    default:
        break;
    }
}

uint16_t LCD_GetWidth(void)
{
	return drvlcdi2c->w;
}

uint16_t LCD_GetHeight(void)
{
	return drvlcdi2c->h;
}

void LCD_Update(void)
{
    ssd13xx_command(SSD1306_COLUMNADDR);
    ssd13xx_command(0);
    ssd13xx_command(drvlcdi2c->w - 1);

    ssd13xx_command(SSD1306_PAGEADDR);
    ssd13xx_command(0);
    ssd13xx_command(drvlcdi2c->h / 8 - 1);

    ssd13xx_fb.ctrl = SSD13xx_CTRL_DC;
    if(drvlcdi2c->i2cdev->cfg & I2C_CFG_DMA){
        I2C_TransmitDMA(drvlcdi2c->i2cdev, SSD1306_I2C_ADDRESS, (uint8_t*)&ssd13xx_fb, drvlcdi2c->w * drvlcdi2c->h / 8 + 1);
    }else{
        I2C_Write(drvlcdi2c->i2cdev, SSD1306_I2C_ADDRESS, (uint8_t*)&ssd13xx_fb, drvlcdi2c->w * drvlcdi2c->h / 8 + 1);
    }
}

void LCD_Invert(uint8_t on)
{
    ssd13xx_command( on ?
        SSD1306_INVERTDISPLAY :
        SSD1306_NORMALDISPLAY);
}

void LCD_SetComPin(uint8_t compin)
{
    ssd13xx_command(SSD1306_SETCOMPINS);
    ssd13xx_command((compin & 0x30) | 0x02);
}