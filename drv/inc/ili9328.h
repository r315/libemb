 /** 
 @file ili9328.h
 @brief  ILI9328 Lcd controller driver
 @author Hugo Reis
 @date 06-02-2017
 **/
#ifndef _ili9328_h_
#define _ili9328_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "drvlcd.h"

//-------------------------------------------------------------------
#define LCD_REG_DRV_CODE		0x00
#define LCD_START_OSC			0x00
#define LCD_DRIV_OUT_CTRL		0x01
#define LCD_DRIV_WAV_CTRL		0x02
#define LCD_ENTRY_MOD			0x03
#define LCD_RESIZE_CTRL			0x04
#define LCD_DISP_CTRL1			0x07
#define LCD_DISP_CTRL2			0x08
#define LCD_DISP_CTRL3			0x09
#define LCD_DISP_CTRL4			0x0A
#define LCD_RGB_DISP_IF_CTRL1	0x0C
#define LCD_FRM_MARKER_POS		0x0D
#define LCD_RGB_DISP_IF_CTRL2	0x0F
#define LCD_POW_CTRL1			0x10
#define LCD_POW_CTRL2			0x11
#define LCD_POW_CTRL3			0x12
#define LCD_POW_CTRL4			0x13
#define LCD_GRAM_HOR_AD			0x20
#define LCD_GRAM_VER_AD			0x21
#define LCD_RW_GRAM				0x22
#define LCD_POW_CTRL7			0x29
#define LCD_FRM_RATE_COL_CTRL	0x2B
#define LCD_GAMMA_CTRL1			0x30
#define LCD_GAMMA_CTRL2			0x31
#define LCD_GAMMA_CTRL3			0x32
#define LCD_GAMMA_CTRL4			0x35 
#define LCD_GAMMA_CTRL5			0x36
#define LCD_GAMMA_CTRL6			0x37
#define LCD_GAMMA_CTRL7			0x38
#define LCD_GAMMA_CTRL8			0x39
#define LCD_GAMMA_CTRL9			0x3C
#define LCD_GAMMA_CTRL10		0x3D
#define LCD_HOR_START_AD		0x50
#define LCD_HOR_END_AD			0x51
#define LCD_VER_START_AD		0x52
#define LCD_VER_END_AD			0x53
#define LCD_GATE_SCAN_CTRL1		0x60
#define LCD_GATE_SCAN_CTRL2		0x61
#define LCD_GATE_SCAN_CTRL3		0x6A
#define LCD_PART_IMG1_DISP_POS	0x80
#define LCD_PART_IMG1_START_AD	0x81
#define LCD_PART_IMG1_END_AD	0x82
#define LCD_PART_IMG2_DISP_POS	0x83
#define LCD_PART_IMG2_START_AD	0x84
#define LCD_PART_IMG2_END_AD	0x85
#define LCD_PANEL_IF_CTRL1		0x90
#define LCD_PANEL_IF_CTRL2		0x92
#define LCD_PANEL_IF_CTRL3		0x93
#define LCD_PANEL_IF_CTRL4		0x95
#define LCD_PANEL_IF_CTRL5		0x97
#define LCD_PANEL_IF_CTRL6		0x98
#define LCD_DELAY               0xFF


#define LCD_DRIV_OUT_CTRL_SM    (1 << 10)
#define LCD_DRIV_OUT_CTRL_SS    (1 << 8)
#define LCD_GATE_SCAN_CTRL1_GS  (1 << 15)
#define LCD_GATE_SCAN_CTRL1_NL320  ((320/8) << 8)

#define LCD_ENTRY_MOD_TRI       (1 << 15)
#define LCD_ENTRY_MOD_DFM       (1 << 14)
#define LCD_ENTRY_MOD_BGR       (1 << 12)
#define LCD_ENTRY_MOD_ORG       (1 << 7)
#define LCD_ENTRY_MOD_VI        (1 << 5)
#define LCD_ENTRY_MOD_HI        (1 << 4)
#define LCD_ENTRY_MOD_AM        (1 << 3)

