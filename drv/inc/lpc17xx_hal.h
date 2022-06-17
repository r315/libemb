#ifndef _lpc17xx_hal_h_
#define _lpc17xx_hal_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "LPC17xx.h"
#include "clock_lpc17xx.h"
#include "pwr_lpc17xx.h"
#include "i2s_lpc17xx.h"
#include "spi_lpc17xx.h"
#include "gpio_lpc17xx.h"
#include "uart_lpc17xx.h"
#include "dma_lpc17xx.h"
#include "dac_lpc17xx.h"
#include "adc_lpc17xx.h"

#define SET_BIT(REG, BIT)       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))

#define PCONP_SPI_ENABLE      SET_BIT(LPC_SC->PCONP, SC_PCONP_PCSPI)
#define PCONP_SSP0_ENABLE     SET_BIT(LPC_SC->PCONP, SC_PCONP_PCSSP0)
#define PCONP_SSP1_ENABLE     SET_BIT(LPC_SC->PCONP, SC_PCONP_PCSSP1)
#define PCONP_UART0_ENABLE    SET_BIT(LPC_SC->PCONP, SC_PCONP_PCUART0)
#define PCONP_UART1_ENABLE    SET_BIT(LPC_SC->PCONP, SC_PCONP_PCUART1)
#define PCONP_UART2_ENABLE    SET_BIT(LPC_SC->PCONP, SC_PCONP_PCUART2)
#define PCONP_UART3_ENABLE    SET_BIT(LPC_SC->PCONP, SC_PCONP_PCUART3)
#define PCONP_I2S_ENABLE      SET_BIT(LPC_SC->PCONP, SC_PCONP_PCI2S)
#define PCONP_GPDMA_ENABLE    SET_BIT(LPC_SC->PCONP, SC_PCONP_PCGPDMA)
#define PCONP_ADC_ENABLE      SET_BIT(LPC_SC->PCONP, SC_PCONP_PCADC)

#ifdef __cplusplus
}
#endif

#endif