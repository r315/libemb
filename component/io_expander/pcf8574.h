#ifndef _pcf8574_h_
#define _pcf8574_h_

#include <stdint.h>
#include "i2c.h"
#include "io_expander.h"

#ifndef PCF8574_I2C_ADDRESS
#define PCF8574_I2C_ADDRESS   			0x20
#endif

void pcf8574_init(i2cbus_t *i2c);
uint8_t pcf8574_read(i2cbus_t *i2c);
void pcf8574_write(i2cbus_t *i2c, uint8_t data);
void pcf8574_set(i2cbus_t *i2c, uint8_t mask);
void pcf8574_clr(i2cbus_t *i2c, uint8_t mask);

extern ioexp_t pcf8574_ioexp;

#endif