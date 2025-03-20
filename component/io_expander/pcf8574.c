
#include <stdint.h>
#include "pcf8574.h"

void IOEXP_Init(i2cbus_t *i2c)
{
    // I/Os should be high before being used as inputs.
    uint8_t data = 0xFF;
    I2C_Write(i2c, PCF8574_I2C_ADDRESS, (const uint8_t*)&data, 1);
}

uint8_t IOEXP_Read(i2cbus_t *i2c)
{
    uint8_t data;
    I2C_Read(i2c, PCF8574_I2C_ADDRESS, &data, 1);
    return data;
}

void IOEXP_Write(i2cbus_t *i2c, uint8_t data)
{
    I2C_Write(i2c, PCF8574_I2C_ADDRESS, &data, 1);
}

void IOEXP_Set(i2cbus_t *i2c, uint8_t mask)
{
    uint8_t data;
    I2C_Read(i2c, PCF8574_I2C_ADDRESS, &data, 1);
    data |= mask;
    I2C_Write(i2c, PCF8574_I2C_ADDRESS, &data, 1);
}

void IOEXP_Clr(i2cbus_t *i2c, uint8_t mask)
{
    uint8_t data;
    I2C_Read(i2c, PCF8574_I2C_ADDRESS, &data, 1);
    data &= ~mask;
    I2C_Write(i2c, PCF8574_I2C_ADDRESS, &data, 1);
}