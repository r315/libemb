#ifndef _uart_lpc17xx_h_
#define _uart_lpc17xxh_


#ifdef __cplusplus
extern "C" {
#endif
 
#include <stdint.h>

#define PCONP_UART0 (1 << 3)

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80


typedef struct _Uart{
    void *dev;
    uint32_t baudrate;
    uint8_t bits;       // 3-0: Number of bits, 4: Configure DMA

}Uart;

struct FdrPair{
	unsigned short fr;
	unsigned char FdrVal; //(MulVal<<4) | MulVal	
};


void UART_Init(Uart *uart, uint8_t number);
void UART_Send(Uart *uart, uint8_t *data, uint32_t len);
void UART_SendDMA(Uart *uart, uint8_t *data, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif
