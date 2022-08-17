#ifndef _dma_lpc17xx_h_
#define _dma_lpc17xx_h_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    uint32_t src;
    uint32_t dst;
    uint32_t lli;
    uint32_t ctl;
}dmalli_t;

#define DMA_MAX_CHANNELS    8

// DMACIntStat
#define INTSTAT_CH0     (1 << 0)
#define INTSTAT_CH1     (1 << 1)
#define INTSTAT_CH2     (1 << 2)
#define INTSTAT_CH3     (1 << 3)
#define INTSTAT_CH4     (1 << 4)
#define INTSTAT_CH5     (1 << 5)
#define INTSTAT_CH6     (1 << 6)
#define INTSTAT_CH7     (1 << 7)

// DMACConfig
#define DMAC_CONFIG_E    (1 << 0)
#define DMAC_CONFIG_M    (1 << 1)

//DMACCxControl
#define DMA_CONTROL_WIDTH8  0
#define DMA_CONTROL_WIDTH16 1
#define DMA_CONTROL_WIDTH32 2
#define DMA_CONTROL_SI      (1 << 26)   // Source Increment
#define DMA_CONTROL_DI      (1 << 27)   // Destination Increment
#define DMA_CONTROL_I       (1 << 31)   // Terminal count interrupt enable
#define DMA_CONTROL_SET_SWIDTH(x) (x << 18)
#define DMA_CONTROL_SET_DWIDTH(x) (x << 21)
#define DMA_CONTROL_SET_TRANSFER(x) (x & 0xFFF)

//DMACCxConfig
#define DMA_CONFIG_E                (1 << 0)    // Channel Enable
#define DMA_CONFIG_M2M              0
#define DMA_CONFIG_M2P              1
#define DMA_CONFIG_P2M              2
#define DMA_CONFIG_P2P              3
#define DMA_CONFIG_SET_SRC_PER(x)   ((x & 15) << 1)
#define DMA_CONFIG_SET_DST_PER(x)   ((x & 15) << 6)
#define DMA_CONFIG_SET_TYPE(x)      (x << 11)
#define DMA_CONFIG_IE               (1 << 14)   // Interrupt error mask
#define DMA_CONFIG_ITC              (1 << 15)   // Terminal count interrupt mask
#define DMA_CONFIG_L                (1 << 16)   // Lock
#define DMA_CONFIG_A                (1 << 17)   // Active
#define DMA_CONFIG_H                (1 << 18)   // Halt

enum {
    DMA_REQ_PER_SSP0_TX = 0,
    DMA_REQ_PER_SSP0_RX,
    DMA_REQ_PER_SSP1_TX,
    DMA_REQ_PER_SSP1_RX,
    DMA_REQ_PER_ADC,
    DMA_REQ_PER_I2S_CH0,
    DMA_REQ_PER_I2S_CH1,
    DMA_REQ_PER_DAC,
    DMA_REQ_PER_UART0_TX,
    DMA_REQ_PER_UART0_RX,
    DMA_REQ_PER_UART1_TX,
    DMA_REQ_PER_UART1_RX,
    DMA_REQ_PER_UART2_TX,
    DMA_REQ_PER_UART2_RX,
    DMA_REQ_PER_UART3_TX,
    DMA_REQ_PER_UART3_RX,

    DMA_REQ_PER_MAT0_0 = DMA_REQ_PER_UART0_TX,
    DMA_REQ_PER_MAT0_1 = DMA_REQ_PER_UART0_RX,
    DMA_REQ_PER_MAT1_0 = DMA_REQ_PER_UART1_TX,
    DMA_REQ_PER_MAT1_1 = DMA_REQ_PER_UART1_RX,
    DMA_REQ_PER_MAT2_0 = DMA_REQ_PER_UART2_TX,
    DMA_REQ_PER_MAT2_1 = DMA_REQ_PER_UART2_RX,
    DMA_REQ_PER_MAT3_0 = DMA_REQ_PER_UART3_TX,
    DMA_REQ_PER_MAT3_1 = DMA_REQ_PER_UART3_RX,
};

#ifdef __cplusplus
}
#endif

#endif