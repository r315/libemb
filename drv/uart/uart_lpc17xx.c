#include <stdint.h>
#include <stdio.h>
#include "lpc17xx_hal.h"
#include "uart.h"
#include "fifo.h"
#include "dma.h"
#include <string.h>

#define DIVADDVAL_MULVAL(D, M) D | (M << 4)

#define UART_MODE_BLOCKING      0
#define UART_MODE_FIFO          1
#define UART_MODE_DMA           2

#ifndef UART_RX_MODE
#define UART_RX_MODE            UART_MODE_DMA
#endif
#ifndef UART_TX_MODE
#define UART_TX_MODE            UART_MODE_DMA
#endif
#ifndef UART_DMA_BUF_SIZE
#define UART_DMA_BUF_SIZE       32
#endif


typedef struct huart {
    LPC_UART_TypeDef *uart;
    #if UART_RX_MODE == UART_MODE_DMA
    dmatype_t dma_rx;
    volatile uint16_t rx_rd;
    uint8_t rx_buf[UART_DMA_BUF_SIZE];
    #else
    fifo_t rxfifo;
    #endif

    #if UART_TX_MODE == UART_MODE_DMA
    dmatype_t dma_tx;
    uint8_t tx_buf[UART_DMA_BUF_SIZE];
    #else
    fifo_t txfifo;
    #endif

}huart_t;

static huart_t huart0, huart1, huart3;

typedef struct FdrPair{
	uint16_t fr;	 // 1.1 < FR < 1.9
	uint16_t FdrVal; //(MulVal<<4) | MulVal
} FdrPair;

// Fractional Divider setting look-up table from DS
static const FdrPair _frdivTab[] = {
	{1000, DIVADDVAL_MULVAL(0, 1)},
	{1067, DIVADDVAL_MULVAL(1, 15)},
	{1071, DIVADDVAL_MULVAL(1, 14)},
	{1077, DIVADDVAL_MULVAL(1, 13)},
	{1083, DIVADDVAL_MULVAL(1, 12)},
	{1091, DIVADDVAL_MULVAL(1, 11)},
	{1100, DIVADDVAL_MULVAL(1, 10)},
	{1111, DIVADDVAL_MULVAL(1, 9)},
	{1125, DIVADDVAL_MULVAL(1, 8)},
	{1133, DIVADDVAL_MULVAL(2, 15)},
	{1143, DIVADDVAL_MULVAL(1, 7)},
	{1154, DIVADDVAL_MULVAL(2, 13)},
	{1167, DIVADDVAL_MULVAL(1, 6)},
	{1182, DIVADDVAL_MULVAL(1, 11)},
	{1200, DIVADDVAL_MULVAL(1, 5)},
	{1214, DIVADDVAL_MULVAL(3, 14)},
	{1222, DIVADDVAL_MULVAL(2, 9)},
	{1231, DIVADDVAL_MULVAL(3, 13)},
	{1250, DIVADDVAL_MULVAL(1, 4)},
	{1267, DIVADDVAL_MULVAL(4, 15)},
	{1273, DIVADDVAL_MULVAL(3, 11)},
	{1286, DIVADDVAL_MULVAL(2, 7)},
	{1300, DIVADDVAL_MULVAL(3, 10)},
	{1308, DIVADDVAL_MULVAL(4, 13)},
	{1333, DIVADDVAL_MULVAL(1, 3)},
	{1357, DIVADDVAL_MULVAL(5, 14)},
	{1364, DIVADDVAL_MULVAL(4, 11)},
	{1375, DIVADDVAL_MULVAL(3, 8)},
	{1385, DIVADDVAL_MULVAL(5, 13)},
	{1400, DIVADDVAL_MULVAL(2, 5)},
	{1417, DIVADDVAL_MULVAL(5, 12)},
	{1429, DIVADDVAL_MULVAL(3, 7)},
	{1444, DIVADDVAL_MULVAL(4, 9)},
	{1455, DIVADDVAL_MULVAL(5, 11)},
	{1462, DIVADDVAL_MULVAL(6, 13)},
	{1467, DIVADDVAL_MULVAL(7, 15)},
	{1500, DIVADDVAL_MULVAL(1, 2)},
	{1533, DIVADDVAL_MULVAL(8, 15)},
	{1538, DIVADDVAL_MULVAL(7, 13)},
	{1545, DIVADDVAL_MULVAL(6, 11)},
	{1556, DIVADDVAL_MULVAL(5, 9)},
	{1571, DIVADDVAL_MULVAL(4, 7)},
	{1583, DIVADDVAL_MULVAL(7, 12)},
	{1600, DIVADDVAL_MULVAL(3, 5)},
	{1615, DIVADDVAL_MULVAL(8, 13)},
	{1625, DIVADDVAL_MULVAL(5, 8)},
	{1636, DIVADDVAL_MULVAL(7, 11)},
	{1643, DIVADDVAL_MULVAL(9, 14)},
	{1667, DIVADDVAL_MULVAL(2, 3)},
	{1692, DIVADDVAL_MULVAL(9, 13)},
	{1700, DIVADDVAL_MULVAL(7, 10)},
	{1714, DIVADDVAL_MULVAL(5, 7)},
	{1727, DIVADDVAL_MULVAL(8, 11)},
	{1733, DIVADDVAL_MULVAL(11, 15)},
	{1750, DIVADDVAL_MULVAL(3, 4)},
	{1769, DIVADDVAL_MULVAL(10, 13)},
	{1778, DIVADDVAL_MULVAL(7, 9)},
	{1786, DIVADDVAL_MULVAL(11, 14)},
	{1800, DIVADDVAL_MULVAL(4, 5)},
	{1818, DIVADDVAL_MULVAL(9, 11)},
	{1833, DIVADDVAL_MULVAL(5, 6)},
	{1846, DIVADDVAL_MULVAL(11, 13)},
	{1857, DIVADDVAL_MULVAL(6, 7)},
	{1867, DIVADDVAL_MULVAL(13, 15)},
	{1875, DIVADDVAL_MULVAL(7, 8)},
	{1889, DIVADDVAL_MULVAL(8, 9)},
	{1900, DIVADDVAL_MULVAL(9, 10)},
	{1909, DIVADDVAL_MULVAL(10, 11)},
	{1917, DIVADDVAL_MULVAL(11, 12)},
	{1923, DIVADDVAL_MULVAL(12, 13)},
	{1929, DIVADDVAL_MULVAL(13, 14)},
	{1933, DIVADDVAL_MULVAL(14, 15)},
};

