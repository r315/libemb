/**
* @file		i2s.h
* @brief	Contains the i2s API header.
*     		
* @version	1.0
* @date		12 Dec. 2021
* @author	Hugo Reis
**********************************************************************/

#ifndef _I2S_H_
#define _I2S_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define I2S_TX_MASTER       1
#define I2S_RX_MASTER       2

typedef enum {
    I2S_BUS0 = 0,
    I2S_BUS1,
    I2S_BUS2,
    I2S_BUS3
}i2sbus_e;

typedef struct {
    void *regs;
    void *dma;
    uint32_t sample_rate;
    uint8_t data_size;
    uint8_t channels;
    uint8_t mode;
    uint8_t mute;
    i2sbus_e bus;
    volatile uint32_t *txbuffer;
    volatile uint32_t *rxbuffer;
    volatile uint32_t wridx;
    volatile uint32_t rdidx;
}i2sbus_t;

void I2S_Init(i2sbus_t *i2s);
void I2S_Config(i2sbus_t *i2s);
void I2S_DMA_IRQHandler(i2sbus_t *spidev);
void I2S_Stop(void);
void I2S_Start(void);

#ifdef __cplusplus
}
#endif

#endif