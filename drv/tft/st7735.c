#include <st7735.h>
#include <board.h>
#include <lcd.h>

//--------------------------------
#ifndef TFT_OFFSET_SOURCE
#define TFT_OFFSET_SOURCE	0
#endif

#ifndef TFT_OFFSET_GATE
#define TFT_OFFSET_GATE		0
#endif

#ifdef TFT_BGR_FILTER
// Applys for TFT's with BGR filter or IPS
#define DEFAULT_MADC		0x08
#else
#define DEFAULT_MADC		0x00
#endif

#define STATE_LANDSCAPE		(1<<0)
#define STATE_INITIALYZED	(1<<1)
#define pgm_read_byte(x) 	*((uint8_t*)(x))

#define DELAY 				0x80

static uint16_t _width, _height, _color;
static uint8_t madd, start_x, start_y, state;

#if TFT_W == 128
// Can't remember where I got this ...
// Init for 7735R, part 1 (red or green tab)
const uint8_t InitCmd[] = {
		17,                       //  17 commands in list:
		ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
		150,                      //     150 ms delay
		ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
		255,                      //     500 ms delay
		ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
		0x01, 0x2C, 0x2D,         //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
		ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
		0x01, 0x2C, 0x2D,         //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
		ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
		0x01, 0x2C, 0x2D,         //     Dot inversion mode
		0x01, 0x2C, 0x2D,         //     Line inversion mode
		ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
		0x07,                     //     No inversion
		ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
		0xA2,
		0x02,                     //     -4.6V
		0x84,                     //     AUTO mode
		ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
		0xC5,                     //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
		ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
		0x0A,                     //     Opamp current small
		0x00,                     //     Boost frequency
		ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
		0x8A,                     //     BCLK/2, Opamp current small & Medium low
		0x2A,
		ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
		0x8A, 0xEE,
		ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
		0x0E,
		ST7735_COLMOD , 1      ,  // 13: set color mode, 1 arg, no delay:
		0x05,
		ST7735_GMCTRP1, 16     ,  // 14: Magical unicorn dust, 16 args, no delay:
		0x02, 0x1c, 0x07, 0x12,
		0x37, 0x32, 0x29, 0x2d,
		0x29, 0x25, 0x2B, 0x39,
		0x00, 0x01, 0x03, 0x10,
		ST7735_GMCTRN1, 16     ,  // 15: Sparkles and rainbows, 16 args, no delay:
		0x03, 0x1d, 0x07, 0x06,
		0x2E, 0x2C, 0x29, 0x2D,
		0x2E, 0x2E, 0x37, 0x3F,
		0x00, 0x00, 0x02, 0x10,
		ST7735_NORON  ,    DELAY, // 16: Normal display on, no args, w/delay
		10,                       //      10 ms delay
		ST7735_DISPON ,    DELAY, // 17: Main screen turn on, no args w/delay
		100						  //      100 ms delay
};
#elif TFT_W == 80
/**
 * Values for 132x162 GRAM, visible 80x160
 * Looks good on small 80x160 IPS display
 */
