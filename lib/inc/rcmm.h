#ifndef _RCMM_H_
#define _RCMM_H_

#include <stdint.h>
#include "ir.h"

/**
 *
 * https://www.sbprojects.net/knowledge/ir/rcmm.php
 *
 * RCMM Frame
 *
 * | Header | Mode | Data
 *
 * Mart:
 * 1T = 1/36000 * 10
 *
 *
 * Modulation:
 *
 * Mark             Space           Description
 * 417.6us (1.5T)   277.8us (1T)    Header
 * 166.7us (0.6T)   277.8us (1T)    0b00  data
 * 166.7us          444.4us (1.6T)  0b01  data
 * 166.7us          611.1us (2.2T)  0b10  data
 * 166.7us          777.8us (2.8T)  0b11  data
 *
 */

//https://www.c4forums.com/forums/topic/26685-ir-codes-for-new-device-vodafone-tv/

/**
 * Mapping of Vodafone remote
 *
 * Key      Code    Press       Release
 * 0        0x00    0x300026    0x3000A6
 * 1        0x01
 * 2        0x02
 * 3        0x03
 * 4        0x04
 * 5        0x05
 * 6        0x06
 * 7        0x07
 * 8        0x08
 * 9        0x09
 * POWER    0x0C
 * MUTE     0x0D
 * VOl+     0x10
 * VOL-     0x11
 * CHAN+    0x20
 * CHAN-    0x21
 * FORWARD  0x2A
 * REWIND   0x2B
 * PAUSE    0x30
 * STOP     0x31
 * PLAY     0x33
 * RECORD   0x37
 * SOURCE   0x38
 * INFO     0x3C
 * SKIP     0x4C
 * REPLAY   0x4D
 * MENU     0x54
 * EXIT     0x55
 * UP       0x58
 * DOWN     0x59
 * LEFT     0x5A
 * RIGHT    0x5B
 * OK       0X5C
 * RED      0x6D
 * GREEN    0x6E
 * YELLOW   0x6F
 * BLUE     0x70
 * BACK     0x83
 *
 */

#define RCMM_2_8T   777
#define RCMM_2_2T   611
#define RCMM_1_6T   444
#define RCMM_1_5T   417
#define RCMM_1T     277
#define RCMM_0_6T   166

#ifdef __cplusplus
extern "C" {
#endif


uint16_t RCMM_Frame(uint16_t *dst, uint8_t control, uint8_t addr, uint8_t code);


#ifdef __cplusplus
}
#endif

#endif
