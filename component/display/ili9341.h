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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "drvlcd.h"

#define TFT_ILI9341

#define ILI9341_SWRST    0x01
#define ILI9341_RDDID    0x04
#define ILI9341_SLPIN    0x10
#define ILI9341_SLPOUT   0x11
#define ILI9341_DISPOFF  0x28
#define ILI9341_DISPON   0x29
#define ILI9341_CASET    0x2A
#define ILI9341_PASET    0x2B
#define ILI9341_RAMWR    0x2C
#define ILI9341_MADCTL   0x36   // Memory Access Control
#define ILI9341_VSCRSADD 0x37
#define ILI9341_COLMOD   0x3A   // Pixel Format set
#define ILI9341_FRCONN   0xB1   // Frame rate control Normal mode
#define ILI9341_FRCONI   0xB2   // Frame rate control Idle mode
#define ILI9341_FRCONP   0xB3   // Frame rate control Partial mode
#define ILI9341_DFCTL    0xB6   // Display Function Control
#define ILI9341_PCON1    0xC0   // power control 1
#define ILI9341_PCON2    0xC1   // power control 2
#define ILI9341_VCOM1    0xC5   // VCOM control 1
#define ILI9341_VCOM2    0xC7   // VCOM control 2
#define ILI9341_PCONA    0xCB   // Power Control A
#define ILI9341_PCONB    0xCF   // Power Control B
#define ILI9341_DTCONA   0xE8   // Driver Timming Control A
#define ILI9341_DTCONB   0xEA   // Driver Timming Control B
#define ILI9341_PSCON    0xED   // Power on Sequence control
#define ILI9341_PRCON    0xF7   // Pump ratio control

#define ILI9341_MADCTL_MY  0x80 // Row Address Order
#define ILI9341_MADCTL_MX  0x40 // Column Address Order
#define ILI9341_MADCTL_MV  0x20 // Row / Column Exchange
#define ILI9341_MADCTL_ML  0x10 // Vertical Refresh Order
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

extern const drvlcd_t ili9341_drv;

#ifdef __cplusplus
};
#endif

#endif

