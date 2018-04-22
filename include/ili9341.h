/**
* @file     ili9341.h
* @brief    Contains the ILI9341 lcd controller API headers and symbols.
*           
* @version  1.0
* @date     5 Nov. 2016
* @author   Hugo Reis
**********************************************************************/

#ifndef _ili9341_h_
#define _ili9341_h_

#include <spi.h>
#include <gpio.h>

#define LCD_ILI9341

#define TFT_W 240
#define TFT_H 320

#define LCD_W LCD_GetWidth()
#define LCD_H LCD_GetHeight()
#define LCD_SIZE LCD_GetSize()

#if defined(__LPCXpresso__)
/**
* @brief Lcd Pin configuration:
*       P0.16  CS
*       P0.23  RST
*       P0.24  C'/D
*       P0.18  SI
*       P0.15  SCK
*       P0.25  Backlight enable
*       P0.17  SO
**/

#define LCD_CS   16
#define LCD_CD   24
#define LCD_RST  23
#define LCD_BKL  25

#define LCD_CS0  GPIO_Clr(LCD_CS)
#define LCD_CS1  GPIO_Set(LCD_CS)
#define LCD_CD0  GPIO_Clr(LCD_CD)
#define LCD_CD1  GPIO_Set(LCD_CD)
#define LCD_RST0 GPIO_Clr(LCD_RST)
#define LCD_RST1 GPIO_Set(LCD_RST)
#define LCD_BKL0 GPIO_Clr(LCD_BKL)
#define LCD_BKL1 GPIO_Set(LCD_BKL)

#define LCD_PIN_INIT                    \
            GPIO_SetOutput(LCD_CS);     \
            GPIO_SetOutput(LCD_CD);     \
            GPIO_SetOutput(LCD_RST);    \
            GPIO_SetOutput(LCD_BKL);    \

#ifdef __USE_CMSIS
#include <time_m3.h>
#else
#include <clock.h>
#endif

#elif defined(__LPC_H2106__)
#include <lpc2106.h>
/**
* @brief Lcd Pin configuration:
*       P0.3  CS
*       P0.4  SCK
*       P0.5  SO
*       P0.6  SI      
*       P0.8  Backlight enable
*       P0.9  C'/D
*       p0.10 RST
**/
 
#define LCD_CS   3
#define LCD_CD   9
#define LCD_RST 10
#define LCD_BKL  8

#define LCD_CS0  GPIO_Clr(LCD_CS)
#define LCD_CS1  GPIO_Set(LCD_CS)
#define LCD_CD0  GPIO_Clr(LCD_CD)
#define LCD_CD1  GPIO_Set(LCD_CD)
#define LCD_RST0 GPIO_Clr(LCD_RST)
#define LCD_RST1 GPIO_Set(LCD_RST)
#define LCD_BKL0 GPIO_Clr(LCD_BKL)
#define LCD_BKL1 GPIO_Set(LCD_BKL)

#define LCD_PIN_INIT                    \
            GPIO_SetOutput(LCD_CS);     \
            GPIO_SetOutput(LCD_CD);     \
            GPIO_SetOutput(LCD_RST);    \
            GPIO_SetOutput(LCD_BKL);    \

#elif defined(__TDSO__) && defined(USE_HAL_DRIVER)
/**
* @brief Lcd Pin configuration:
*       PA4  CS
*       PB1  RST
*       PB10 C'/D
*       PA7  SI
*       PA5  SCK
*       PB11 Backlight enable
*       PA6  SO
**/

#define LCD_CS   GPIO_PIN_4
#define LCD_CD   GPIO_PIN_10
#define LCD_RST  GPIO_PIN_1
#define LCD_BKL  GPIO_PIN_11

#define LCD_CS0  GPIO_Clr(GPIOA, LCD_CS)
#define LCD_CS1  GPIO_Set(GPIOA, LCD_CS)
#define LCD_CD0  GPIO_Clr(GPIOB, LCD_CD)
#define LCD_CD1  GPIO_Set(GPIOB, LCD_CD)
#define LCD_RST0 GPIO_Clr(GPIOB, LCD_RST)
#define LCD_RST1 GPIO_Set(GPIOB, LCD_RST)
#define LCD_BKL0 GPIO_Clr(GPIOB, LCD_BKL)
#define LCD_BKL1 GPIO_Set(GPIOB, LCD_BKL)

#define LCD_PIN_INIT // handle by HAL_Init

#elif defined(__ESP03__)
/**
* @brief Lcd Pin configuration:
*       GPIO12  CS
*            RST
*           C'/D
*       GPIO13  SI
*       GPIO14  SCK
*       PB11    LED
*       ---     SO
**/

#define LCD_CS   15
#define LCD_CD   12
#define LCD_RST  16
#define LCD_BKL  1

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
#define LCD_BKL0  GPIO_OUTPUT_SET(LCD_BKL,0)
#define LCD_BKL1  GPIO_OUTPUT_SET(LCD_BKL,1)

#define LCD_PIN_INIT                                     \
{                                                        \
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12); \
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2); \
/* https://github.com/willemwouters/ESP8266/blob/master/sdk/esp_iot_sdk_v0.9.3/IoT_Demo/driver/gpio16.c */ \
    WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32)0x1); \
    WRITE_PERI_REG(RTC_GPIO_CONF, (READ_PERI_REG(RTC_GPIO_CONF) & (uint32)0xfffffffe) | (uint32)0x0); \
    WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32)0xfffffffe) | (uint32)0x1); \
}


#else
#error "Please Define board"
#endif

#define SWRST    0x01
#define RDDID    0x04
#define SLPIN    0x10
#define SLPOUT   0x11
#define DISPOFF  0x28
#define DISPON   0x29
#define CASET    0x2A
#define PASET    0x2B
#define RAMWR    0x2C
#define MADCTL   0x36   // Memory Access Control
#define VSCRSADD 0x37
#define COLMOD   0x3A   // Pixel Format set
#define FRCONN   0xB1   // Frame rate control Normal mode
#define FRCONI   0xB2   // Frame rate control Idle mode
#define FRCONP   0xB3   // Frame rate control Partial mode
#define DFCTL    0xB6   // Display Function Control
#define PCON1    0xC0   // power control 1
#define PCON2    0xC1   // power control 2
#define VCOM1    0xC5   // VCOM control 1
#define VCOM2    0xC7   // VCOM control 2
#define PCONA    0xCB   // Power Control A
#define PCONB    0xCF   // Power Control B
#define DTCONA   0xE8   // Driver Timming Control A
#define DTCONB   0xEA   // Driver Timming Control B
#define PSCON    0xED   // Power on Sequence control
#define PRCON    0xF7   // Pump ratio control

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

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

