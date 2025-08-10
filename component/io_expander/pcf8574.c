
#include <stdint.h>
#include "pcf8574.h"

ioexp_t pcf8574_ioexp = {
    .address = PCF8574_I2C_ADDRESS,
    .init = pcf8574_init,
    .read = pcf8574_read,
    .write = pcf8574_write,
    .set = pcf8574_set,
    .clr = pcf8574_clr
};

void pcf8574_init(i2cbus_t *i2c)
{
    // I/Os should be high before being used as inputs.
    uint8_t data = 0xFF;
    I2C_Write(i2c, pcf8574_ioexp.address, (const uint8_t*)&data, 1);
}

uint8_t pcf8574_read(i2cbus_t *i2c)
{
    uint8_t data;
    I2C_Read(i2c, pcf8574_ioexp.address, &data, 1);
    return data;
}

void pcf8574_write(i2cbus_t *i2c, uint8_t data)
{
    I2C_Write(i2c, pcf8574_ioexp.address, &data, 1);
}

void pcf8574_set(i2cbus_t *i2c, uint8_t mask)
{
    uint8_t data;
    I2C_Read(i2c, pcf8574_ioexp.address, &data, 1);
    data |= mask;
    I2C_Write(i2c, pcf8574_ioexp.address, &data, 1);
}

void pcf8574_clr(i2cbus_t *i2c, uint8_t mask)
{
    uint8_t data;
    I2C_Read(i2c, pcf8574_ioexp.address, &data, 1);
    data &= ~mask;
    I2C_Write(i2c, pcf8574_ioexp.address, &data, 1);
}