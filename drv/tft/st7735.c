
#include <common.h>
#include <lcd.h>
#include <st7735.h>


static uint16_t _width, _height;


void LCD_Command(uint8_t data){
    LCD_CD0;    
    SPI_Send(data);    
    LCD_CD1;
}

void LCD_Data(uint16_t data){
    LCD_CS0;
#ifdef SPI_16XFER
    SPI_Send16(data);
#else
    SPI_Send(data>>8);
    SPI_Send(data);   
#endif
    LCD_CS1;
}

#define DELAY 0x80
const uint8_t PROGMEM  InitCmd[] = {                 // Init for 7735R, part 1 (red or green tab)
    21,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
    150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
    255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
    0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
    0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
    0x01, 0x2C, 0x2D,       //     Dot inversion mode
    0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
    0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
    0xA2,
    0x02,                   //     -4.6V
    0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
    0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
    0x0A,                   //     Opamp current small
    0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
    0x8A,                   //     BCLK/2, Opamp current small & Medium low
    0x2A,
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
    0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
    0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
    0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
    0x05,
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
    0x00, 0x00,             //     XSTART = 0
    0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
    0x00, 0x00,             //     XSTART = 0
    0x00, 0x9F,
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
    0x02, 0x1c, 0x07, 0x12,
    0x37, 0x32, 0x29, 0x2d,
    0x29, 0x25, 0x2B, 0x39,
    0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
    0x03, 0x1d, 0x07, 0x06,
    0x2E, 0x2C, 0x29, 0x2D,
    0x2E, 0x2E, 0x37, 0x3F,
    0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
    10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
    100
};                  //     100 ms delay


// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void commandList(const uint8_t *addr) {
    uint8_t  numCommands, numArgs;
    uint16_t ms;

    numCommands = pgm_read_byte(addr++);   // Number of commands to follow
    while(numCommands--) {                 // For each command...
        writecommand(pgm_read_byte(addr++)); //   Read, issue command
        numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
        ms       = numArgs & DELAY;          //   If hibit set, delay follows args
        numArgs &= ~DELAY;                   //   Mask out delay bit
        while(numArgs--) {                   //   For each argument...
            writedata(pgm_read_byte(addr++));  //     Read, issue argument
        }

        if(ms) {
            ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
            if(ms == 255) ms = 500;
            myDelay(ms);
        }
    }
}



void LCD_Scroll(uint16_t sc){
    LCD_CS0;
    LCD_Command(VSCRSADD);
#ifdef SPI_16XFER
    SPI_Send16(sc);
#else
    SPI_Send(sc>>8);
    SPI_Send(sc);
#endif
    LCD_CS1;
}

void LCD_Fill(uint32_t n, uint16_t color){
	if(!n) return;
    LCD_CS0;
#ifdef SPI_16XFER
    while(n--)
        SPI_Send16(color);
#elif defined(LCD_DMA)
    LCD_Fill_DMA(n, color);
#else
	while(n--){
		SPI_Send(color>>8);
		SPI_Send(color);
	}
#endif
    LCD_CS1;
}

void LCD_Fill_Data(uint32_t n, uint16_t *data){
	if(!n) return;
    LCD_CS0;
#if defined(LCD_DMA)      // TODO: Validar com TDSO
    LCD_Fill_Data_DMA(n, data);
#else
	while(n--){
		SPI_Send((*data)>>8);
		SPI_Send(*(data++));
	}
#endif
    LCD_CS1;
}

void LCD_IndexedColor(uint16_t *colors, uint8_t *index, uint32_t size){
    LCD_CS0;
#ifdef SPI_16XFER
    while(size--){
        SPI_Send16(colors[*index]);
        index += 1;
    }
#else
    while(size--){
        SPI_Send(colors[*index]>>8);
        SPI_Send(colors[*index]);
        index += 1;
    }
#endif
    LCD_CS1;
}

