#include <stdint.h>
#include <stdio.h>
#include "lpc17xx_hal.h"
#include "uart.h"

#define DIVADDVAL_MULVAL(D, M) D | (M << 4)

static serialbus_t *s_uart[4];

typedef struct FdrPair
{
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

static void frdivLookup(volatile uint32_t *fdr, volatile uint32_t *dlm, volatile uint32_t *dll, uint32_t baudrate, uint32_t pclk)
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

	*fdr = pfrdvtab->FdrVal;
	*dlm = (DLest >> 8) & 255;
	*dll = DLest & 255;
}

void UART_Init(serialbus_t *serialbus)
{
	LPC_UART_TypeDef *puart = NULL;
	IRQn_Type irq;
	uint32_t pclk;

	switch (serialbus->bus)
	{
	case UART_BUS0:
		puart = (LPC_UART_TypeDef *)LPC_UART0;
		s_uart[0] = serialbus;
		// Turn on power to UART0
		PCONP_UART0_ENABLE;
		// Turn on UART0 peripheral clock
		CLOCK_SetPCLK(PCLK_UART0, PCLK_4);
		pclk = CLOCK_GetPCLK(PCLK_UART0);
		irq = UART0_IRQn;

		// P0.2 = TXD0, P0.3 = RXD0, Alternative function impose direction
		LPC_PINCON->PINSEL0 &= ~0xf0;
		LPC_PINCON->PINSEL0 |= (0x05 << 4);
		break;

	case UART_BUS1:
		puart = (LPC_UART_TypeDef *)LPC_UART1;
		s_uart[1] = serialbus;
		PCONP_UART1_ENABLE;
		CLOCK_SetPCLK(PCLK_UART1, PCLK_4);
		pclk = CLOCK_GetPCLK(PCLK_UART1);
		irq = UART1_IRQn;

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
		puart = (LPC_UART_TypeDef *)LPC_UART3;
		s_uart[3] = serialbus;
		PCONP_UART3_ENABLE;
		CLOCK_SetPCLK(PCLK_UART3, PCLK_4);
		pclk = CLOCK_GetPCLK(PCLK_UART3);
		irq = UART3_IRQn;

		// P0.0 = TXD3, P0.1 = RXD3
		// LPC_PINCON->PINSEL0 &= ~(0x0f << 20);
		// LPC_PINCON->PINSEL0 |= (0x0A << 20);

		// P0.25 = TXD3, P0.26 = RXD3
		// LPC_PINCON->PINSEL1 &= ~(0x0f << 18);
		LPC_PINCON->PINSEL1 |= (0x0F << 18);
		break;

	default:
		return;
	}

	puart->LCR = UART_LCR_DLAB | UART_LCR_WL8;
	frdivLookup((uint32_t *)&puart->FDR, (uint32_t *)&puart->DLM, (uint32_t *)&puart->DLL, serialbus->speed, pclk);
	puart->LCR = UART_LCR_WL8; // 8 bits, no Parity, 1 Stop bit
	puart->FCR = 0;			   // Disable FIFO

	NVIC_EnableIRQ(irq);
	serialbus->ctrl = puart;

	fifo_init(&serialbus->rxfifo);
	fifo_init(&serialbus->txfifo);

	SET_BIT(puart->IER, UART_IER_RBR | UART_IER_THRE);
}

char UART_GetChar(serialbus_t *huart)
{
	LPC_UART_TypeDef *uart = (LPC_UART_TypeDef *)huart->ctrl;
	while ((uart->LSR & UART_LSR_RDR) == 0)
		;			  // Nothing received so just block
	return uart->RBR; // Read Receiver buffer register
}

void UART_PutChar(serialbus_t *huart, char c)
{
	LPC_UART_TypeDef *uart = (LPC_UART_TypeDef *)huart->ctrl;
	
	fifo_put(&huart->txfifo, (uint8_t)c);
	
	while(!(uart->LSR & UART_LSR_TEMT));
	fifo_get(&huart->txfifo, (uint8_t *)&uart->THR);
}

void UART_Puts(serialbus_t *huart, const char *str)
{
	LPC_UART_TypeDef *uart = (LPC_UART_TypeDef *)huart->ctrl;

	while (*str)
	{
		if(fifo_put(&huart->txfifo, (uint8_t)*str++) == 0){
			break;
		}
	}
	
	while(!(uart->LSR & UART_LSR_TEMT));
	fifo_get(&huart->txfifo, (uint8_t *)&uart->THR);
}

uint8_t UART_GetCharNonBlocking(serialbus_t *huart, char *c)
{
	return fifo_get(&huart->rxfifo, (uint8_t *)c);
}

uint8_t UART_Kbhit(serialbus_t *huart)
{
	return fifo_avail(&huart->rxfifo);
}

void UART_Attach(serialbus_t *huart, void (*fptr)(void))
{
	huart->cb = fptr;
}

uint16_t UART_Write(serialbus_t *huart, uint8_t *data, uint16_t len)
{
	LPC_UART_TypeDef *uart = (LPC_UART_TypeDef *)huart->ctrl;
	uint16_t count = len;
	while (--count)
	{
		if (fifo_put(&huart->txfifo, (uint8_t)*data++) == 0){
			// Fifo full, break and return bytes pushed to fifo
			break;
		}
	}
	fifo_get(&huart->txfifo, (uint8_t *)&uart->THR);
	return len - count;
}

uint16_t UART_Read(serialbus_t *huart, uint8_t *data, uint16_t len)
{
	return 0;
}

static void UART_IRQHandler(void *ptr)
{
	serialbus_t *serialbus;
	LPC_UART_TypeDef *uart;
	uint32_t iir, lsr;

	if (ptr == NULL)
	{
		return;
	}

	serialbus = (serialbus_t *)ptr;
	uart = (LPC_UART_TypeDef *)serialbus->ctrl;
	iir = uart->IIR; // Read clears interrupt identification

	if ((iir & UART_IIR_STATUS) == 0)
	{
		// Check source
		switch ((iir >> 1) & 7)
		{

		case UART_IIR_INTID_THRE:
			fifo_get(&serialbus->txfifo, (uint8_t *)&uart->THR);
			break;

		case UART_IIR_INTID_RDA:
			fifo_put(&serialbus->rxfifo, uart->RBR);
			break;

		case UART_IIR_INTID_RLS:
			lsr = uart->LSR;
			if (lsr & (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_RXFE | UART_LSR_BI))
			{
				// errors have occurred, dummy read
				uart->SCR = uart->RBR;
			}
			else if (lsr & UART_LSR_RDR)
			{
				// Data received
				fifo_put(&serialbus->rxfifo, uart->RBR);
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
	UART_IRQHandler(s_uart[0]);
}

void UART1_IRQHandler(void){
	UART_IRQHandler(s_uart[1]);
}

void UART2_IRQHandler(void){}

void UART3_IRQHandler(void){
	UART_IRQHandler(s_uart[3]);
}