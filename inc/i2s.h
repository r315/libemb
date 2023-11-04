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

// Mode bits
#define I2S_EN_TX           (1 << 0)
#define I2S_MASTER_TX       (1 << 1)
#define I2S_EN_RX           (1 << 2)
#define I2S_MASTER_RX       (1 << 3)
#define I2S_MCLK_OUT        (1 << 4)
#define I2S_FMT_PHILIPS     (0 << 5)
#define I2S_FMT_MSB         (1 << 5)
#define I2S_FMT_LSB         (2 << 5)
#define I2S_FMT_PCM         (3 << 5)


typedef enum {
    I2S_BUS0 = 0,
    I2S_BUS1,
    I2S_BUS2,
    I2S_BUS3
}i2sbusnum_t;

typedef enum {
    I2S_DT16_SL16 = 0,
    I2S_DT16_SL32,
    I2S_DT24_SL32,
    I2S_DT32_SL32,
}i2sdsize_t;

typedef void (*i2sCallback)(uint32_t *, uint32_t);

typedef struct {
    void *regs;
    void *dma;
    uint32_t sample_rate;
    i2sdsize_t data_size;
    uint8_t channels;
    uint8_t mode;
    uint8_t mute;
    i2sbusnum_t bus;
    uint32_t *txbuffer;
    uint32_t *rxbuffer;
    uint32_t tx_buf_len;    // size in samples
    uint32_t rx_buf_len;
    i2sCallback txcp;
    i2sCallback rxcp;
}i2sbus_t;


void I2S_Init(i2sbus_t *i2s);
void I2S_Config(i2sbus_t *i2s);
void I2S_Stop(i2sbus_t *i2s);
void I2S_Start(i2sbus_t *i2s);
void I2S_Mute(i2sbus_t *i2s, uint8_t mute);

#ifdef __cplusplus
}
#endif

#endif
