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
    uint8_t addr;               // own address
    uint32_t speed;
    i2cbusnum_t bus_num;        // Bus number
}i2cbus_t;


void I2C_Init(i2cbus_t *i2c);
void I2C_Reset(i2cbus_t *i2c);
/**
 * @brief Basic I2C transfers
 *
 * @param i2c   Structure that holds i2c handler
 * @param addr  7-bit Device address
 * @param data  Data to ve sent/received
 * @param size  Number of bytes in data
 * @return uint16_t number of bytes transfered
 */
uint16_t I2C_Write(i2cbus_t *i2c, uint8_t addr, const uint8_t *data, uint16_t size);
uint16_t I2C_Read(i2cbus_t *i2c, uint8_t addr, uint8_t *data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
