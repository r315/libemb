#ifndef _pca9538_h_
#define _pca9538_h_

#include <stdint.h>
#include "i2c.h"
#include "io_expander.h"

#ifndef PCA9538_I2C_ADDRESS
#define PCA9538_I2C_ADDRESS   			0x70
#endif

#define PCA9538_IN_REG  0
#define PCA9538_OUT_REG 1
#define PCA9538_POL_REG 2
#define PCA9538_CFG_REG 3

void pca9538_init(i2cbus_t *i2c);
uint8_t pca9538_read(i2cbus_t *i2c);
void pca9538_write(i2cbus_t *i2c, uint8_t data);
void pca9538_set(i2cbus_t *i2c, uint8_t mask);
void pca9538_clr(i2cbus_t *i2c, uint8_t mask);

extern ioexp_t pca9538_ioexp;

#endif