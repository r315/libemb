#ifndef _uart_h_
#define _uart_h_

#include "fifo.h"

typedef enum uartx{
    UART0 = 0,
    UART1,
    UART2,
    UART3
}uart_e;

typedef struct serialbus {
    void *ctrl;             // CMSIS compliant controller
    uint8_t  bus;   		// bus number 0,1...
    uint32_t speed;
    fifo_t rxfifo;
    fifo_t txfifo;
}serialbus_t;

void UART_Init(serialbus_t *huart);
void UART_PutChar(serialbus_t *huart, char c);
void UART_Puts(serialbus_t *huart, const char *str);
char UART_GetChar(serialbus_t *huart);
uint8_t UART_GetCharNonBlocking(serialbus_t *huart, char *c);
uint8_t UART_Kbhit(serialbus_t *huart);

#endif /* _usart_h_ */
