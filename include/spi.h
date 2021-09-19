/**
* @file		spi.h
* @brief	Contains the spi API header.
*     		
* @version	1.0
* @date		5 Nov. 2016
* @author	Hugo Reis
**********************************************************************/

#ifndef _SPI_H_
#define _SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


/*
SPI modes                                 0    1    2    3
               _   _   _   _ 
CPOL = 0    __| |_| |_| |_| |__           X    X         
            __   _   _   _   __
CPOL = 1      |_| |_| |_| |_|                       X    X
          __ ___ ___ ___ ___ ___ ___   
CPHA = 0  __X___X___X___X___X___X___      X         X     
          ____ ___ ___ ___ ___ ___ ___ 
CPHA = 1  ____X___X___X___X___X___X___         X         X

*/

#define SPI_BUS0	0
#define SPI_BUS1	1
#define SPI_BUS2	2

enum spimode_e{
	SPI_MODE0 = 0x00,
	SPI_MODE1 = 0x10,
	SPI_MODE2 = 0x20,
	SPI_MODE3 = 0x30,
};

enum spiflags_e{
	SPI_IDLE = 0,
	SPI_DMA_NO_MINC = (1 << 0),
	SPI_16BIT = (1 << 1),
	SPI_BUSY = (1 << 2),
    SPI_SW_CS = (1 << 3)
};

typedef struct spidev{
	void *ctrl;		// CMSIS compliant controller
	void *dma;      // DMA channel/controller
	uint8_t  bus;   // bus number 0,1...
	uint32_t freq;  // Clock frequency in khz
	uint8_t  cfg;   // MSB: Mode, LSB: databits
	uint32_t trf_counter;
	void (*eot_cb)(void);
}spidev_t;

void SPI_Init(spidev_t *spidev);
void SPI_BeginTransfer(spidev_t *spidev, int csBitId);
void SPI_EndTransfer(spidev_t *spidev, int csBitId);
void SPI_Write(spidev_t *spidev, uint8_t *src, uint32_t count);
void SPI_WriteDMA(spidev_t *spidev, uint16_t *data, uint32_t count);
void SPI_WriteIntDMA(spidev_t *spidev, uint16_t data, uint32_t count);
void SPI_WaitEOT(spidev_t *spidev);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_H_ */
