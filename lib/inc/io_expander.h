#ifndef _io_expander_h_
#define _io_expander_h_

#include <stdint.h>
#include "i2c.h"

typedef struct ioexp_s {
    uint8_t address;
    void (*init)(i2cbus_t *i2c);
    uint8_t (*read)(i2cbus_t *i2c);
    void (*write)(i2cbus_t *i2c, uint8_t data);
    void (*set)(i2cbus_t *i2c, uint8_t mask);
    void (*clr)(i2cbus_t *i2c, uint8_t mask);
}ioexp_t;

#endif