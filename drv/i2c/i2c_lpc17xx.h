/**
* @file		i2c.h
* @brief	Contains the i2c API header.
*     		
* @version	1.0
* @date		6 Apr. 2017
* @author	Hugo Reis
**********************************************************************/

#ifndef _I2C_LPC17XX_H_
#define _I2C_LPC17XX_H_

#include <stdint.h>
#include <LPC17xx.h>

#define I2C_IF0 0
#define I2C_IF1 1
#define I2C_IF2 2

#define I2C0_ON (1<<7)  //PCI2C0
#define I2C1_ON (1<<19) //PCI2C1
#define I2C2_ON (1<<26) //PCI2C2

#define I2C_DEFAULT_CLOCK 100000 //Standard 100Khz

#define I2C_MAX_ITF 3

#define I2C_MAX_TIMEOUT 0x1000000

enum I2C_States{
	I2C_IDLE = 0,
	REPEATED_START, 
	SLA_READ, 
	SLA_WRITE, 
	DATA_WRITE, 
	DATA_READ,
	CALL_CB,           
	ERROR_SLA_NACK,
	ERROR_DTA_W_NACK, 
	ERROR_DTA_R_NACK
};

typedef struct _I2C_Controller{
	LPC_I2C_TypeDef *interface; // i2c interface to be used
	uint8_t device;             // 8-bit slave address
	uint8_t *data;              // data buffer for r/w
	uint32_t size;              // Size of data
	volatile uint8_t state;     // current state
	uint8_t operation;			// Read/Write
	uint32_t count;             // current byte counter	
	void (*cb)(void*);          // callback for async api
}I2C_Controller;

//P0.27 -> SDA0
//P0.28 -> SCL0
#define I2C0_ConfigPins()                        \
	LPC_PINCON->PINSEL1 &= ~((3<<24) | (3<<22)); \
	LPC_PINCON->PINSEL1 |=  ((1<<24) | (1<<22)); \


//P0.0 -> SDA1
//P0.1 -> SCL1
#define I2C1_ConfigPins()                        \
	LPC_PINCON->PINSEL0 |=  (3<<0) | (3<<2);     \


//P0.10 -> SDA2
//P0.11 -> SCL2
#define I2C2_ConfigPins()                        \
	LPC_PINCON->PINSEL0 &= ~((3<<20) | (3<<22)); \
	LPC_PINCON->PINSEL0 |=  ((2<<20) | (2<<22)); \

#define I2C_AA   (1<<2)
#define I2C_SI   (1<<3)
#define I2C_STO  (1<<4)
#define I2C_STA  (1<<5)
#define I2C_I2EN (1<<6)

#define I2C_START               0x08
#define I2C_REPEATEAD_START     0x10
#define I2C_SLA_W_ACK           0x18
#define I2C_SLA_W_NACK          0x20
#define I2C_DTA_W_ACK           0x28
#define I2C_DTA_W_NACK          0x30
#define I2C_SLA_LOST            0x38
#define I2C_SLA_R_ACK           0x40
#define I2C_SLA_R_NACK          0x48
#define I2C_DTA_R_ACK           0x50
#define I2C_DTA_R_NACK          0x58
#define I2C_NO_INFO             0xF8

/**
 * @brief Faz a iniciação do interface i2c (0-2) e define o ritmo de clock
 *
 * \param ifNumber - Number of hw interface 0-2
 * \param dev      - 8-Bit Slave address
 * 
 * \return 		   - pointer to internal controller
 * */

I2C_Controller *I2C_Init(uint8_t ifNumber, uint8_t dev);

/**
* @brief Escreve dados de um buffer para o bus
*/
int32_t I2C_Write(uint8_t ifNumber, uint8_t *data, uint32_t size);

/**
* @brief Lê dados do bus i2c para um buffer
**/
int32_t I2C_Read(uint8_t ifNumber, uint8_t *data, uint32_t size);

/**
 * @brief Async I2C bus read
 * 
 * \param i2citfc	- 	Interface controller
 * \param data		- 	Destination buffer
 * \param size		- 	Number of bytes to be read
 * \param callback	- 	Callback for data ready
 * 
 * \return 			-	size No error, 0 error ocurred 
 * */
int32_t I2C_ReadAsync(uint8_t ifNumber, uint8_t *data, uint32_t size, void (*)(void*));


/**
 * @brief Async I2C bus Write
 * 
 * \param i2citfc	- 	Interface controller
 * \param data		- 	Source data
 * \param size		- 	Number of bytes to be written
 * \param callback	- 	Callback for data write
 * 
 * \return 			-	size No error, 0 error ocurred   
 * */
int32_t I2C_WriteAsync(uint8_t ifNumber, uint8_t *data, uint32_t size, void (*)(void*));

#endif
