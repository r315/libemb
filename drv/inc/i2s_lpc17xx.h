#ifndef _uart_lpc17xx_h_
#define _uart_lpc17xxh_


#ifdef __cplusplus
extern "C" {
#endif
 
#include <stdint.h>


/* I2SDAO Bits */
#define I2SDAO_WIDTH_8B     (0 << 0)
#define I2SDAO_WIDTH_16B    (1 << 0)
#define I2SDAO_WIDTH_32B    (3 << 0)
#define I2SDAO_MONO         (1 << 2)
#define I2SDAO_STOP         (1 << 3)
#define I2SDAO_RESET        (1 << 4)
#define I2SDAO_WS_SEL       (1 << 5)
#define I2SDAO_MUTE         (1 << 15)

/* I2SDAO Bits */
#define I2SDAI_WIDTH_8B     (0 << 0)
#define I2SDAI_WIDTH_16B    (1 << 0)
#define I2SDAI_WIDTH_32B    (3 << 0)
#define I2SDAI_MONO         (1 << 2)
#define I2SDAI_STOP         (1 << 3)
#define I2SDAI_RESET        (1 << 4)
#define I2SDAI_WS_SEL       (1 << 5)

/* I2SSTATE Bits */
#define I2SSTATE_IRQ        (1 << 0)
#define I2SSTATE_DMAREQ1    (1 << 1)
#define I2SSTATE_DMAREQ2    (1 << 2)
#define I2SSTATE_RX_LEVEL_pos   8
#define I2SSTATE_TX_LEVEL_pos   16

/* I2STXMODE Bits */
#define I2STXMODE_TX4PIN    (1 << 2)
#define I2STXMODE_TXMCENA   (1 << 3)

/* I2STXMODE Bits */
#define I2SRXMODE_RX4PIN    (1 << 2)
#define I2SRXMODE_RXMCENA   (1 << 3)

/* I2SIRS Bits */
#define I2SIRQ_RX_IRQ_EN    (1 << 0)
#define I2SIRQ_TX_IRQ_EN    (1 << 1)
#define I2SIRQ_RX_DEPTH_POS 8
#define I2SIRQ_TX_DEPTH_POS 16
#define I2SIRQ_RX_DEPTH_MSK (0xF << I2SIRQ_RX_DEPTH_POS)
#define I2SIRQ_TX_DEPTH_MSK (0xF << I2SIRQ_TX_DEPTH_POS)

#define I2S_RXFIFO_SIZE     8
#define I2S_TXFIFO_SIZE     8

#ifdef __cplusplus
}
#endif

#endif
