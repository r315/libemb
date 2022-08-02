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
#include "i2c.h"

#define I2C_MAX_ITF 3
#define I2C_IF0 	0
#define I2C_IF1 	1
#define I2C_IF2 	2

#define I2C_DEFAULT_CLOCK 100000 //Standard 100Khz

#define I2C_MAX_TIMEOUT 0x1000000

typedef enum {
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
}i2cstates_e;

typedef struct _I2C_Controller{
	i2cbus_t *i2c;
	LPC_I2C_TypeDef *interface; // i2c interface to be used
	uint8_t device;             // 8-bit slave address
	uint8_t *data;              // data buffer for r/w
	uint32_t size;              // Size of data
	uint8_t operation;			// Read/Write
	uint32_t count;             // current byte counter	
	volatile i2cstates_e state; // current state
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

// I2CONCLR
#define I2C_CONCLR_AAC   (1<<2)
#define I2C_CONCLR_SIC   (1<<3)
#define I2C_CONCLR_STAC  (1<<5)
#define I2C_CONCLR_I2ENC (1<<6)

// I2CONSET
#define I2C_CONSET_AA    (1<<2)
#define I2C_CONSET_SI    (1<<3)
#define I2C_CONSET_STO   (1<<4)
#define I2C_CONSET_STA   (1<<5)
#define I2C_CONSET_I2EN  (1<<6)

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

#endif
