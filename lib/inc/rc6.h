#ifndef _RC6_H_
#define _RC6_H_

#include <stdint.h>
#include "ir.h"

// https://www.sbprojects.net/knowledge/ir/rc6.php

/**
 * Mapping of PHILIPS remote
 *
 * 0        0x00
 * 1        0x01
 * 2        0x02
 * 3        0x03
 * 4        0x04
 * 5        0x05
 * 6        0x06
 * 7        0x07
 * 8        0x08
 * 9        0x09
 * BACK     0x0A
 * POWER    0x0C
 * MUTE     0x0D
 * VOl+     0x10
 * VOL-     0x11
 * FFW      0x28
 * RFW      0x2B
 * PLAY     0x2C
 * PAUSE    0x30
 * STOP     0x31
 * SOURCE   0x38
 * TEXT     0x3C
 * OPTIONS  0x40
 * CH+      0x4C
 * SUBTITLE 0x4B
 * CH-      0x4D
 * HOME     0x54
 * UP       0x58
 * DOWN     0x59
 * LEFT     0x5A
 * RIGHT    0x5B
 * OK       0X5C
 * RED      0x6D
 * GREEN    0x6E
 * YELLOW   0x6F
 * BLUE     0x70
 * ADJUST   0x90
 * FIND     0xCC
 *
 * Mark time 1T = 16*(1/carrier)
 *
 */

#define RC6_6T  2666
#define RC6_2T  889
#define RC6_1T  444

#ifdef __cplusplus
extern "C" {
#endif


uint16_t RC6_Frame(uint16_t *dst, uint8_t tr, uint8_t addr, uint8_t data);


#ifdef __cplusplus
}
#endif

#endif
