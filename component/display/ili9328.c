#include "ili9328.h"
#include "drvlcd.h"
#include "gpio.h"

#define LCD_CS1     GPIO_Write(drvlcd->cs, GPIO_PIN_HIGH)
#define LCD_CS0     GPIO_Write(drvlcd->cs, GPIO_PIN_LOW)
#define LCD_RS1     GPIO_Write(drvlcd->cd, GPIO_PIN_HIGH)
#define LCD_RS0     GPIO_Write(drvlcd->cd, GPIO_PIN_LOW)
#define LCD_BKL1    GPIO_Write(drvlcd->bkl, GPIO_PIN_HIGH)
#define LCD_BKL0    GPIO_Write(drvlcd->bkl, GPIO_PIN_LOW)
#define LCD_RST1    GPIO_Write(drvlcd->rst, GPIO_PIN_HIGH)
#define LCD_RST0    GPIO_Write(drvlcd->rst, GPIO_PIN_LOW)
#define LCD_WR1     GPIO_Write(drvlcd->wr, GPIO_PIN_HIGH)
#define LCD_WR0     GPIO_Write(drvlcd->wr, GPIO_PIN_LOW)
#define LCD_RD1     GPIO_Write(drvlcd->rd, GPIO_PIN_HIGH)
#define LCD_RD0     GPIO_Write(drvlcd->rd, GPIO_PIN_LOW)


static drvlcdparallel_s *drvlcd;
static uint16_t width, height;

const drvlcd_t ili9328_drv =
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
    LCD_GetSize
};

extern void DelayMs(uint32_t ms);
static void (*windowSet)(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

void LCD_Bkl(uint8_t state){
	if(state){
		LCD_BKL1;
	}else{
		LCD_BKL0;
	}
}

//--------------------------------------------------------
//
//--------------------------------------------------------
#if 0
uint16_t LCD_ReadReg(uint8_t reg){
    uint16_t dta = 0;

    LCD_RS0;                         /* Set register */
	LCD_IOD = 0;
	LCD_WR0;LCD_WR1;
	LCD_IOD = reg;
	LCD_WR0;LCD_WR1;

    LCD_IODDIR = 0x00;          /* change to input */
    DelayMs(3);
	LCD_RS1;
	LCD_RD0; DelayMs(3); dta = (LCD_IOD << 8); LCD_RD1;
	LCD_RD0; DelayMs(3); dta |= (LCD_IOD);     LCD_RD1;
	LCD_IODDIR = 0xFF;          /* change to out      put */
    DelayMs(3);
    return dta;
}

uint16_t LCD_ReadId(void){
    /*uint16_t id;
    if (LCD_ReadReg(0x04) == 0x8000) {
        if (LCD_ReadReg(0xD0) == 0x990000) {
          return 0x8357;
        }
    }

    id = LCD_ReadReg(0xD3);
    if (id == 0x9341) {
        return id;
    }*/
    return LCD_ReadReg(LCD_REG_DRV_CODE);
}
#endif
//--------------------------------------------------------
//write data to LCD
//--------------------------------------------------------
void LCD_Data(uint16_t data)
{
    drvlcd->write(data);
}
//--------------------------------------------------------
//write instruction to LCD
//--------------------------------------------------------
static void ili9328WriteReg(uint8_t ins, uint16_t data)
{
	LCD_RS0;
	drvlcd->write(ins);
	LCD_RS1;
    drvlcd->write(data);
}
//--------------------------------------------------------
//
//--------------------------------------------------------
void LCD_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    windowSet(x, y, w, h);
    LCD_RS0;
    drvlcd->write(LCD_RW_GRAM);
    LCD_RS1;
}

void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
    uint32_t count = w * h;

    LCD_Window(x, y, w, h);

    while(count--){
		drvlcd->write(color);
	}
}

void LCD_WriteArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data){
	uint32_t count = w * h;

	LCD_Window(x, y, w, h);

	while(count--){
		drvlcd->write(*data++);
	}
}
//-------------------------------------------------------------------
//	 Creates a write window inside GRAM
//-------------------------------------------------------------------

static void windowPortrait(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
	ili9328WriteReg(LCD_HOR_START_AD, x);
	ili9328WriteReg(LCD_HOR_END_AD, x + w - 1);
	ili9328WriteReg(LCD_VER_START_AD, y);
	ili9328WriteReg(LCD_VER_END_AD, y + h - 1);

	ili9328WriteReg(LCD_GRAM_HOR_AD, x);
	ili9328WriteReg(LCD_GRAM_VER_AD, y);
}