static void frdivLookup(LPC_UART_TypeDef *uart, uint32_t baudrate, uint32_t pclk)
{
	uint32_t DLest, FRest;
	const FdrPair *pfrdvtab = _frdivTab;

	pclk = (uint64_t)pclk * 1000 / 16; // pre calculate pclk

	DLest = pclk / baudrate;

	if (DLest % 1000)
	{ // Check if DLest is integer
		uint16_t fr = 1000;
		do
		{
			fr += 100; // Pick an FR value
			DLest = pclk / (baudrate * fr);
			FRest = pclk / (baudrate * DLest);
		} while (FRest < 1100 || FRest > 1900);

		while (FRest > pfrdvtab->fr)
		{ // Search for closer FR value on look-up table
			pfrdvtab++;
		}
	}

    uart->LCR |= UART_LCR_DLAB;
	uart->FDR = pfrdvtab->FdrVal;
	uart->DLM = (DLest >> 8) & 255;
	uart->DLL = DLest & 255;
    uart->LCR &= ~UART_LCR_DLAB;
}

void UART_Init(serialbus_t *serialbus)
{
	huart_t *huart;
	uint32_t pclk;

    #if UART_RX_MODE == UART_MODE_DMA
    uint32_t rx_req;
    #endif
    #if UART_TX_MODE == UART_MODE_DMA
    uint32_t tx_req;
    #endif
    #if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
    IRQn_Type irq = 0;
    #endif


	switch (serialbus->bus)
	{
	case UART_BUS0:
		huart0.uart = LPC_UART0;
        huart = &huart0;
		// Turn on power to UART0
		PCONP_UART0_ENABLE;
		// Turn on UART0 peripheral clock
		CLOCK_SetPCLK(PCLK_UART0, PCLK_4);
		pclk = CLOCK_GetPCLK(PCLK_UART0);

        #if UART_RX_MODE == UART_MODE_DMA
        rx_req = DMA_REQ_UART0_RX;
        #endif
        #if UART_TX_MODE == UART_MODE_DMA
        tx_req = DMA_REQ_UART0_TX;
        #endif
        #if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
		irq = UART0_IRQn;
        #endif

		// P0.2 = TXD0, P0.3 = RXD0, Alternative function impose direction
		LPC_PINCON->PINSEL0 &= ~0xf0;
		LPC_PINCON->PINSEL0 |= (0x05 << 4);
		break;

	case UART_BUS1:
        huart1.uart = (LPC_UART_TypeDef*)LPC_UART1;
        huart = &huart1;
		PCONP_UART1_ENABLE;
		CLOCK_SetPCLK(PCLK_UART1, PCLK_4);
		pclk = CLOCK_GetPCLK(PCLK_UART1);

        #if UART_RX_MODE == UART_MODE_DMA
        rx_req = DMA_REQ_UART1_RX;
        #endif
        #if UART_TX_MODE == UART_MODE_DMA
        tx_req = DMA_REQ_UART1_TX;
        #endif
        #if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
		irq = UART1_IRQn;
        #endif

		// P2.0 = TXD1, P2.1 = RXD1
		LPC_PINCON->PINSEL4 &= ~(0x0f << 0);
		LPC_PINCON->PINSEL4 |= (0x0A << 0);
		break;

	case UART_BUS2:
#if 0 /* PINS are not available on BB */
			puart = (LPC_UART_TypeDef *)LPC_UART2;
			s_uart[2] = serialbus;
			PCONP_UART2_ENABLE();
			CLOCK_SetPCLK(PCLK_UART2, PCLK_4);
			pclk = CLOCK_GetPCLK(PCLK_UART2);
			irq = UART2_IRQn;
			//P0.10 = TXD2, P0.11 = RXD2
	        LPC_PINCON->PINSEL0 &= ~(0x0f << 20);
	        LPC_PINCON->PINSEL0 |= (0x05 << 20);
			break;
#else
		return;
#endif

	case UART_BUS3:
		huart3.uart = LPC_UART3;
        huart = &huart3;
		PCONP_UART3_ENABLE;
		CLOCK_SetPCLK(PCLK_UART3, PCLK_4);
		pclk = CLOCK_GetPCLK(PCLK_UART3);


        #if UART_RX_MODE == UART_MODE_DMA
        rx_req = DMA_REQ_UART3_RX;
        #endif
        #if UART_TX_MODE == UART_MODE_DMA
        tx_req = DMA_REQ_UART3_TX;
        #endif
        #if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
		irq = UART3_IRQn;
        #endif

		// P0.0 = TXD3, P0.1 = RXD3
        LPC_GPIO0->FIODIR0 = (LPC_GPIO0->FIODIR0 & 0xFC);
		LPC_PINCON->PINSEL0 &= ~(0x0f << 0);
		LPC_PINCON->PINSEL0 |= (0x0A << 0);

		// P0.25 = TXD3, P0.26 = RXD3
        //LPC_GPIO0->FIODIR3 = (LPC_GPIO3->FIODIR0 & 0xF9);
		//LPC_PINCON->PINSEL1 &= ~(0x0f << 18);
		//LPC_PINCON->PINSEL1 |= (0x0F << 18);
		break;

	default:
		return;
	}

    uint8_t wls = serialbus->datalength - 5;

    if(wls > 3){
        // Word len not supported
        return;
    }

	frdivLookup(huart->uart, serialbus->speed, pclk);

	huart->uart->LCR = wls & 3;

    if(serialbus->stopbit == UART_CFG_STOP_2BIT){
        huart->uart->LCR |= UART_LCR_SB;
    }

    if(serialbus->parity != UART_CFG_PARITY_NONE){
        uint32_t parity = serialbus->parity == UART_CFG_PARITY_EVEN ? UART_LCR_PS_EVEN : UART_LCR_PS_ODD;
        huart->uart->LCR |= UART_LCR_PE | parity;
    }

	serialbus->handle = huart;

    #if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
	NVIC_EnableIRQ(irq);
	fifo_init(&huart->rxfifo);
	fifo_init(&huart->txfifo);
	SET_BIT(huart->uart->IER, UART_IER_RBR | UART_IER_THRE);
	huart->uart->FCR = 0;			   // Disable FIFO
    #endif

    #if UART_RX_MODE == UART_MODE_DMA
    huart->dma_rx.dir = DMA_DIR_P2M;
    huart->dma_rx.dst = (void*)huart->rx_buf;
    huart->dma_rx.src = (void*)&huart->uart->RBR;
    huart->dma_rx.ssize = DMA_DATA_SIZE_8;
    huart->dma_rx.dsize = DMA_DATA_SIZE_8;
    huart->dma_rx.len = UART_DMA_BUF_SIZE;
    huart->dma_rx.single = 0;
    huart->dma_rx.eot = NULL;
    DMA_Config(&huart->dma_rx, rx_req);
    DMA_Start(&huart->dma_rx);
    huart->uart->FCR = UART_FCR_EN | UART_FCR_DMA;
    #endif

    #if UART_TX_MODE == UART_MODE_DMA
    huart->dma_tx.dir = DMA_DIR_M2P;
    huart->dma_tx.src = (void*)huart->tx_buf;
    huart->dma_tx.dst = (void*)&huart->uart->THR;
    huart->dma_tx.ssize = DMA_DATA_SIZE_8;
    huart->dma_tx.dsize = DMA_DATA_SIZE_8;
    huart->dma_tx.len = 0;
    huart->dma_tx.single = 1;
    huart->dma_tx.eot = NULL;
    DMA_Config(&huart->dma_tx, tx_req);
    huart->uart->FCR = UART_FCR_EN | UART_FCR_DMA;
    #endif

}

