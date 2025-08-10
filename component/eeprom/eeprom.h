#ifndef _eeprom_h_
#define _eeprom_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "i2c.h"

#ifndef EEPROM_SIZE
#define EEPROM_SIZE             256
#endif

typedef struct eeprom_s{
    uint8_t address;
    uint8_t (*init)(i2cbus_t *i2c);
    uint16_t (*read)(i2cbus_t *i2c, uint16_t address, uint8_t *dst, uint16_t count);
    uint16_t (*write)(i2cbus_t *i2c, uint16_t address, const uint8_t *src, uint16_t count);
    uint16_t (*erase)(i2cbus_t *i2c);
}eeprom_t;


#ifdef __cplusplus
}
#endif

#endif /* _eeprom_h_ */