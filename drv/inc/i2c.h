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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


typedef enum {
    I2C_BUS0 = 0,
    I2C_BUS1,
    I2C_BUS2,
    I2C_BUS3
}i2cbusnum_t;

typedef struct {
    void *peripheral;
    uint8_t addr;               // Slave address
    uint32_t speed;
    i2cbusnum_t bus_num;        // Bus number
}i2cbus_t;


void I2C_Init(i2cbus_t *i2c);
uint32_t I2C_Write(i2cbus_t *i2c, const uint8_t *data, uint32_t size);
uint32_t I2C_Read(i2cbus_t *i2c, uint8_t *data, uint32_t size);
void I2C_Reset(i2cbus_t *i2c);
static inline void I2C_SetSlave(i2cbus_t *i2c, uint8_t slave){i2c->addr = slave;} // 7bit address

#ifdef __cplusplus
}
#endif

#endif