uint32_t UART_Available(serialbus_t *serialbus)
{
    huart_t *huart = (huart_t *)serialbus->handle;
    #if UART_RX_MODE == UART_MODE_DMA
    uint32_t dma_idx = DMA_GetTransfers(&huart->dma_rx);
    return (dma_idx < huart->rx_rd) ? huart->dma_rx.len - huart->rx_rd + dma_idx : dma_idx - huart->rx_rd;
    #else
	return fifo_avail(&huart->rxfifo);
    #endif
}

uint32_t UART_Write(serialbus_t *serialbus, const uint8_t *buf, uint32_t len){
	huart_t *huart = (huart_t *)serialbus->handle;
    #if UART_TX_MODE == UART_MODE_FIFO
    const uint8_t *end = buf + len;
	while(buf < end){
		if (fifo_put(&huart->txfifo, *buf)){
			buf++;
		}else{
            fifo_get(&huart->txfifo, (uint8_t *)&huart->uart->THR);
            while(fifo_free(&huart->txfifo) == 0);
        }
	}

	fifo_get(&huart->txfifo, (uint8_t *)&huart->uart->THR);
    #else

    if(len > UART_DMA_BUF_SIZE){
        len = UART_DMA_BUF_SIZE; // truncate data
    }

    while(DMA_GetTransfers(&huart->dma_tx) != huart->dma_tx.len);

    memcpy(huart->tx_buf, buf, len);
    huart->dma_tx.len = len;
    DMA_Start(&huart->dma_tx);
    #endif
	return len;
}

