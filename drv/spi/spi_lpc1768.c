

#include <clock.h>
#include <spi_lpc1768.h>


void SSP_SetPCLK(LPC_SSP_TypeDef *sspx, uint8_t ck){
uint8_t sBit;
uint32_t *pksel;

	if(sspx == LPC_SSP0){
		pksel = (uint32_t*)&LPC_SC->PCLKSEL1;
		sBit = PCLK_SSP0;
	}else{
		pksel = (uint32_t*)&LPC_SC->PCLKSEL0;
		sBit = PCLK_SSP1;
	}

	*pksel &= ~(3<<sBit);  //set default pclock

	switch(ck){
		case 1: *pksel |= (PCLK_1<<sBit); break; //SystemCoreClock
		case 2: *pksel |= (PCLK_2<<sBit); break; //SystemCoreClock / 2
		case 4: *pksel |= (PCLK_4<<sBit); break; //SystemCoreClock / 4
		default:
		case 8: *pksel |= (PCLK_8<<sBit); break; //SystemCoreClock / 8
	}
}

void SSP_Init(LPC_SSP_TypeDef *sspx, uint32_t speed, uint16_t dss){
uint32_t cpsr;
uint8_t ck;

	sspx->CR0 = 0;
	sspx->CR1 = 0;	

	if(sspx == LPC_SSP0){
		LPC_SC->PCONP |= SSP0_ON;						// power up module
		LPC_PINCON->PINSEL0 &= ~(SSP0_CLK_PIN_MASK);	// configure pins
	    LPC_PINCON->PINSEL1 &= ~(SSP0_PINS_MASK);
	    LPC_PINCON->PINSEL0 |= SSP0_CLK_PIN;
	    LPC_PINCON->PINSEL1 |= SSP0_PINS;
	}else{
		LPC_SC->PCONP |= SSP1_ON;
		LPC_PINCON->PINSEL0 &= ~(SSP1_PINS_MASK);
		LPC_PINCON->PINSEL0 |= SSP1_PINS;
	}

	for(ck = 8; ck > 0; ck >>= 1){          //calculate ssp prescaler
		cpsr = (SystemCoreClock/ck)/speed;  // CPSR = cclk/PCLKSEL/spi speed
		if((cpsr < SSP_MIN_CLK) && (cpsr > SSP_MAX_CLK)){
			SSP_SetPCLK(sspx, ck);			
			break;
		}
	}	

	if(!ck){
		SSP_SetPCLK(sspx, ck);
		cpsr = SSP_MIN_CLK;
	}

	sspx->CPSR = cpsr & 0xFE;	// must be an even number

	sspx->CR0 = (dss & 0xFF) - 1; // data size

	switch(dss >> 8 ){
		case SPI_MODE0: break;
		case SPI_MODE1: sspx->CR0 |= SSP_CPHA; break;
		case SPI_MODE2: sspx->CR0 |= SSP_CPOL; break;
		case SPI_MODE3: sspx->CR0 |= SSP_CPHA | SSP_CPHA; break;
		default: break;
	}

	sspx->CR1 = SSP_SSE;       // Enable ssp
}

void SSP_Transfer(LPC_SSP_TypeDef *sspx, void *buffer, uint16_t lenght){
volatile uint16_t dmy;

	while(sspx->SR & SSP_SR_RNE){ // empty fifo
		dmy = sspx->DR;
	}

	if((sspx->CR0 & 0x0F) < 8){	// check if 8 or 16 bit
		while(lenght--){
			sspx->DR = *((uint8_t*)buffer);
			while(sspx->SR & SSP_SR_BSY);
			*((uint8_t*)buffer) = sspx->DR;
			buffer = (uint8_t*)buffer + 1;
		}
	}else{
		while(lenght--){
			sspx->DR = *((uint16_t*)buffer);
			while(sspx->SR & SSP_SR_BSY);
			*((uint16_t*)buffer) = sspx->DR;
			buffer = (uint16_t*)buffer + 1;
		}
	}
}


// for compatibility
void SPI_Init(int frequency, int bitData){
	SSP0_PowerUp();
	SSP0_ConfigPins();
	SSP_Init(LPC_SSP0, frequency, bitData);
}

uint16_t SPI_Send(uint16_t data){
	LPC_SSP0->DR = data;
		while((LPC_SSP0->SR & SSP_SR_BSY));
	return LPC_SSP0->DR;
}

void SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int lenght){
	while(lenght--){
		*rxBuffer++ = SPI_Send(*txBuffer++);
	}
}

