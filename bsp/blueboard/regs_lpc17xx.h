
#ifndef _regs_lpc17xx_h_
#define _regs_lpc17xx_h_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Register bit definitions
 * */
#define SET_BIT(REG, BIT)       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))

// PCONP
#define SC_PCONP_PCTIM0         (1 << 1)
#define SC_PCONP_PCTIM1         (1 << 2)
#define SC_PCONP_PCUART0        (1 << 3)
#define SC_PCONP_PCUART1        (1 << 4)
#define SC_PCONP_PCPWM1         (1 << 6)
#define SC_PCONP_PCI2C0         (1 << 7)
#define SC_PCONP_PCSPI          (1 << 8)
#define SC_PCONP_PCRTC          (1 << 9)
#define SC_PCONP_PCSSP1         (1 << 10)
#define SC_PCONP_PCADC          (1 << 12)
#define SC_PCONP_PCCAN1         (1 << 13)
#define SC_PCONP_PCCAN2         (1 << 14)
#define SC_PCONP_PCGPIO         (1 << 15)
#define SC_PCONP_PCRIT          (1 << 16)
#define SC_PCONP_PCMCPWM        (1 << 17)
#define SC_PCONP_PCQEI          (1 << 18)
#define SC_PCONP_PCI2C1         (1 << 19)
#define SC_PCONP_PCSSP0         (1 << 21)
#define SC_PCONP_PCTIM2         (1 << 22)
#define SC_PCONP_PCTIM3         (1 << 23)
#define SC_PCONP_PCUART2        (1 << 24)
#define SC_PCONP_PCUART3        (1 << 25)
#define SC_PCONP_PCI2C2         (1 << 26)
#define SC_PCONP_PCI2S          (1 << 27)
#define SC_PCONP_PCGPDMA        (1 << 29)
#define SC_PCONP_PCENET         (1 << 30)
#define SC_PCONP_PCUSB          (1 << 31)

#define PCONP_SPI_ENABLE()      SET_BIT(LPC_SC->PCONP, SC_PCONP_PCSPI)
#define PCONP_SSP0_ENABLE()     SET_BIT(LPC_SC->PCONP, SC_PCONP_PCSSP0)
#define PCONP_SSP1_ENABLE()     SET_BIT(LPC_SC->PCONP, SC_PCONP_PCSSP1)
#define PCONP_UART0_ENABLE()    SET_BIT(LPC_SC->PCONP, SC_PCONP_PCUART0)
#define PCONP_UART1_ENABLE()    SET_BIT(LPC_SC->PCONP, SC_PCONP_PCUART1)
#define PCONP_UART2_ENABLE()    SET_BIT(LPC_SC->PCONP, SC_PCONP_PCUART2)
#define PCONP_UART3_ENABLE()    SET_BIT(LPC_SC->PCONP, SC_PCONP_PCUART3)
#define PCONP_I2S_ENABLE()      SET_BIT(LPC_SC->PCONP, SC_PCONP_PCI2S)
#define PCONP_GPDMA_ENABLE()    SET_BIT(LPC_SC->PCONP, SC_PCONP_PCGPDMA)

// SSPx
#define SSP_CR0_CPOL        (1<<6)  // idle level
#define SSP_CR0_CPHA        (1<<7)  // data valid
#define SSP_CR1_SSE         (1<<1)  // ssp enable
#define SSP_MS              (1<<2)  // master mode
#define SSP_SOD             (1<<3)  // slave output disable
#define SSP_TFE             (1<<0)
#define SSP_TNF             (1<<1)

#define SSP_SR_TFE          (1<<0)
#define SSP_SR_TNF          (1<<1)
#define SSP_SR_RNE          (1<<2)
#define SSP_SR_RFF          (1<<3)
#define SSP_SR_BSY          (1<<4)

/* SPI */
#define SPI_SPCR_EN         (1 << 2) //transfered bits selected by bits 11:8
#define SPI_SPCR_CPHA       (1 << 3)
#define SPI_SPCR_CPOL       (1 << 4)
#define SPI_SPCR_MSTR       (1 << 5) //Master mode
#define SPI_SPCR_LSBF       (1 << 6)
#define SPI_SPCR_SPIE       (1 << 7)

#define SPI_SPSR_ABRT       (1 << 3)
#define SPI_SPSR_MODF       (1 << 4)
#define SPI_SPSR_ROVR       (1 << 5)
#define SPI_SPSR_WCOL       (1 << 6)
#define SPI_SPSR_SPIF       (1 << 7)

/* UART0/2/3 */