uint32_t UART_Read(serialbus_t *serialbus, uint8_t *buf, uint32_t len){
    huart_t *huart = (huart_t *)serialbus->handle;
    uint32_t count = len;
    #if UART_RX_MODE == UART_MODE_FIFO
    uint32_t count = len;
	while(count--){
        while(!fifo_get(&huart->rxfifo, buf));
        buf++;
    }
    #else
    while(count--){
        while(UART_Available(serialbus) == 0);
        *buf++ = huart->rx_buf[huart->rx_rd++];
        if(huart->rx_rd == huart->dma_rx.len){
            huart->rx_rd = 0;
        }
    }
    #endif
    return len;
}

#if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
static void UART_IRQHandler(huart_t *huart)
{
	uint32_t iir, lsr;
	iir = huart->uart->IIR; // Read clears interrupt identification

	if ((iir & UART_IIR_STATUS) == 0)
	{
		// Check source
		switch ((iir >> 1) & 7)
		{

		case UART_IIR_INTID_THRE:
			fifo_get(&huart->txfifo, (uint8_t *)&huart->uart->THR);
			break;

		case UART_IIR_INTID_RDA:
			fifo_put(&huart->rxfifo, huart->uart->RBR);
			break;

		case UART_IIR_INTID_RLS:
			lsr = huart->uart->LSR;
			if (lsr & (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_RXFE | UART_LSR_BI))
			{
				// errors have occurred, dummy read
				huart->uart->SCR = huart->uart->RBR;
			}
			else if (lsr & UART_LSR_RDR)
			{
				// Data received
				fifo_put(&huart->rxfifo, huart->uart->RBR);
			}
			break;

		case UART_IIR_INTID_CTI:
		case UART_IIR_INTID_MODEM:
		default:
			break;
		}
	}
}

void UART0_IRQHandler(void){
	UART_IRQHandler(&huart0);
}

void UART1_IRQHandler(void){
	UART_IRQHandler(&huart1);
}

void UART2_IRQHandler(void){}

void UART3_IRQHandler(void){
	UART_IRQHandler(&huart3);
}
#endif