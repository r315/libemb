#ifndef _ds1086_h_
#define _ds1086_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "i2c.h"


/**
 * @brief Basic driver to DS1086L
 * https://www.analog.com/media/en/technical-documentation/data-sheets/DS1086L.pdf
 */

#define DS1086_DEVICE_ADDR  0x58
#define DS1086_PRES         0x02
#define DS1086_DAC          0x08
#define DS1086_OFFSET       0x0E
#define DS1086_ADDR         0x0D
#define DS1086_RANGE        0x37
#define DS1086_WRITE_EE     0x3F

#define DS1086L_FREQUENCY_MIN   130000UL
#define DS1086L_FREQUENCY_MAX   66600000UL
#define DS1086L_OSCILLATOR_MIN  33300000UL
#define DS1086L_OSCILLATOR_MAX  66600000UL
#define DS1086L_PRESCALLER_MASK 0x03C0
#define DS1086L_JITTER_MASK     0xF800
#define DS1086L_OUT_CTRL_MASK   0x0400
#define DS1086_RANGE_MASK       0x1F
#define DS1086_OFFSET_MASK      0x1F
#define DS1086_DAC_MASK         0xFFC0
#define DS1086L_DAC_STEP        5000
#define DS1086_ADDR_WC          (1 << 3)

int8_t DS1086_Init(i2cbus_t*);
uint8_t DS1086_FrequencySet(uint32_t freq);
static inline uint32_t DS1086_MinFreqGet(void){return DS1086L_FREQUENCY_MIN;}
static inline uint32_t DS1086_MaxFreqGet(void){return DS1086L_FREQUENCY_MAX;}
uint8_t DS1086_PrescallerRead(uint16_t *value);
uint8_t DS1086_PrescallerWrite(uint16_t mask, uint16_t value);
uint8_t DS1086_DacRead(uint16_t *value);
uint8_t DS1086_DacWrite(uint16_t value);
uint8_t DS1086_OffsetRead(uint8_t *of);
uint8_t DS1086_OffsetWrite(uint8_t of);
uint8_t DS1086_RangeRead(uint8_t *os);
uint8_t DS1086_AddrRead(uint8_t *ad);
uint8_t DS1086_AddrWrite(uint8_t ad);
uint8_t DS1086_ReadReg(uint8_t, uint16_t*);
uint8_t DS1086_WriteReg(uint8_t, uint16_t);

#ifdef __cplusplus
}
#endif

#endif