/**
* @file     st7735.h
* @brief    Contains the st7735 lcd controller API headers and symbols.
*           
* @version  1.0
* @date     8 Jun. 2018
* @author   Hugo Reis
**********************************************************************/

#ifndef _st7735_h_
#define _st7735_h_

#include <stdint.h>
#include <spi.h>
#include <gpio.h>

#define LCD_ST7735

#define TFT_W 128
#define TFT_H 160

#define LCD_W LCD_GetWidth()
#define LCD_H LCD_GetHeight()
#define LCD_SIZE LCD_GetSize()

#if defined(__ESP03__)
#include "esp8266/pin_mux_register.h"
/**
* @brief Lcd Pin configuration:
*       GPIO15  CS
*       TBD     RST
*       GPIO12  C'/D
*       GPIO13  SI
*       GPIO14  SCK
*       GPIO16  LED
*       ---     SO
**/

#define LCD_CS   15
#define LCD_CD   12
#define LCD_RST
#define LCD_BKL  16

static inline void GPIO16_OUTPUT_SET(uint8_t value)
{
    WRITE_PERI_REG(RTC_GPIO_OUT, (READ_PERI_REG(RTC_GPIO_OUT) & (uint32_t)0xfffffffe) | (uint32_t)(value & 1));
}

#define LCD_CS0   //GPIO_OUTPUT_SET(LCD_CS,0)  // hw activated
#define LCD_CS1   //GPIO_OUTPUT_SET(LCD_CS,1)
#define LCD_CD0   GPIO_OUTPUT_SET(LCD_CD,0)
#define LCD_CD1   GPIO_OUTPUT_SET(LCD_CD,1)
#define LCD_RST0  //GPIO_OUTPUT_SET(LCD_RST,0)
#define LCD_RST1  //GPIO_OUTPUT_SET(LCD_RST,1)
#define LCD_BKL0  GPIO16_OUTPUT_SET(0)
#define LCD_BKL1  GPIO16_OUTPUT_SET(1)

#define LCD_PIN_INIT                                     \
{                                                        \
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12); \
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2); \
/* https://github.com/willemwouters/ESP8266/blob/master/sdk/esp_iot_sdk_v0.9.3/IoT_Demo/driver/gpio16.c */ \
    WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32_t)0x1); \
    WRITE_PERI_REG(RTC_GPIO_CONF, (READ_PERI_REG(RTC_GPIO_CONF) & (uint32_t)0xfffffffe) | (uint32_t)0x0); \
    WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32_t)0xfffffffe) | (uint32_t)0x1); \
}


#else
#error "Please Define board"
#endif


#define INITR_GREENTAB 0x0
#define INITR_REDTAB   0x1

#define ST7735_TFTWIDTH  128
#define ST7735_TFTHEIGHT 160

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1


#define pgm_read_byte(x) *((uint8_t*)(x))
#define writecommand(x)  LCD_Command(x)
#define writedata(x)     SPI_Send(x)
#define myDelay(x)        DelayMs(x)
#define PROGMEM
#define CASET            ST7735_CASET
#define PASET            ST7735_RASET
#define RAMWR            ST7735_RAMWR
#define VSCRSADD         0x37
#define MADCTL           ST7735_MADCTL

#define MADCTL_MY  (1<<7)
#define MADCTL_MX  (1<<6)
#define MADCTL_MV  (1<<5)
#define MADCTL_ML  (1<<4)
#define MADCTL_RGB (1<<3)
#define MADCTL_MH  (1<<2)

#define LCD_PORTRAIT  0
#define LCD_LANDSCAPE 1
#define LCD_REVERSE_PORTRAIT  2
#define LCD_REVERSE_LANDSCAPE 3

/**
* @brief preencimento de n pixels da mesma cor
*   Nota: LCD_Window deve de ser chamado previamente
*         para definir uma zona para escrita
**/
void LCD_Fill(uint32_t n, uint16_t color);

/**
* @brief Efectua a copia de uma zona de memoria para o
*  	display
*   Nota: LCD_Window deve de ser chamado previamente
*         para definir uma zona para escrita
**/
void LCD_Fill_Data(uint32_t n, uint16_t *data);

/**
 * @brief efectua deslocamento vertical
 */
void LCD_Scroll(uint16_t sc);

/**
 * @brief retorna a largura currente em pixels
 */
uint16_t LCD_GetWidth(void);

/**
 * @brief retorna a altura currente em pixels
 */
uint16_t LCD_GetHeight(void);

/**
 * @brief retorna o numero total de pixels do display
 */
uint32_t LCD_GetSize(void);

/*
 * @brief muda a orientação do lcd
 */
void LCD_Rotation(uint8_t m);

/**
 * @brief acesso directo ao lcd para escrita de dados
 *        Pode ser usado para escrita de bloco
 */
void LCD_Data(uint16_t data);

#endif

