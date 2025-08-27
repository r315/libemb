#ifndef _ssd13xx_h_
#define _ssd13xx_h_

#include <stdint.h>
#include "drvlcd.h"

#define SSD1306_I2C_ADDRESS   			0x3C // 7-bit address

#define SSD1306_LCDWIDTH                128
#define SSD1306_LCDHEIGHT               64

/* Commands */
#define SSD1306_SETLOWCOLUMN 			0x00
#define SSD1306_SETHIGHCOLUMN 			0x10
#define SSD1306_MEMORYMODE 				0x20
#define SSD1306_COLUMNADDR 				0x21
#define SSD1306_PAGEADDR   				0x22
#define SSD1306_SETSTARTLINE 			0x40
#define SSD1306_SETCONTRAST 			0x81
#define SSD1306_CHARGEPUMP 				0x8D
#define SSD1306_SEGREMAP 				0xA0
#define SSD1306_DISPLAYALLON_RESUME		0xA4
#define SSD1306_DISPLAYALLON 			0xA5
#define SSD1306_NORMALDISPLAY 			0xA6
#define SSD1306_INVERTDISPLAY 			0xA7
#define SSD1306_SETMULTIPLEX 			0xA8
#define SSD1306_DISPLAYOFF 				0xAE
#define SSD1306_DISPLAYON 				0xAF
#define SSD1306_COMSCAN                 0xC0
#define SSD1306_SETDISPLAYOFFSET 		0xD3
#define SSD1306_SETCOMPINS 				0xDA
#define SSD1306_SETVCOMDETECT 			0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 		0xD5
#define SSD1306_SETPRECHARGE 			0xD9
#define SSD1306_NOP			 			0xE3


/* Commands Bits */
#define SSD1306_EXTERNALVCC 			0x1
#define SSD1306_SWITCHCAPVCC 			0x2
#define SSD13xx_CTRL_Co                 0x80    // Continuation bit
#define SSD13xx_CTRL_DC                 0x40    // Data/Command bit

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 		0x2F
#define SSD1306_DEACTIVATE_SCROLL 		0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 	0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A

#define SSD1306_ACK_RETRIES             50;


enum{
    BLACK = 0,
    WHITE,
    INVERSE
};

extern const drvlcd_t ssd13xx_drv;

void LCD_Update(void);

#endif
