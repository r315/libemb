#ifndef _spi_lpc17xx_h_
#define _spi_lpc17xx_h_

#ifdef __cplusplus
extern "C" {
#endif

/**
SPI Pins:
  SCK       P0.15
  SSEL      P0.16
  MISO      P0.17
  MOSI      P0.18

SSP0 Pins:
  SCK0      P0.15     P1.20
  SSEL0     P0.16     P1.21
  MISO0     P0.17     P1.23
  MOSI0     P0.18     P1.24
  
SSP1 Pins:
  SCK1      P0.7      P1.31
  SSEL1     P0.6      -
  MISO1     P0.8      -
  MOSI1     P0.9      -
*/

// SSPx
#define SSP_CR0_DSS_8       7
#define SSP_CR0_DSS_16      15
#define SSP_CR0_CPOL        (1<<6)  // idle level
#define SSP_CR0_CPHA        (1<<7)  // data valid
#define SSP_CR1_SSE         (1<<1)  // ssp enable
#define SSP_MS              (1<<2)  // master mode
#define SSP_SOD             (1<<3)  // slave output disable
#define SSP_TFE             (1<<0)
#define SSP_TNF             (1<<1)

#define SSP_SR_TFE          (1<<0)
#define SSP_SR_TNF          (1<<1)
#define SSP_SR_RNE          (1<<2)
#define SSP_SR_RFF          (1<<3)
#define SSP_SR_BSY          (1<<4)

/* SPI */
#define SPI_SPCR_EN         (1 << 2) //transfered bits selected by bits 11:8
#define SPI_SPCR_CPHA       (1 << 3)
#define SPI_SPCR_CPOL       (1 << 4)
#define SPI_SPCR_MSTR       (1 << 5) //Master mode
#define SPI_SPCR_LSBF       (1 << 6)
#define SPI_SPCR_SPIE       (1 << 7)

#define SPI_SPSR_ABRT       (1 << 3)
#define SPI_SPSR_MODF       (1 << 4)
#define SPI_SPSR_ROVR       (1 << 5)
#define SPI_SPSR_WCOL       (1 << 6)
#define SPI_SPSR_SPIF       (1 << 7)

#define SPI_NUM_BUS         2

#define SSP_MAX_PCLK_DIV    8
#define SSP_MIN_PRES        10
#define SSP_MAX_PRES        254


#ifdef __cplusplus
}
#endif

#endif