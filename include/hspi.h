#ifndef INCLUDE_HSPI_H_
#define INCLUDE_HSPI_H_

#include <c_types.h>
#include <esp8266/pin_mux_register.h>
#include <driver/spi_register.h>
#include <esp8266/esp8266.h>
#include <gpio.h>

#define SPI         0
#define HSPI        1
#define CS_LOW      0
#define CS_HIGH     1

#define SPIFIFOSIZE 16 //16 words length

typedef enum
{
	HSPI_MODE_TX,
	HSPI_MODE_TX_RX
}Hspi_Mode;

extern uint32_t *spi_fifo;
extern uint8_t hspi_cs_gpio;

void HSPI_Init(uint32_t prescaler, Hspi_Mode mode);
void HSPI_Send_Data(const uint8_t * data, uint8_t datasize);

static inline void HSPI_Wait_Ready(void){while (READ_PERI_REG(SPI_CMD(HSPI))&SPI_USR);}
static inline void HSPI_Set_CS(uint8_t level){if(hspi_cs_gpio != (uint8_t)(-1)) GPIO_OUTPUT_SET(hspi_cs_gpio, level);}

static inline void HSPI_Prepare_TX(uint32_t bytecount)
{
	uint32_t bitcount = bytecount * 8 - 1;
	HSPI_Wait_Ready();
	HSPI_Set_CS(CS_HIGH);
	WRITE_PERI_REG(SPI_USER1(HSPI), (bitcount & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S);
}

static inline void HSPI_Start_TX()
{
	SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);   // send	
	HSPI_Wait_Ready();
}

static inline void HSPI_Send(uint8_t data)
{
	HSPI_Prepare_TX(1);
	*spi_fifo = data;
	HSPI_Start_TX();
}

/**
 * Note when selecting HSPI_MODE_TX_RX, GPIO12 cannot be 
 * used as CS
 * */
static inline void HSPI_Configure_CS(uint8_t gpio_no){
	hspi_cs_gpio = gpio_no;
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15); // Disable CS on GPIO15
}

#endif /* INCLUDE_HSPI_H_ */
