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

enum{
	SPI_MODE0 = 0x00,
	SPI_MODE1 = 0x10,
	SPI_MODE2 = 0x20,
	SPI_MODE3 = 0x30,
};


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


#define SPI_8BIT  8
#define SPI_16BIT 16

#define SPI_BUS0	0
#define SPI_BUS1	1
#define SPI_BUS2	2

typedef struct{
	void *dev;
	uint32_t freq;
	uint8_t  cfg;  // MSB: Mode, LSB: databits
	uint8_t  bus;
}Spi_Type;


/**
* @brief Faz a iniciação do controlador, configurando os pinos, o ritmo de envio e o
*        numero de bits de dados.
*/
void SPI_Init(Spi_Type *spi);

/**
* @brief Coloca ativo o chip select do dispositivo slave
*/
void SPI_BeginTransfer(int csBitId);

/**
* @brief Coloca desativo o chip select do dispositivo slave
**/
void SPI_EndTransfer(int csBitId);

/**
* @brief Realiza uma transferencia. 
**/
//void SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int lenght);
void SPI_Transfer(Spi_Type *spi, void *buffer, uint16_t lenght);

/**
* @brief initiates and 8bit data transfer
*		 rev 00 only suports 8bit
**/
uint16_t SPI_Send(Spi_Type *spi, uint16_t data);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_H_ */