static void windowLandscape(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
	ili9328WriteReg(LCD_HOR_START_AD, y);
	ili9328WriteReg(LCD_HOR_END_AD, y + h - 1);
	ili9328WriteReg(LCD_VER_START_AD, x);
	ili9328WriteReg(LCD_VER_END_AD, x + w - 1);

	ili9328WriteReg(LCD_GRAM_HOR_AD, y);
	ili9328WriteReg(LCD_GRAM_VER_AD, x);
}
//--------------------------------------------------------
//
//--------------------------------------------------------
void LCD_Pixel(uint16_t x, uint16_t y, uint16_t c){
	LCD_Window(x,y,1,1);
	LCD_Data(c);
}
//-----------------------------------------------------------------*/
//	 set VLE bit on GATE_SCAN_CTRL2(61h) first
//-------------------------------------------------------------------
void LCD_Scroll(uint16_t y)
{
	//while ((int16_t)y < 0)
 	//	y += 320;
	//while (y >= 320)
 	//	y -= 320;
	ili9328WriteReg(LCD_GATE_SCAN_CTRL3, y);
}
//--------------------------------------------------------
// initialize lcd
//--------------------------------------------------------
uint8_t LCD_Init(void *param)
{
    drvlcd = param;

	LCD_CS1;
    LCD_WR1;
    LCD_RD1;
	LCD_RST0;
	DelayMs(100);
	LCD_RST1;
	LCD_CS0;

	ili9328WriteReg(0xE5, 0x8000);          //set the internal vcore voltage
	ili9328WriteReg(LCD_START_OSC, 1);      //start oscillator
	DelayMs(50);

	ili9328WriteReg(LCD_DRIV_OUT_CTRL, SHIFT_DIR);
	ili9328WriteReg(LCD_DRIV_WAV_CTRL, 0x0700);     //set 1 line inversion

	ili9328WriteReg(LCD_ENTRY_MOD, VAL_ENTRY_MOD);  //set GRAM write direction, BGR=0

	ili9328WriteReg(LCD_RESIZE_CTRL, 0);    //no resizing

	ili9328WriteReg(LCD_DISP_CTRL2, 0x0202); //front & back porch periods = 2
	ili9328WriteReg(LCD_DISP_CTRL3, 0);
	ili9328WriteReg(LCD_DISP_CTRL4, 0);
	ili9328WriteReg(LCD_RGB_DISP_IF_CTRL1, 0); //select system interface
	ili9328WriteReg(LCD_FRM_MARKER_POS, 0);
	ili9328WriteReg(LCD_RGB_DISP_IF_CTRL2, 0);

	ili9328WriteReg(LCD_POW_CTRL1, 0);
	ili9328WriteReg(LCD_POW_CTRL2, 0);
	ili9328WriteReg(LCD_POW_CTRL3, 0);
	ili9328WriteReg(LCD_POW_CTRL4, 0);
	DelayMs(200);

	ili9328WriteReg(LCD_POW_CTRL1, 0x17B0);
	ili9328WriteReg(LCD_POW_CTRL2, 0x0137);
	DelayMs(50);

	ili9328WriteReg(LCD_POW_CTRL3, 0x013C);
	DelayMs(50);

	ili9328WriteReg(LCD_POW_CTRL4, 0x1400);
	ili9328WriteReg(LCD_POW_CTRL7, 0x0007);
	DelayMs(50);

	ili9328WriteReg(LCD_GAMMA_CTRL1, 0x0007);
	ili9328WriteReg(LCD_GAMMA_CTRL2, 0x0504);
	ili9328WriteReg(LCD_GAMMA_CTRL3, 0x0703);
	ili9328WriteReg(LCD_GAMMA_CTRL4, 0x0002);
	ili9328WriteReg(LCD_GAMMA_CTRL5, 0x0707);
	ili9328WriteReg(LCD_GAMMA_CTRL6, 0x0406);
	ili9328WriteReg(LCD_GAMMA_CTRL7, 0x0006);
	ili9328WriteReg(LCD_GAMMA_CTRL8, 0x0404);
	ili9328WriteReg(LCD_GAMMA_CTRL9, 0x0700);
	ili9328WriteReg(LCD_GAMMA_CTRL10, 0x0A08);

	ili9328WriteReg(START_ADX, 0);
	ili9328WriteReg(END_ADX, drvlcd->w - 1);
	ili9328WriteReg(START_ADY, 0);
	ili9328WriteReg(END_ADY, drvlcd->h - 1);

	ili9328WriteReg(LCD_GATE_SCAN_CTRL1, VAL_GATE_SCAN);
	ili9328WriteReg(LCD_GATE_SCAN_CTRL2, 0x0003);
	ili9328WriteReg(LCD_GATE_SCAN_CTRL3, 0);

	ili9328WriteReg(LCD_PART_IMG1_DISP_POS, 0);
	ili9328WriteReg(LCD_PART_IMG1_START_AD, 0);
	ili9328WriteReg(LCD_PART_IMG1_END_AD, 0);
	ili9328WriteReg(LCD_PART_IMG2_DISP_POS, 0);
	ili9328WriteReg(LCD_PART_IMG2_START_AD, 0);
	ili9328WriteReg(LCD_PART_IMG2_END_AD, 0);

	ili9328WriteReg(LCD_PANEL_IF_CTRL1, 0x0010);
	ili9328WriteReg(LCD_PANEL_IF_CTRL2, 0);
	ili9328WriteReg(LCD_PANEL_IF_CTRL3, 3);
	ili9328WriteReg(LCD_PANEL_IF_CTRL4, 0x0110);
	ili9328WriteReg(LCD_PANEL_IF_CTRL5, 0);
	ili9328WriteReg(LCD_PANEL_IF_CTRL6, 0);

	ili9328WriteReg(LCD_DISP_CTRL1, 0x0173);
	DelayMs(500);

	LCD_SetOrientation(LCD_PORTRAIT);

    return 1;
}

