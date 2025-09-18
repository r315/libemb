#include <stddef.h>
#include "ili9341.h"
#include "drvlcd.h"
#include "spi.h"
#include "gpio.h"

#define DELAY 			0x0080

#define LCD_CS1     GPIO_Write(drvlcd->cs, GPIO_PIN_HIGH)
#define LCD_CS0     GPIO_Write(drvlcd->cs, GPIO_PIN_LOW)
#define LCD_CD1     GPIO_Write(drvlcd->cd, GPIO_PIN_HIGH)
#define LCD_CD0     GPIO_Write(drvlcd->cd, GPIO_PIN_LOW)
#define LCD_BKL1    GPIO_Write(drvlcd->bkl, GPIO_PIN_HIGH)
#define LCD_BKL0    GPIO_Write(drvlcd->bkl, GPIO_PIN_LOW)
#define LCD_RST1    GPIO_Write(drvlcd->rst, GPIO_PIN_HIGH)
#define LCD_RST0    GPIO_Write(drvlcd->rst, GPIO_PIN_LOW)

static uint16_t _width, _height;
static spibus_t *spibus;
static drvlcdspi_t *drvlcd;
static uint8_t scratch[4];

const drvlcd_t ili9341_drv =
{
    LCD_Init,
    LCD_FillRect,
    LCD_WriteArea,
    LCD_Pixel,
    LCD_Scroll,
    LCD_SetOrientation,
    LCD_Window,
    LCD_Data,
    LCD_Bkl,
    LCD_GetWidth,
    LCD_GetHeight,
    LCD_GetSize,
    LCD_DataEnd
};

extern void DelayMs(uint32_t ms);

static const uint8_t ili9341_init_seq[] =
{
    16,     // Number of commands
    ILI9341_PCONB, 3, 0x00, 0xC1, 0x30,
    ILI9341_PSCON, 4, 0x64, 0x03, 0x12, 0x81,
    ILI9341_DTCONA, 3, 0x85, 0x00, 0x78,
    ILI9341_PCONA, 5, 0x39, 0x2c, 0x00, 0x34, 0x02,
    ILI9341_PRCON, 1, 0x20,
    ILI9341_DTCONB, 2, 0x00, 0x00,
    ILI9341_PCON1, 1, 0x23,
    ILI9341_PCON2, 1, 0x10,
    ILI9341_VCOM1, 2, 0x3e, 0x28,
    ILI9341_VCOM2, 1, 0x86,
    ILI9341_MADCTL, 1, ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR,
    ILI9341_COLMOD, 1, 0x55,
    ILI9341_FRCONN, 2, 0x00, 0x18,
    ILI9341_DFCTL, 4, 0x08, 0x82, 0x27, 0x00,
    ILI9341_SLPOUT, DELAY, 120,
    ILI9341_DISPON, DELAY, 5
};

static void LCD_Command(uint8_t cmd){
    LCD_CD0;
    SPI_Transfer(spibus, &cmd, 1);
    LCD_CD1;
}

void LCD_Data(uint16_t data){
    // MSB first
    scratch[0] = data >> 8;
    scratch[1] = data;
	SPI_Transfer(spibus, scratch, 2);
}

/**
 * @brief Companion code to the above tables.  Reads and issues
 * a series of LCD commands stored as byte array.
 * */
static void LCD_InitSequence(const uint8_t *addr) {
	uint8_t  numCommands, numArgs;
	uint16_t ms;

	numCommands = *addr++;          // Get total number os commands
	while(numCommands--) {
		LCD_Command(*addr++);       // Send command
		numArgs  = *addr++;         // Get number of args
		ms       = numArgs;         // Get argument
		numArgs &= ~DELAY;          // Clear delay flag
		SPI_Transfer(spibus, (uint8_t*)addr, numArgs); // Send arguments
		addr += numArgs;            // Move to next command

		if(ms & DELAY) {            // If argument was delay, do it
			ms = *addr++;
			if(ms == 255) ms = 500;
			DelayMs(ms);
		}
	}
}


static void LCD_WriteData(uint16_t *data, uint32_t count){

	if(spibus->cfg & SPI_CFG_DMA){
        spibus->cfg |= SPI_CFG_TRF_16BIT;
		SPI_TransferDMA(spibus, (uint8_t*)data, count);
		//LCD_CS1; // SET by DMA handler
	}else{
		while(count--)
			LCD_Data(*data++);
		LCD_CS1;
	}
}

void LCD_DataEnd(void){
    spibus->cfg &= ~(SPI_CFG_TRF_16BIT | SPI_CFG_TRF_CONST);
	SPI_WaitEOT(spibus);
	LCD_CS1;
}

