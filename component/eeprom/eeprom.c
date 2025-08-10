#include <string.h>
#include "eeprom.h"
#include "i2c.h"
#include "board.h"

#define EEPROM_TWR      5

#ifndef EEPROM_I2C_ADDRESS
#define EEPROM_I2C_ADDRESS      0x50
#endif

#ifndef EEPROM_PAGE_SIZE
#define EEPROM_PAGE_SIZE        8
#endif


uint8_t EEPROM_Init(i2cbus_t *i2c){
    uint8_t data;
    return I2C_Read(i2c, EEPROM_I2C_ADDRESS, &data, 1) == 1;
}

uint16_t EEPROM_Read(i2cbus_t *i2c, uint16_t address, uint8_t *dst, uint16_t count){
    if(address > 255 || !I2C_Write(i2c, EEPROM_I2C_ADDRESS, (uint8_t*)&address, 1)){
        return 0;
    }

    return I2C_Read(i2c, EEPROM_I2C_ADDRESS, dst, count);
}

uint16_t EEPROM_Write(i2cbus_t *i2c, uint16_t address, const uint8_t *src, uint16_t count){
    uint8_t buf[EEPROM_PAGE_SIZE + 1], chunk, remaning;

    if(address > 255){
        return 0;
    }

    buf[0] = address & 255;
    remaning = count;

    while(remaning){
        if(remaning > EEPROM_PAGE_SIZE){
            chunk = EEPROM_PAGE_SIZE;
        }else{
            chunk = remaning;
        }

        memcpy(buf + 1, src, chunk);
        if(!I2C_Write(i2c, EEPROM_I2C_ADDRESS, (const uint8_t*)buf, chunk + 1)){
            return count - remaning;
        }
        DelayMs(EEPROM_TWR); // device dependent delay
        remaning -= chunk;
        src += chunk;
        buf[0] += chunk;
    }

    return count;
}

uint16_t EEPROM_Erase(i2cbus_t *i2c){
    uint8_t dummy[EEPROM_PAGE_SIZE];

    memset(dummy, 0xFF, EEPROM_PAGE_SIZE);

    for(uint16_t i = 0; i < EEPROM_SIZE; i += EEPROM_PAGE_SIZE){
        if(EEPROM_Write(i2c, i, dummy, EEPROM_PAGE_SIZE) != EEPROM_PAGE_SIZE){
            return 0;
        }
    }
    return EEPROM_SIZE;
}

eeprom_t eeprom_256 = {
    .address = EEPROM_I2C_ADDRESS,
    .init = EEPROM_Init,
    .read = EEPROM_Read,
    .write = EEPROM_Write,
    .erase = EEPROM_Erase
};
