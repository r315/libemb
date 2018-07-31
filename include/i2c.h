/**
* @file		i2c.h
* @brief	Contains the i2c API header.
*     		
* @version	1.0
* @date		6 Apr. 2017
* @author	Hugo Reis
**********************************************************************/

#ifndef _I2C_H_
#define _I2C_H_

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
	REPEATED_START,    //0x01
	SLA_READ,          //0x02
	SLA_WRITE,         //0x03
	DATA_WRITE,        //0x04
	DATA_READ,		   //0x05
	ADDRESS_HIGH,      //0x06
	ADDRESS_LOW,       //0x07
	ERROR_SLA_W_NACK,  //0x08
	ERROR_SLA_R_NACK,  //0x09
	ERROR_DTA_W_NACK,  //0x0A
	ERROR_DTA_R_NACK   //0x0B
};

typedef struct{
	LPC_I2C_TypeDef *interface; // i2c interface to be used
	uint8_t device;             // 8-bit slave address
	uint8_t *data;              // data buffer for r/w
	volatile uint8_t state;     // current state
	uint8_t operation;			// Read/Write
	uint32_t count;             // data size	

}I2C_Controller;

//P0.27 -> SDA0
//P0.28 -> SCL0
#define I2C0_ConfigPins()                        \
	LPC_PINCON->PINSEL1 &= ~((3<<24) | (3<<22)); \
	LPC_PINCON->PINSEL1 |=  ((1<<24) | (1<<22)); \


//P0.19 -> SDA1
//P0.20 -> SCL1
#define I2C1_ConfigPins()                     \
	LPC_PINCON->PINSEL1 |=  (3<<8) | (3<<6);  \


//P0.10 -> SDA2
//P0.11 -> SCL2
#define I2C2_ConfigPins()                        \
	LPC_PINCON->PINSEL1 &= ~((3<<24) | (3<<22)); \
	LPC_PINCON->PINSEL1 |=  ((2<<24) | (2<<22)); \

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
 * \param i2cifc   - i2c controller structure
 * \param ifNumber - Number of hw interface 0-2
 * \param dev      - 8-Bit Slave address
 * 
 * */

I2C_Controller *I2C_Init(uint8_t ifNumber, uint8_t dev);

/**
* @brief Escreve dados de um buffer para o bus
*/
int8_t I2C_Write(uint8_t ifNumber, uint8_t *data, uint32_t size);

/**
* @brief Lê dados do bus i2c para um buffer
**/
int8_t I2C_Read(uint8_t ifNumber, uint8_t *data, uint32_t size);

/**
 * @brief Async read
 * 
 * \param i2citfc	- 	Interface controller
 * \param data		- 	Destination buffer
 * \param size		- 	Number of bytes to be read
 * \param callback	- 	Callback for data ready
 * */
int8_t I2C_ReadAsync(uint8_t ifNumber, uint8_t *data, void (*)(void*));


/**
 * @brief Async Write
 * 
 * \param i2citfc	- 	Interface controller
 * \param data		- 	Source data
 * \param size		- 	Number of bytes to be written
 * \param callback	- 	Callback for data write
 * */
int8_t I2C_WriteAsync(uint8_t ifNumber, uint8_t *data, void (*)(void*));



#endif
