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

enum i2ccfg_e{
    I2C_CFG_DMA       = (1 << 1),
};

enum i2cx_e{
    I2C_BUS0 = 0,
    I2C_BUS1,
    I2C_BUS2,
    I2C_BUS3
};

typedef enum i2cerr_e {
    I2C_SUCCESS,
    I2C_ERR,
    I2C_ERR_PARM,
    I2C_ERR_BUSY,
    I2C_ERR_TIMEOUT,
    I2C_ERR_START,
    I2C_ERR_STOP,
    I2C_ERR_ADDR,
    I2C_ERR_ACK,
    I2C_ERR_TXE
}i2cerr_t;

typedef struct {
    void *handle;
    uint8_t addr;       // own address
    uint32_t speed;
    uint8_t cfg;
    uint8_t bus_num;    // Bus number
}i2cbus_t;

uint32_t I2C_Init(i2cbus_t *i2cbus);
void I2C_Reset(i2cbus_t *i2cbus);
/**
 * @brief Basic I2C transfers
 *
 * @param i2c   Structure that holds i2c handler
 * @param addr  7-bit Device address
 * @param data  Data to be sent/received
 * @param size  Number of bytes in data
 * @return uint16_t number of bytes transfered
 */
uint16_t I2C_Write(i2cbus_t *i2cbus, uint8_t addr, const uint8_t *data, uint16_t size);
uint16_t I2C_Read(i2cbus_t *i2cbus, uint8_t addr, uint8_t *data, uint16_t size);

uint32_t I2C_TransmitDMA(i2cbus_t *i2cbus, uint8_t addr, const uint8_t *data, uint16_t size);
/**
 * @brief Configure a End Of Transfer callback.
 * Using a separated function instead of a struct member
 * allows setting a callback at any time by any driver/app
 * @param i2cbus
 * @param eot
 */
void I2C_SetEOT(i2cbus_t *i2cbus, void (*eot)(void));
void I2C_WaitEOT(i2cbus_t *i2cbus);
i2cerr_t I2C_Busy(i2cbus_t *i2cbus);

#ifdef __cplusplus
}
#endif

#endif
