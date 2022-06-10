#ifndef _spi_lpc17xx_h_
#define _spi_lpc17xx_h_

#ifdef __cplusplus
extern "C" {
#endif

// SSPx
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


#ifdef __cplusplus
}
#endif

#endif