uint16_t LCD_GetWidth(void){
    return width;
}

uint16_t LCD_GetHeight(void){
    return height;
}

uint32_t LCD_GetSize(void){
    return width * height;
}

void LCD_SetOrientation(drvlcdorientation_t m) {

    switch (m) {
        case LCD_PORTRAIT:
            ili9328WriteReg(LCD_DRIV_OUT_CTRL,
                        LCD_DRIV_OUT_CTRL_SS);          // Source drive S720-S1
            ili9328WriteReg(LCD_GATE_SCAN_CTRL1,
                        LCD_GATE_SCAN_CTRL1_GS |        // Gate Scan G320-G1
                        LCD_GATE_SCAN_CTRL1_NL320);     // Scan 320 Lines
            ili9328WriteReg(LCD_ENTRY_MOD,
                        LCD_ENTRY_MOD_VI |              // Increment vertical address
                        LCD_ENTRY_MOD_HI |              // Increment horizontal address
                        BGR_BIT);
            width = drvlcd->w;
            height = drvlcd->h;
            windowSet = windowPortrait;
            break;

        case LCD_LANDSCAPE:
            ili9328WriteReg(LCD_DRIV_OUT_CTRL, 0);      //Source drive S1-S720
            ili9328WriteReg(LCD_GATE_SCAN_CTRL1,
                        LCD_GATE_SCAN_CTRL1_GS |        // Gate Scan G320-G1
                        LCD_GATE_SCAN_CTRL1_NL320);     // Scan 320 Lines
            ili9328WriteReg(LCD_ENTRY_MOD,
                        LCD_ENTRY_MOD_AM |              // Update vertically
                        LCD_ENTRY_MOD_VI |              // Increment vertical address
                        LCD_ENTRY_MOD_HI |              // Increment horizontal address
                        BGR_BIT);
            height = drvlcd->w;
            width = drvlcd->h;
            windowSet = windowLandscape;
            break;

        case LCD_REVERSE_PORTRAIT:
            ili9328WriteReg(LCD_DRIV_OUT_CTRL,
                        LCD_DRIV_OUT_CTRL_SS);          //Source drive S720-S1
            ili9328WriteReg(LCD_GATE_SCAN_CTRL1,
                        //LCD_GATE_SCAN_CTRL1_GS |        // Gate Scan G320-G1
                        LCD_GATE_SCAN_CTRL1_NL320);     // Scan 320 Lines
            ili9328WriteReg(LCD_ENTRY_MOD,
                        //LCD_ENTRY_MOD_AM |              // Update vertically
                        LCD_ENTRY_MOD_VI |              // Increment vertical address
                        //LCD_ENTRY_MOD_HI |              // Increment horizontal address
                        BGR_BIT);
            height = drvlcd->h;
            width = drvlcd->w;
            windowSet = windowPortrait;
            break;

        case LCD_REVERSE_LANDSCAPE:
            ili9328WriteReg(LCD_DRIV_OUT_CTRL,
                        LCD_DRIV_OUT_CTRL_SS);          //Source drive S720-S1
            ili9328WriteReg(LCD_GATE_SCAN_CTRL1,
                        //LCD_GATE_SCAN_CTRL1_GS |        // Gate Scan G320-G1
                        LCD_GATE_SCAN_CTRL1_NL320);     // Scan 320 Lines
            ili9328WriteReg(LCD_ENTRY_MOD,
                        LCD_ENTRY_MOD_AM |              // Update vertically
                        LCD_ENTRY_MOD_VI |              // Increment vertical address
                        LCD_ENTRY_MOD_HI |              // Increment horizontal address
                        BGR_BIT);
            height = drvlcd->w;
            width = drvlcd->h;
            windowSet = windowLandscape;
            break;

        default:
        	return;
    }
}



