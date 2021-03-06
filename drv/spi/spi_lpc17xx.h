#ifndef _SPI_17xx_H_
#define _SPI_17xx_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <LPC17xx.h>

#define SPI_PowerUp() SC->PCONP |= SPI0_ON
#define SSP0_PowerUp() LPC_SC->PCONP |= SSP0_ON;
#define SSP1_PowerUp() LPC_SC->PCONP |= SSP1_ON;

#define SSP0_ConfigPins()                             \
	    LPC_PINCON->PINSEL0 &= ~(SSP0_CLK_PIN_MASK);  \
	    LPC_PINCON->PINSEL1 &= ~(SSP0_PINS_MASK);     \
	    LPC_PINCON->PINSEL0 |= SSP0_CLK_PIN;          \
	    LPC_PINCON->PINSEL1 |= SSP0_PINS;

#define SSP1_ConfigPins()                         \
		LPC_PINCON->PINSEL0 &= ~(SSP1_PINS_MASK); \
		LPC_PINCON->PINSEL0 |= SSP1_PINS;

#define SPI_ConfigPins() PINCON->PINSEL0 = SPI0_PINS;

#define SPI0_ON (1<<8)
#define SSP0_ON (1<<21)
#define SSP1_ON (1<<10)

/* SSPx pins */
#define SSP0_CLK_PIN_MASK (3<<30)
#define SSP0_CLK_PIN      (2<<30)
#define SSP0_PINS_MASK    (0x0F<<2)
#define SSP0_PINS         (0x0A<<2)
#define SSP1_PINS_MASK    (0xFF<<12)
#define SSP1_PINS         (0x2A<<14)
#define SSP1_SSEL         (2<<12)

#define SSP_CPOL (1<<6)  // idle level
#define SSP_CPHA (1<<7)  // data valid
#define SSP_SSE  (1<<1)  // ssp enable
#define SSP_MS   (1<<2)  // master mode
#define SSP_SOD  (1<<3)  // slave output disable
#define SSP_TFE  (1<<0)
#define SSP_TNF  (1<<1)

// Status register bits
#define SSP_SR_TFE (1<<0)
#define SSP_SR_TNF (1<<1)
#define SSP_SR_RNE (1<<2)
#define SSP_SR_RFF (1<<3)
#define SSP_SR_BSY (1<<4)

#define SSP_MAX_CLK 10
#define SSP_MIN_CLK 254

/* Control bits */
#define SPI0_EN_NBITS (1<<2)	//transfered bits selected by bits 11:8
#define SPI0_CPHA     (1<<3)
#define SPI0_CPOL     (1<<4)
#define SPI0_MSTR     (1<<5)	//Master mode
#define SPI0_LSBF     (1<<6)
#define SPI0_SPIE     (1<<7)

#define SPI0_MIN_DIV 8

/* status register bits */
#define SPI0_SPIF     (1<<7)

/* pincon function select */
/*Remark: LPC2104/05/06 and LPC2104/05/06/00 configured to operate as a SPI master MUST
select SSEL functionality on P0.7 and have HIGH level on this pin in order to act as a master.*/
#define SPI0_PINS     (0x55<<8)

#define SPI_MAX_CLK   8		//min pckl divider
#define SPI_MAX_FREQ  0

#define SPI_NUM_BUS 2

#ifdef __cplusplus
}
#endif

#endif /* _SPI_1768_H_ */

