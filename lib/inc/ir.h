#ifndef _IR_H_
#define _IR_H_

#include <stdint.h>

#define IR_MARK_ON(us)     (us | 0x8000)
#define IR_MARK_OFF(us)     (us)

#ifdef __cplusplus
extern "C" {
#endif

enum ir_mode {
    IR_MODE_DUPLEX,
    IR_MODE_TX,
    IR_MODE_RX
};

uint32_t IR_Init(enum ir_mode mode);
void IR_Pulse(uint16_t duration);
void IR_Transmit(const uint16_t *buf, uint16_t length);
void IR_Receive(uint16_t *buf, uint16_t len);
void IR_SetRxCallback(void (*cb)(uint16_t));
void IR_SetTxCallback(void (*cb)(void));
void IR_CancelReceive(void);

#ifdef __cplusplus
}
#endif

#endif