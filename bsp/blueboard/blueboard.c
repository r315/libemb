// File Name: blueboard.c
// Date:  	  6-10-2010
#include "blueboard.h"
#include "button.h"
#include "spi.h"
#include "drvlcd.h"
#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


spibus_t spibus_1, spibus_sd;


#ifdef ENABLE_SDCARD
static void sdcardInit()
{
    BOARD_SDCARD_DESELECT;
    GPIO_Config(P0_16, GPO_PP);

	spibus_sd.bus = SPI_BUS0;
    spibus_sd.freq = 8000UL;
    spibus_sd.flags  = SPI_MODE0;
    SPI_Init(&spibus_sd);
}
#endif

#ifdef ENABLE_BUTTONS
static void buttonsInit()
{
    BUTTON_Init(BUTTON_DEFAULT_HOLD_TIME);
}
#endif

#ifdef ENABLE_TFT_DISPLAY
static drvlcdparallel_s lcd0;

static void displayWrite(uint32_t data)
{
	LPC_GPIO0->FIOPIN0 = data >> 8;
	LCDWR0;
    LCDWR1;
    LPC_GPIO0->FIOPIN0 = data;
	LCDWR0;
    LCDWR1;
}

void BB_LCD_Init(void)
{
    //LPC_GPIO1->FIODIR |= LCD_CS|LCD_RS|LCD_WR|LCD_RD|LCD_LED|LCD_RST;

    lcd0.bkl = P1_1;
    lcd0.cs  = P1_10;
    lcd0.cd  = P1_9;
    lcd0.wr  = P1_8;
    lcd0.rd  = P1_4;
    lcd0.rst = P1_0;
    lcd0.write = displayWrite;

    lcd0.w = 240;
    lcd0.h = 320;

    LPC_GPIO0->FIODIR0 |= 0xFF;
    GPIO_Config(lcd0.cs, GPO_PP);
    GPIO_Config(lcd0.cd, GPO_PP);
    GPIO_Config(lcd0.rst, GPO_PP);
    GPIO_Config(lcd0.wr, GPO_PP);
    GPIO_Config(lcd0.rd, GPO_PP);
    GPIO_Config(lcd0.bkl, GPO_PP);

    LCD_Init(&lcd0);

    LCD_Bkl(1);

    LCD_FillRect(0, 0, lcd0.w, lcd0.h, 0);
}
#endif
//---------------------------------------------------
//
//---------------------------------------------------
void BB_Init(void){
    CLOCK_Init(100);

	LEDS_INIT;
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;

    #ifdef ENABLE_TFT_DISPLAY
    BB_LCD_Init();
    #endif
}

void SW_Reset(void){
    NVIC_SystemReset();
}

void DelayMs(uint32_t ms){
    CLOCK_DelayMs(ms);
}

uint32_t GetTick(void){
    return CLOCK_GetTicks();
}

uint32_t ElapsedTicks(uint32_t start_ticks){
    return CLOCK_ElapsedTicks(start_ticks);
}
//---------------------------------------------------
//
//---------------------------------------------------
void BB_ConfigClockOut(uint8_t en){
	if(en){
		LPC_SC->CLKOUTCFG = (1<<4)|/* CCLK/2 */ (1<<8);/* CLKOU_EN*/
		LPC_PINCON->PINSEL3 |= (1<<22);// P1.27 CLKOUT
	}else{
		LPC_SC->CLKOUTCFG = (1<<4)|/* CCLK/2 */ (1<<8);/* CLKOU_EN*/
		LPC_PINCON->PINSEL3 &= ~(1<<22);
	}
}


//--------------------------------------------------
//
//--------------------------------------------------
void BB_SPI_Write(uint8_t *data, uint32_t count){
	SPI_Transfer(&spibus_1, data, count);
}

void BB_SPI_WriteDMA(uint8_t *data, uint32_t count){
	SPI_TransferDMA(&spibus_1, data, count);
}

uint16_t BB_SPI_Send(uint8_t data){
	return SPI_Xchg(&spibus_1, &data);
}

void BB_SPI_WaitEOT(){
	SPI_WaitEOT(&spibus_1);
}

void BB_SPI_SetFrequency(uint32_t freq){
    spibus_1.freq = freq;
    SPI_Init(&spibus_1);
}

#ifdef __cplusplus
}
#endif

