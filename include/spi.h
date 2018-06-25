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

#include <stdint.h>

#if defined(__ESP03__) 
#include <hspi.h>
#include <esp8266/esp8266.h>
#define SPI_Send HSPI_Send
#else  /* __ESP03__ */
/**
* @brief initiates and 8bit data transfer
*		 rev 00 only suports 8bit
**/
uint16_t SPI_Send(uint16_t data);
#endif

#if defined(__BB__)
#include <spi_lpc1768.h>
#endif

/**
* @brief Faz a iniciação do controlador, configurando os pinos, o ritmo de envio e o
*        numero de bits de dados.
*/
void SPI_Init(int frequency, int bitData);

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
void SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int lenght);


#endif /* _SPI_H_ */