static void LCD_CasRasSet(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	LCD_Command(ILI9341_CASET);
	scratch[0] = x1 >> 8;
	scratch[1] = x1;
	scratch[2] = x2 >> 8;
	scratch[3] = x2;
	SPI_Transfer(spibus, scratch, 4);

	LCD_Command(ILI9341_PASET);
	scratch[0] = y1 >> 8;
	scratch[1] = y1;
	scratch[2] = y2 >> 8;
	scratch[3] = y2;
	SPI_Transfer(spibus, scratch, 4);

	LCD_Command(ILI9341_RAMWR);
}

/**
 * @brief
 *
 * @param x
 * @param y
 * @param w
 * @param h
 */
void LCD_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    SPI_WaitEOT(spibus);

    LCD_CS0;
	LCD_CasRasSet(x, y, x + (w - 1), y + (h - 1));
}

/**
 * @brief Fill's area with same color
 *
 * \param x :
 * \param y :
 * \param w :
 * \param h :
 * \param color :
 */
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
    uint32_t count = w * h;

    if(!count){
        return;
    }

	LCD_Window(x, y, w, h);

	if(spibus->cfg & SPI_CFG_DMA){
        spibus->cfg |= SPI_CFG_TRF_16BIT | SPI_CFG_TRF_CONST;
        *((uint16_t*)scratch) = color;
		SPI_TransferDMA(spibus, (uint8_t*)scratch, count);
		//LCD_CS1; // SET by DMA handler
	}else{
		while(count--){
            scratch[1] = color >> 8;
            scratch[0] = color;
	        SPI_Transfer(spibus, scratch, 2);
        }
		LCD_CS1;
	}
}

/**
 * @brief Write data block to defined area
 *
 * \param x :
 * \param y :
 * \param w :
 * \param h :
 * \param data : Pointer to data
 */
void LCD_WriteArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data){
    uint32_t count = w * h;

    if(!count){
        return;
    }

	LCD_Window(x, y, w, h);
    LCD_WriteData(data, count);
}

/**
 * @brief
 *
 * @param x
 * @param y
 * @param c
 */
void LCD_Pixel(uint16_t x, uint16_t y, uint16_t color){

    SPI_WaitEOT(spibus);

    LCD_CS0;
    LCD_CasRasSet(x, y, x, y);
    LCD_Data(color);
    LCD_CS1;
}

/**
 * @brief
 *
 * @param spi
 */
uint8_t LCD_Init(void *driver)
{
    if(driver == NULL){
        return 0;
    }

    drvlcd = (drvlcdspi_t*)driver;
    spibus = &drvlcd->spidev;

    if(spibus->cfg & SPI_CFG_DMA){
        SPI_SetEOT(spibus, LCD_DataEnd);
    }

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
    LCD_Command(ILI9341_SWRST);
    LCD_CS1;

    DelayMs(5);

    LCD_CS0;
    LCD_InitSequence(ili9341_init_seq);
    LCD_CS1;

    _width  = drvlcd->w;
    _height = drvlcd->h;

    return 1;
}

/**
 * @brief
 *
 * @param m
 */
void LCD_SetOrientation(drvlcdorientation_t m) {

    switch (m) {
        case LCD_PORTRAIT:
            m = (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
            _width  = drvlcd->w;
            _height = drvlcd->h;
            break;
        case LCD_LANDSCAPE:
            m = (ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
            _width  = drvlcd->h;
            _height = drvlcd->w;
            break;
        case LCD_REVERSE_PORTRAIT:
            m = (ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
            _width  = drvlcd->w;
            _height = drvlcd->h;
            break;
        case LCD_REVERSE_LANDSCAPE:
            m = (ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
            _width  = drvlcd->h;
            _height = drvlcd->w;
            break;

        default:
         return;
    }

    SPI_WaitEOT(spibus);

    LCD_CS0;
    LCD_Command(ILI9341_MADCTL);
    SPI_Transfer(spibus, &m, 1);
    LCD_CS1;
}

/**
 * @brief
 *
 * @param sc
 */
void LCD_Scroll(uint16_t sc){

    SPI_WaitEOT(spibus);

    LCD_CS0;
    LCD_Command(ILI9341_VSCRSADD);
    LCD_Data(sc);
    LCD_CS1;
}

/**
 * @brief
 *
 * @return uint16_t
 */
uint16_t LCD_GetWidth(void){
   return _width;
}

/**
 * @brief
 *
 * @return uint16_t
 */
uint16_t LCD_GetHeight(void){
   return _height;
}

/**
 * @brief
 *
 * @return uint32_t
 */
uint32_t LCD_GetSize(void){
   return _height*_width;
}

/**
 * @brief
 *
 * @param state
 */
void LCD_Bkl(uint8_t state){
    if(state != 0) {
        LCD_BKL1;
    }
    else {
        LCD_BKL0;
    }
}