void LCD_Pixel(uint16_t x, uint16_t y, uint16_t c){
    LCD_CS0;
#ifdef SPI_16XFER
    LCD_Command(CASET);
    SPI_Send16(x);
    SPI_Send16(x);

    LCD_Command(PASET);
    SPI_Send16(y);
    SPI_Send16(y);

    LCD_Command(RAMWR);
#elif defined(LCD_DMA)
    LCD_Window(x,y,1,1);
    LCD_Data(c);
#else
    LCD_Command(CASET);
    SPI_Send(x>>8);
    SPI_Send(x);
    SPI_Send(x>>8);
    SPI_Send(x);

    LCD_Command(PASET);
    SPI_Send(y>>8);
    SPI_Send(y);
    SPI_Send(y>>8);
    SPI_Send(y);

    LCD_Command(RAMWR);
    SPI_Send(c>>8);
    SPI_Send(c);
#endif
    LCD_CS1;
}

void LCD_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
    LCD_CS0;
#ifdef SPI_16XFER
    LCD_Command(CASET);
    SPI_Send16(x);
    SPI_Send16(x + w -1);
  
    LCD_Command(PASET);
    SPI_Send16(y);
    SPI_Send16(y + h - 1);

    LCD_Command(RAMWR);
#elif defined(LCD_DMA)
uint16_t data [2];
	data[0] = x;
	data[1] = x + ( w - 1 );
	LCD_Command(CASET);
	LCD_Fill_Data_DMA(sizeof(data)/sizeof(uint16_t), data);
	data[0] = y;
	data[1] = y + ( h - 1 );
	LCD_Command(PASET);
	LCD_Fill_Data_DMA(sizeof(data)/sizeof(uint16_t), data);
	LCD_Command(RAMWR);
#else
    LCD_Command(CASET);
    SPI_Send(x>>8);
    SPI_Send(x);
    x += w - 1;
    SPI_Send(x>>8);
    SPI_Send(x);
  
    LCD_Command(PASET);
    SPI_Send(y>>8);
    SPI_Send(y);
    y += h - 1;
    SPI_Send(y>>8);
    SPI_Send(y);
    LCD_Command(RAMWR);
#endif
    LCD_CS1;
}

void LCD_Init(void){
   LCD_PIN_INIT;

   LCD_CD0;
   LCD_CS1;
   LCD_BKL0;
   LCD_RST1;
   DelayMs(10);

   LCD_RST0;
   DelayMs(2);
   LCD_RST1;
   DelayMs(5);


   LCD_CS0;

   commandList(InitCmd);

   LCD_CS1;

   _width  = TFT_W;
   _height = TFT_H;
   // user must enable backlight after LCD_Init
}

void LCD_Rotation(uint8_t m) {

    switch (m) {
        case LCD_PORTRAIT:
            m = (MADCTL_MX | MADCTL_MY);
            _width  = TFT_W;
            _height = TFT_H;
            break;
        case LCD_LANDSCAPE:
            m = (MADCTL_MY | MADCTL_MV);
            _width  = TFT_H;
            _height = TFT_W;
            break;
        case LCD_REVERSE_PORTRAIT:
            m = 0;
            _width  = TFT_W;
            _height = TFT_H;
            break;
        case LCD_REVERSE_LANDSCAPE:
            m = (MADCTL_MV | MADCTL_MX);
            _width  = TFT_H;
            _height = TFT_W;
            break;

        default:
         return;
    }
    LCD_CS0;
    LCD_Command(MADCTL);
    SPI_Send(m);
    LCD_CS1;
}

uint16_t LCD_GetWidth(void){
   return _width;
}

uint16_t LCD_GetHeight(void){
   return _height;
}

uint32_t LCD_GetSize(void){
   return _height*_width;
}

void LCD_Bkl(uint8_t state){
    if(state != 0) {LCD_BKL1;}
    else {LCD_BKL0;}
}

