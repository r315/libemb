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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "drvlcd.h"

#define TFT_ST7735S

// System Function Commands
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
#define ST7735_VSCSAD  0x37

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

// Panel Function Commands
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
#define ST7735_VMOFCTR 0xC7

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define ST7735_PWCTR6  0xFC

// -----------------------------

#define ST7735_MADCTL_MY  (1<<7)    // Row order
#define ST7735_MADCTL_MX  (1<<6)    // Column order
#define ST7735_MADCTL_MV  (1<<5)    // Exchange row/col order
#define ST7735_MADCTL_ML  (1<<4)    // v-refresh order
#define ST7735_MADCTL_RGB (1<<3)    // RGB-BGR order
#define ST7735_MADCTL_MH  (1<<2)    // h-refresh order

extern const drvlcd_t st7735_drv;


#ifdef __cplusplus
}
#endif

#endif

