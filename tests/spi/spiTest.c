
#include <libemb.h>
#include <spi.h>



void main(void){
Spi_Type spi;

uint8_t buf[32];

	CLOCK_Init(CCLK_100);

	spi.bus = SPI_BUS0;
	spi.freq = 500000;
	spi.cfg = SPI_MODE0 | SPI_8BIT;	

	SPI_Init(&spi);

	ACCEL_Init(&spi);

	for(uint8_t i = 0; i < 32; i++){
		buf[i] = i;
	}

	while(1){

		SPI_Transfer(&spi, buf, 32);
		
		DelayMs(100);
	}

}
