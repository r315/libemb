#include <hspi.h>
#include <gpio.h>

#define __min(a,b) ((a > b) ? (b):(a))

uint32_t *spi_fifo;
uint8_t hspi_cs_gpio;

void HSPI_Init(uint32_t prescaler, Hspi_Mode mode)
{
uint32_t regvalue;
	WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105);     // Clear bit9
	
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_HSPIQ_MISO); // GPIO12
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_HSPID_MOSI); // GPIO13
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_HSPI_CLK);   // GPIO14
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_HSPI_CS0);   // GPIO15

	// SPI clock = CPU clock / 10 / 4
	// time length HIGHT level = (CPU clock / 10 / 2) ^ -1,
	// time length LOW level = (CPU clock / 10 / 2) ^ -1
	WRITE_PERI_REG(SPI_CLOCK(HSPI), ((((prescaler - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
			   ((1 & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |
			   ((0 & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |
			   ((1 & SPI_CLKCNT_L) << SPI_CLKCNT_L_S)));

	spi_fifo = (uint32_t*)SPI_W0(HSPI);
	regvalue = SPI_USR_MOSI;

	WRITE_PERI_REG(SPI_CTRL1(HSPI), 0);

	switch(mode)
	{
		case HSPI_MODE_TX:
		    regvalue &= ~(BIT2 | SPI_USR_ADDR | SPI_USR_DUMMY | SPI_USR_MISO | SPI_USR_COMMAND | BIT0);
			break;
		case HSPI_MODE_TX_RX:			
			regvalue |= BIT0 | SPI_CK_I_EDGE;
		    regvalue &= ~(BIT2 | SPI_USR_ADDR | SPI_USR_DUMMY | SPI_USR_MISO | SPI_USR_COMMAND);
			break;
	}

	WRITE_PERI_REG(SPI_USER(HSPI), regvalue);
	hspi_cs_gpio = (uint8_t)(-1);
}

/**
 * Send data is limited to 64 bytes
 * */
void HSPI_Send_Data(const uint8_t * data, uint8_t datasize)
{
	uint32_t *_data = (uint32_t*)data;
	uint8_t i;

	uint8_t words_to_send = __min((datasize + 3) / 4, SPIFIFOSIZE);
	HSPI_Prepare_TX(datasize);
	for(i = 0; i < words_to_send;i++)
		spi_fifo[i] = _data[i];
	HSPI_Start_TX();
}

/*
void spi_send_uint16_r(uint16_t data, int32_t repeats)
{
	uint32_t i;
	uint32_t word = data << 16 | data;

	while(repeats > 0)
	{
		uint16_t bytes_to_transfer = __min(repeats * sizeof(uint16_t) , SPIFIFOSIZE * sizeof(uint32_t));
		spi_wait_ready();
		spi_prepare_tx(bytes_to_transfer);
		for(i = 0; i < (bytes_to_transfer + 3) / 4;i++)
			spi_fifo[i] = word;
		spi_start_tx();
		repeats -= bytes_to_transfer / 2;
	}
}
*/