// LSR
#define UART_LSR_RDR        (1 << 0)
#define UART_LSR_OE         (1 << 1)
#define UART_LSR_PE         (1 << 2)
#define UART_LSR_FE         (1 << 3)
#define UART_LSR_BI         (1 << 4)
#define UART_LSR_THRE       (1 << 5)
#define UART_LSR_TEMT       (1 << 6)
#define UART_LSR_RXFE       (1 << 7)

/* UART1 */

// IER
#define UART_IER_RBR        (1 << 0)    // Receive data available
#define UART_IER_THRE       (1 << 1)    // Transmitter Holding Register Empty
#define UART_IER_RX         (1 << 2)    // RX Line
#define UART_IER_MS         (1 << 3)    // Modem Status
#define UART_IER_CTS        (1 << 7)    //
#define UART_IER_ABEO       (1 << 8)    // Auto-baud 
#define UART_IER_ABTO       (1 << 9)    // Auto-baud timeout

// IIR
#define UART_IIR_STATUS     (1 << 0)
#define UART_IIR_THRE       1
#define UART_IIR_RLS        3
#define UART_IIR_RDA        2
#define UART_IIR_CTI        6

// FCR
#define UART_FCR_EN         (1 << 0)    // 
#define UART_FCR_RX_RST     (1 << 1)    // 
#define UART_FCR_TX_RST     (1 << 2)    // 
#define UART_FCR_DMA        (1 << 3)    //
#define UART_FCR_RX_LVL0    (0 << 6)    // 
#define UART_FCR_RX_LVL1    (1 << 6)    // 
#define UART_FCR_RX_LVL2    (2 << 6)    // 
#define UART_FCR_RX_LVL3    (3 << 6)    // 

// LCR
#define UART_LCR_WL5        (0 << 0)
#define UART_LCR_WL6        (1 << 0)
#define UART_LCR_WL7        (2 << 0)
#define UART_LCR_WL8        (3 << 0)    // Word length 8bit 
#define UART_LCR_SB         (1 << 2)    // Stop bit
#define UART_LCR_PE         (1 << 3)    // Parity enable
#define UART_LCR_PS_ODD     (0 << 4)    // Odd parity
#define UART_LCR_PS_EVEN    (1 << 4)    // Even parity
#define UART_LCR_PS_SET     (2 << 4)    // Forced "1"
#define UART_LCR_PS_CLR     (3 << 4)    // Forced "0"
#define UART_LCR_BRK        (1 << 6)    // Break Control
#define UART_LCR_DLAB       (1 << 7)    // Diviso latched enable

// TER
#define UART_TER_TXEN       (1 << 7)

/** @defgroup I2S Interface
 * @{
 */
// DAO
#define I2S_DAO_WIDTH_8B            (0 << 0)
#define I2S_DAO_WIDTH_16B           (1 << 0)
#define I2S_DAO_WIDTH_32B           (3 << 0)
#define I2S_DAO_MONO                (1 << 2)
#define I2S_DAO_STOP                (1 << 3)
#define I2S_DAO_RESET               (1 << 4)
#define I2S_DAO_WS_SEL              (1 << 5)
#define I2S_DAO_MUTE                (1 << 15)
// DAI
#define I2S_DAI_WIDTH_8B            (0 << 0)
#define I2S_DAI_WIDTH_16B           (1 << 0)
#define I2S_DAI_WIDTH_32B           (3 << 0)
#define I2S_DAI_MONO                (1 << 2)
#define I2S_DAI_STOP                (1 << 3)
#define I2S_DAI_RESET               (1 << 4)
#define I2S_DAI_WS_SEL              (1 << 5)
// STATE
#define I2S_STATE_IRQ               (1 << 0)
#define I2S_STATE_DMAREQ1           (1 << 1)
#define I2S_STATE_DMAREQ2           (1 << 2)
// IRQ
#define I2S_IRQ_RX_EN               (1 << 0)
#define I2S_IRQ_TX_EN               (1 << 1)


// TXMODE/RXMODE
#define I2S_TXMODE_TXCLKSEL_FRAC    (0 << 0)
#define I2S_TXMODE_TXCLKSEL_MCLK    (2 << 0)
#define I2S_TXMODE_TX4PIN           (1 << 2)
#define I2S_TXMODE_TXMCENA          (1 << 3)
#define I2S_RXMODE_RXCLKSEL_FRAC    (0 << 0)
#define I2S_RXMODE_RXCLKSEL_MCLK    (2 << 0)
#define I2S_RXMODE_RX4PIN           (1 << 2)
#define I2S_RXMODE_RXMCENA          (1 << 3)

/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
