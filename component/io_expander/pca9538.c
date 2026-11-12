
#include <stdint.h>
#include "pca9538.h"

ioexp_t pca9538_ioexp = {
    .address = PCA9538_I2C_ADDRESS,
    .init = pca9538_init,
    .read = pca9538_read,
    .write = pca9538_write,
    .set = pca9538_set,
    .clr = pca9538_clr
};

static void pca9538_io_write(i2cbus_t *i2c, uint8_t mask, uint8_t set)
{
    uint8_t buf[2] = {PCA9538_CFG_REG, 0};
    // set mask bits as output
    I2C_Write(i2c, pca9538_ioexp.address, buf, 1);
    I2C_Read(i2c, pca9538_ioexp.address, buf, 1);
    buf[1] = buf[0] & ~mask;
    buf[0] = PCA9538_CFG_REG;
    I2C_Write(i2c, pca9538_ioexp.address, buf, 2);
    // Now write the bits
    buf[0] = PCA9538_IN_REG;
    I2C_Write(i2c, pca9538_ioexp.address, buf, 1);
    I2C_Read(i2c, pca9538_ioexp.address, buf, 1);
    buf[1] = set ? (buf[0] | mask) : (buf[0] & ~mask);
    buf[0] = PCA9538_OUT_REG;
    I2C_Write(i2c, pca9538_ioexp.address, buf, 2);
}

void pca9538_init(i2cbus_t *i2c)
{
    const uint8_t data[2] = {PCA9538_CFG_REG, 0xFF};
    // Make all ios input
    I2C_Write(i2c, pca9538_ioexp.address, data, 2);
}

uint8_t pca9538_read(i2cbus_t *i2c)
{
    uint8_t data = 0;
    I2C_Write(i2c, pca9538_ioexp.address, &data, 1);
    I2C_Read(i2c, pca9538_ioexp.address, &data, 1);
    return data;
}

void pca9538_write(i2cbus_t *i2c, uint8_t data)
{
    const uint8_t buf[2] = {PCA9538_OUT_REG, data};
    I2C_Write(i2c, pca9538_ioexp.address, buf, 2);
}

void pca9538_set(i2cbus_t *i2c, uint8_t mask)
{
    pca9538_io_write(i2c, mask, 1);
}

void pca9538_clr(i2cbus_t *i2c, uint8_t mask)
{
    pca9538_io_write(i2c, mask, 0);
}