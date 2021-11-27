
#include "board.h"
#include "spi.h"

#include <LPC17xx.h>


#define SSP0_ConfigPins()                             \
	    LPC_PINCON->PINSEL0 &= ~(SSP0_CLK_PIN_MASK);  \
	    LPC_PINCON->PINSEL1 &= ~(SSP0_PINS_MASK);     \
	    LPC_PINCON->PINSEL0 |= SSP0_CLK_PIN;          \
	    LPC_PINCON->PINSEL1 |= SSP0_PINS;

#define SSP1_ConfigPins()                         \
		LPC_PINCON->PINSEL0 &= ~(SSP1_PINS_MASK); \
		LPC_PINCON->PINSEL0 |= SSP1_PINS;

#define SPI_ConfigPins() PINCON->PINSEL0 = SPI0_PINS;


/* SSPx pins */
#define SSP0_CLK_PIN_MASK (3<<30)
#define SSP0_CLK_PIN      (2<<30)
#define SSP0_PINS_MASK    (0x0F<<2)
#define SSP0_PINS         (0x0A<<2)
#define SSP1_PINS_MASK    (0xFF<<12)
#define SSP1_PINS         (0x2A<<14)
#define SSP1_SSEL         (2<<12)


#define SSP_MAX_CLK 10
#define SSP_MIN_CLK 254


/* pincon function select */
/*Remark: LPC2104/05/06 and LPC2104/05/06/00 configured to operate as a SPI master MUST
select SSEL functionality on P0.7 and have HIGH level on this pin in order to act as a master.*/
#define SPI0_PINS     (0x55<<8)

#define SPI_MAX_CLK   8		//min pckl divider
#define SPI_MAX_FREQ  0

#define SPI_NUM_BUS 2

void SSP_SetPCLK(uint8_t psel, uint8_t ck){	
	switch(ck){
		case 1: CLOCK_SetPCLK(psel, PCLK_1); break;
		case 2: CLOCK_SetPCLK(psel, PCLK_2); break;
		case 4: CLOCK_SetPCLK(psel, PCLK_4); break;
		default:
		case 8: CLOCK_SetPCLK(psel, PCLK_8); break;
	}
}

void SPI_Init(spibus_t *spi){
	uint32_t cpsr;
	uint8_t ck, psel;
	LPC_SSP_TypeDef *sspx;

	if(spi->bus >= SPI_NUM_BUS)
		return;

	if(spi->bus == SPI_BUS0){
		sspx = LPC_SSP0;
        psel = PCLK_SSP0;
	}else{
		sspx = LPC_SSP1;
        psel = PCLK_SSP1;
	}

	sspx->CR0 = 0;
	sspx->CR1 = 0;	

	if(sspx == LPC_SSP0){
		PCONP_SSP0_ENABLE();
		LPC_PINCON->PINSEL0 &= ~(SSP0_CLK_PIN_MASK);	// configure pins
	    LPC_PINCON->PINSEL1 &= ~(SSP0_PINS_MASK);
	    LPC_PINCON->PINSEL0 |= SSP0_CLK_PIN;
	    LPC_PINCON->PINSEL1 |= SSP0_PINS;
	}else{
		PCONP_SSP1_ENABLE();
		LPC_PINCON->PINSEL0 &= ~(SSP1_PINS_MASK);
		LPC_PINCON->PINSEL0 |= SSP1_PINS;
	}

	//calculate ssp prescaler
	for(ck = 8; ck > 0; ck >>= 1){          
		cpsr = (SystemCoreClock/ck)/spi->freq;
		if((cpsr < SSP_MIN_CLK) && (cpsr > SSP_MAX_CLK)){
			SSP_SetPCLK(psel, ck);			
			break;
		}
	}	

	if(!ck){
		SSP_SetPCLK(psel, ck);
		cpsr = SSP_MIN_CLK;
	}

	sspx->CPSR = cpsr & 0xFE;	// must be an even number

	sspx->CR0 = (spi->cfg & 0x0F) - 1; // data size

	switch(spi->cfg & 0xF0){
		case SPI_MODE0: break;
		case SPI_MODE1: sspx->CR0 |= SSP_CR0_CPHA; break;
		case SPI_MODE2: sspx->CR0 |= SSP_CR0_CPOL; break;
		case SPI_MODE3: sspx->CR0 |= SSP_CR0_CPHA | SSP_CR0_CPHA; break;
		default: break;
	}

	sspx->CR1 = SSP_CR1_SSE;       // Enable ssp
	
	spi->ctrl = sspx;
}

void SPI_Write(spibus_t *spi, uint8_t *buffer, uint32_t lenght){

LPC_SSP_TypeDef *sspx = (LPC_SSP_TypeDef*)spi->ctrl;

	while(sspx->SR & SSP_SR_RNE){ // empty fifo
	    LPC_RTC->GPREG0 = sspx->DR;
	}

	if((sspx->CR0 & 0x0F) < 8){	// check if 8 or 16 bit
		while(lenght--){
			sspx->DR = *((uint8_t*)buffer);
			while(sspx->SR & SSP_SR_BSY);
			*((uint8_t*)buffer) = sspx->DR;
			buffer = buffer + 1;
		}
	}else{
		while(lenght--){
			sspx->DR = *((uint16_t*)buffer);
			while(sspx->SR & SSP_SR_BSY);
			*((uint16_t*)buffer) = sspx->DR;
			buffer = buffer + 2;
		}
	}
}

/*
// for compatibility
void SPI_Init(int frequency, int bitData){
	SSP0_PowerUp();
	SSP0_ConfigPins();
	SSP_Init(LPC_SSP0, frequency, bitData);
}
void SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int lenght){

	while(lenght--){

		*rxBuffer++ = SPI_Send(*txBuffer++);

	}
}
*/
uint16_t SPI_Send(spibus_t *spi, uint16_t data){
LPC_SSP_TypeDef *sspx = (LPC_SSP_TypeDef*)spi->ctrl;
	sspx->DR = data;
		while((sspx->SR & SSP_SR_BSY));
	return sspx->DR;
}



