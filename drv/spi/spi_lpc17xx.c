#include "lpc17xx_hal.h"
#include "spi.h"

static void SSP_SetPCLK(uint8_t psel, uint8_t ck){	
	switch(ck){
		case 1: CLOCK_SetPCLK(psel, PCLK_1); break;
		case 2: CLOCK_SetPCLK(psel, PCLK_2); break;
		case 4: CLOCK_SetPCLK(psel, PCLK_4); break;
		default:
		case 8: CLOCK_SetPCLK(psel, PCLK_8); break;
	}
}

void SPI_Init(spibus_t *spi){
	uint32_t data;
	uint8_t ck, psel;
	LPC_SSP_TypeDef *sspx;


	switch(spi->bus){
		default:
			return;

		case SPI_BUS0:
			PCONP_SSP0_ENABLE;
			sspx = LPC_SSP0;
        	psel = PCLK_SSP0;			
			/*Remark: LPC2104/05/06 and LPC2104/05/06/00 configured to operate as a SPI master MUST
			select SSEL functionality on P0.7 and have HIGH level on this pin in order to act as a master.*/
			LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~(P0_15_FUNC_MASK)) | (P0_15_SCK0 << P0_15_FUNC_POS);
			LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(P0_17_FUNC_MASK | P0_18_FUNC_MASK)) | 
							  (P0_17_MISO0 << P0_17_FUNC_POS) | (P0_18_MOSI0 << P0_18_FUNC_POS);
			if(spi->flags & SPI_HW_CS){
				GPIO_Init(P0_16, P0_16_SSEL0);
			}
			break;

		case SPI_BUS1:
			PCONP_SSP1_ENABLE;
			sspx = LPC_SSP1;
        	psel = PCLK_SSP1;
			LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~(P0_7_FUNC_MASK | P0_8_FUNC_MASK | P0_9_FUNC_MASK)) | 
							(P0_7_SCK1 << P0_7_FUNC_POS) | (P0_8_MISO1 << P0_8_FUNC_POS) | (P0_9_MOSI1 << P0_9_FUNC_POS);
			if(spi->flags & SPI_HW_CS){
				GPIO_Init(P0_6, P0_6_SSEL1);
			}
			break;

		case SPI_BUS2:	// Alternative pins for SSP0
			PCONP_SSP0_ENABLE;
			sspx = LPC_SSP0;
        	psel = PCLK_SSP0;
			LPC_PINCON->PINSEL3 = (LPC_PINCON->PINSEL0 & ~(P1_20_FUNC_MASK | P1_23_FUNC_MASK | P1_24_FUNC_MASK)) | 
							(P1_20_SCK0 << P1_20_FUNC_POS) | (P1_23_MISO0 << P1_23_FUNC_POS) | (P1_24_MOSI0 << P1_24_FUNC_POS);
			
			if(spi->flags & SPI_HW_CS){
				GPIO_Init(P1_21, P1_21_SSEL0);
			}
			break;

	}

	sspx->CR0 = 0;
	sspx->CR1 = 0;

	//calculate ssp prescaler
	for(ck = SSP_MAX_PCLK_DIV; ck > 0; ck >>= 1){          
		data = (SystemCoreClock/ck)/spi->freq;
		if((data < SSP_MAX_PRES) && (data > SSP_MIN_PRES)){
			SSP_SetPCLK(psel, ck);			
			break;
		}
	}	

	if(!ck){
		// On fail to find prescaler, use maximum divisor 
		SSP_SetPCLK(psel, ck);
		data = SSP_MAX_PRES;
	}

	sspx->CPSR = data & 0xFE;	// must be an even number

	data = (spi->flags & SPI_16BIT) ? SSP_CR0_DSS_16 : SSP_CR0_DSS_8;

	switch(spi->flags & 0xC0){
		case SPI_MODE0: break;
		case SPI_MODE1: data |= SSP_CR0_CPHA; break;
		case SPI_MODE2: data |= SSP_CR0_CPOL; break;
		case SPI_MODE3: data |= SSP_CR0_CPOL | SSP_CR0_CPHA; break;
		default: break;
	}

	sspx->CR0 = data;

	sspx->CR1 = SSP_CR1_SSE;       // Enable ssp
	
	spi->ctrl = sspx;
	spi->flags |= SPI_ENABLED;
}

void SPI_Transfer(spibus_t *spi, uint8_t *buffer, uint32_t lenght){

	LPC_SSP_TypeDef *sspx = (LPC_SSP_TypeDef*)spi->ctrl;

	while(sspx->SR & SSP_SR_RNE){ // empty fifo
	    LPC_RTC->GPREG0 = sspx->DR;
	}

	if(spi->flags & SPI_16BIT){
		while(lenght--){
			sspx->DR = *((uint16_t*)buffer);
            while((sspx->SR & SSP_SR_BSY)){}
			*((uint16_t*)buffer) = sspx->DR;
			buffer = buffer + 2;
		}
	}else{
		while(lenght--){
			sspx->DR = *((uint8_t*)buffer);
            while((sspx->SR & SSP_SR_BSY)){}
			*((uint8_t*)buffer) = sspx->DR;
			buffer = buffer + 1;
		}
	}
}

uint16_t SPI_Send(spibus_t *spi, uint16_t data){
	LPC_SSP_TypeDef *sspx = (LPC_SSP_TypeDef*)spi->ctrl;
	sspx->DR = data;
	while((sspx->SR & SSP_SR_BSY)){}
	return sspx->DR;
}

void SPI_WriteDMA(spibus_t *spi, uint16_t *data, uint32_t count){
	//LPC_SSP_TypeDef *sspx = (LPC_SSP_TypeDef*)spi->ctrl;	
}

//------------------------------
// Interrupt Handlers
//------------------------------
void SPI_IRQHandler(void){

}

void SSP0_IRQHandler(void){

}

void SSP1_IRQHandler(void){
	
}