const uint8_t InitCmd [] = {
	18,                       //  18 commands in list:
	ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
	120,                      //     120ms delay
	ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
	255,                      //     500 ms delay	
	ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
	0x05, 0x3A, 0x3A,         //     Rate = fosc/([05]x2+40) * (LINE+[3A]+[3A]+2)
	ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
	0x05, 0x3A, 0x3A,         //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
	ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
	0x05, 0x3A, 0x3A,         //     Dot inversion mode
	0x05, 0x3A, 0x3A,         //     Line inversion mode
	ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
	0x07,                     //     No inversion
	ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
	0xA8,                     //     AVDD = 5V, GVDD = 4.3V
	0x08,                     //     GVCL = -4.3V
	0x84,                     //     AUTO mode
	ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
	0xC0,                     //     VGH25 = 2.4V, VGSEL = -7.5V, VGH = 11.9V (2 * AVDD + VGH25 - 0.5)
	ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
	0x0D,                     //     Opamp current: small, medium low
	0x00,                     //     Boost frequency
	ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
	0x8D,                     //     BCLK/2, Opamp current small & Medium low
	0x2A,                     //
	ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
	0x8D,                     //
	0xEE,                     //
	ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
	0x1A,                     //
	ST7735_GMCTRP1, 16     ,  // 13: Magical unicorn dust, 16 args, no delay:
	0x03, 0x22, 0x07, 0x0A,
	0x2E, 0x30, 0x25, 0x2A,
	0x28, 0x26, 0x2E, 0x3A,
	0x00, 0x01, 0x03, 0x13,
	ST7735_GMCTRN1, 16     ,  // 14: Sparkles and rainbows, 16 args, no delay:
	0x04, 0x16, 0x06, 0x0D,
	0x2D, 0x26, 0x23, 0x27,
	0x27, 0x25, 0x2D, 0x3B,
	0x00, 0x01, 0x04, 0x13,
	ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
	0x05,                     //     16-bit/pixel
	ST7735_NORON  ,    DELAY, // 16: Normal display on, no args, w/delay
	10,                       //     10 ms delay
#ifdef TFT_BGR_FILTER
	ST7735_INVON, 0        ,  // 17: invert display, no args, no delay
#else
	ST7735_INVOFF, 0       ,  // 17: Don't invert display, no args, no delay
#endif
	ST7735_DISPON ,    DELAY, // 18: Main screen turn on, no args w/delay
	100						  //     100 ms delay
};
#endif

/**
 * @brief Writes command to display
 */
static void LCD_Command(uint8_t data){
	LCD_CD0; 
	SPI_Send(data);
	LCD_CD1;
}

/**
 * @brief Write single data 16bit in Big-endian
 *
 */
inline void LCD_Data(uint16_t data){
	SPI_Send(data>>8);
	SPI_Send(data);
}

/*
 * @brief Companion code to the above tables.  Reads and issues
 * a series of LCD commands stored as byte array.
 * */
void LCD_CommandList(const uint8_t *addr) {
	uint8_t  numCommands, numArgs;
	uint16_t ms;

	numCommands = pgm_read_byte(addr++);       // Number of commands to follow
	while(numCommands--) {                     // For each command...
		LCD_Command(pgm_read_byte(addr++));   //   Read, issue command
		numArgs  = pgm_read_byte(addr++);      //   Number of args to follow
		ms       = numArgs & DELAY;            //   If hibit set, delay follows args
		numArgs &= ~DELAY;                     //   Mask out delay bit
		while(numArgs--) {                     //   For each argument...
			SPI_Send(pgm_read_byte(addr++));  //   Read, issue argument
		}

		if(ms) {
			ms = pgm_read_byte(addr++);        // Read post-command delay time (ms)
			if(ms == 255) ms = 500;
			DelayMs(ms);
		}
	}
}

void LCD_Scroll(uint16_t sc){
	LCD_CS0;
	LCD_Command(ST7735_VSCSAD);
	LCD_Data(sc);
	LCD_CS1;
}

void LCD_Fill(uint32_t count, uint16_t color){
	if(!count) return;
	LCD_CS0;
#ifdef SPI_BLOCK_DMA
	_color = color;
	SPI_WriteDMA(&_color, count | 0x80000000);
#else
	while(count--){
		LCD_Data(color);
	}
#endif
	LCD_CS1;
}

/**
 * @brief Write a block of data
 * TODO: test block transfer on endian
 */
void LCD_Write(uint16_t *data, uint32_t count){
	if(count == 0) return;
	LCD_CS0;
#ifdef SPI_BLOCK_DMA
	SPI_WriteDMA(data, count);
#else
	while(count--){
		LCD_Data(*(data++));
	}
#endif
	LCD_CS1;
}

