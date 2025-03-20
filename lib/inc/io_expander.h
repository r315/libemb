#ifndef _pcf8574_h_
#define _pcf8574_h_

#include <stdint.h>
#include "i2c.h"

void IOEXP_Init(i2cbus_t *i2c);
uint8_t IOEP_Read(i2cbus_t *i2c);
void IOEXP_Write(i2cbus_t *i2c, uint8_t data);
void IOEXP_Set(i2cbus_t *i2c, uint8_t mask);
void IOEXP_Clr(i2cbus_t *i2c, uint8_t mask);

#endif