/*-------------------------------------------------------------------
 
  v-- origin in software
  ___________________ 
 | _______________   |	Landscape
 ||               |[]|	REG_03 = 0x38
 ||               |  |	REG_01 = 0x0000
 || ------        |[]|	REG_60 = 0xA700
 ||      /        |  |
 ||      ----->   |[]|
 ||               |  |
 ||_______________|[]| < S720
 |___________________|
  ^-- LCD's origin (0x0000)
  G320 
  ___________________ 
 | _______________   |	Reversed Landscape
 ||               |[]|	REG_03 = 0x38
 ||               |  |	REG_01 = 0x0100
 || <------       |[]|	REG_60 = 0x2700
 ||       /       |  |
 ||      ------   |[]|
 ||	              |  |
 ||_______________|[]|
 |___________________|
                 ^-- origin in software
  ^-- LCD's origin (0x0000)

  ________________
 | ______________ |		
 ||B             ||		Portrait (default)
 ||              ||		REG_03 = 0x30
 || ------       ||		REG_01 = 0x0100
 ||      /       ||		REG_60 = 0x2700
 ||      ------> ||		
 ||              ||
 ||              ||
 ||_____________E||
 | [] [] [] [] [] |
 |________________|		


  ________________
 | ______________ |		
 ||             B||		Reversed portrait
 ||              ||		REG_03 = 0x20
 ||       ------ ||		REG_01 = 0x0100
 ||      \       ||		REG_60 = 0x2700
 || <-----       ||		
 ||              ||
 ||              ||
 ||E_____________||
 | [] [] [] [] [] |
 |________________|	

*/

//-------------------------------------------------------------------	

#define LCD_ILI9328

#if 0//defined(LCD_LANDSCAPE)
	#define VAL_ENTRY_MOD	0x0038 | BGR_BIT
	#define SHIFT_DIR		0x0000 // clr ss
	#define VAL_GATE_SCAN	0xA700 
	#define GRAM_ADX		LCD_GRAM_VER_AD
	#define GRAM_ADY		LCD_GRAM_HOR_AD
	#define START_ADX   	LCD_VER_START_AD
	#define END_ADX   		LCD_VER_END_AD
	#define START_ADY   	LCD_HOR_START_AD
	#define END_ADY   		LCD_HOR_END_AD
//#elif defined(ORIENTATION270)
	#define VAL_ENTRY_MOD	0x0038 | BGR_BIT
	#define SHIFT_DIR		0x0100 // set ss
	#define VAL_GATE_SCAN	0x2700 
	#define GRAM_ADX		LCD_GRAM_VER_AD
	#define GRAM_ADY		LCD_GRAM_HOR_AD
	#define START_ADX   	LCD_VER_START_AD
	#define END_ADX   		LCD_VER_END_AD
	#define START_ADY   	LCD_HOR_START_AD
	#define END_ADY   		LCD_HOR_END_AD
#else
	#define VAL_ENTRY_MOD	0x0030	| BGR_BIT//16bit interface
	#define SHIFT_DIR		0x0100	 //set SS
	#define VAL_GATE_SCAN	0xA800 
	#define GRAM_ADX		LCD_GRAM_HOR_AD
	#define GRAM_ADY		LCD_GRAM_VER_AD
	#define START_ADX   	LCD_HOR_START_AD
	#define END_ADX   		LCD_HOR_END_AD
	#define START_ADY   	LCD_VER_START_AD
	#define END_ADY   		LCD_VER_END_AD
#endif 

#ifdef BGR_MODE
	#define BGR_BIT 0
#else	
	#define BGR_BIT (1<<12)
#endif

extern const drvlcd_t ili9328_drv;

uint16_t LCD_ReadData(void);
uint16_t LCD_ReadId(void);

#ifdef __cplusplus
};
#endif

#endif