void LCD_Pixel(uint16_t x, uint16_t y, uint16_t color){

	x += start_x;
	y += start_y;

	LCD_CS0;

#ifdef _SPI_BLOCK_DMA
	uint32_t buf;
	buf = (x << 24) | (x << 8) | (x >> 8);
	LCD_Command(ST7735_CASET);
	SPI_Write((uint8_t*)&buf, 4);

	buf = (y << 24) | (y << 8) | (y >> 8);
	LCD_Command(ST7735_RASET);
	SPI_Write((uint8_t*)&buf, 4);

	buf = (color >> 8) | (color << 8);
	LCD_Command(ST7735_RAMWR);
	SPI_Write((uint8_t*)&buf, 2);
#else
	LCD_Command(ST7735_CASET);	
	LCD_Data(x);
	LCD_Data(x);

	LCD_Command(ST7735_RASET);	
	LCD_Data(y);
	LCD_Data(y);

	LCD_Command(ST7735_RAMWR);
	LCD_Data(color);
#endif
	LCD_CS1;
}

void LCD_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h){

	x += start_x;
	y += start_y;

	LCD_CS0;

	LCD_Command(ST7735_CASET);
	LCD_Data(x);
	LCD_Data(x + w - 1);

	LCD_Command(ST7735_RASET);
	LCD_Data(y);	
	LCD_Data(y + h - 1);

	LCD_Command(ST7735_RAMWR);
	LCD_CS1;
}

void LCD_Init(void){

	//LCD_PIN_INIT; made on board level

	LCD_CD0;
	LCD_CS1;
	LCD_BKL0;
	LCD_RST1;
	DelayMs(10);

	LCD_RST0;
	DelayMs(2);
	LCD_RST1;
	DelayMs(5);

	madd = DEFAULT_MADC;

	LCD_CS0;
	LCD_CommandList(InitCmd);
	LCD_Command(ST7735_MADCTL);   // row addr/col addr, bottom to top refresh
	SPI_Send(madd);
	LCD_CS1;

	// Set offset
	start_x = TFT_OFFSET_SOURCE;
	start_y = TFT_OFFSET_GATE;
	_width  = TFT_W;
	_height = TFT_H;
	state = STATE_INITIALYZED;
}

/**
 * Not tested DO pin is not available
 */
uint32_t LCD_GetId(void){
	uint32_t id;
	LCD_CS0;
	LCD_Command(0x04);
	SPI_Read((uint8_t*)&id, 4);
	LCD_CS1;
	return id;
}

void LCD_Rotation(uint8_t m) {

	madd &= ~(MADCTL_MY | MADCTL_MX | MADCTL_MV);

	switch (m) {
	case LCD_PORTRAIT:
		madd |= (MADCTL_MX | MADCTL_MY);
		state &= ~(STATE_LANDSCAPE);
		break;
	case LCD_LANDSCAPE:
		madd |= (MADCTL_MY | MADCTL_MV);
		state |= (STATE_LANDSCAPE);
		break;
	case LCD_REVERSE_PORTRAIT:
		state &= ~(STATE_LANDSCAPE);
		//madd |= 0;
		break;
	case LCD_REVERSE_LANDSCAPE:
		state |= (STATE_LANDSCAPE);
		madd |= (MADCTL_MV | MADCTL_MX);
		break;

	default:
		return;
	}

	if(state & STATE_LANDSCAPE){
		start_x = TFT_OFFSET_GATE;
		start_y = TFT_OFFSET_SOURCE;
		_width  = TFT_H;
		_height = TFT_W;
	}else{
		start_x = TFT_OFFSET_SOURCE;
		start_y = TFT_OFFSET_GATE;
		_width  = TFT_W;
		_height = TFT_H;
	}

	LCD_CS0;
	LCD_Command(ST7735_MADCTL);
	SPI_Send(madd);
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
