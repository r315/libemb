#ifndef _IR_H_
#define _IR_H_

#include <stdint.h>

#define IR_MARK_ON(us)     (us | 0x8000)
#define IR_MARK_OFF(us)     (us)

#ifdef __cplusplus
extern "C" {
#endif

void IR_Init(void);
void IR_Pulse(uint16_t duration);
void IR_Transmit(const uint16_t *sequence, uint16_t length);
void IR_SetEotHandler(void (*cb)(void));

#ifdef __cplusplus
}
#endif

#endif