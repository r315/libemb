#ifndef _uart_h_
#define _uart_h_

#include "stdout.h"
#include "fifo.h"

typedef struct uarthandler {
    stdout_t out;
    void *usart;
    fifo_t rx;
    fifo_t tx;
    uint32_t baud;
}uarthandler_t;

extern uarthandler_t huart1, huart2, huart3;

void UART_Init(uarthandler_t *huart);
void UART_putchar(uarthandler_t *huart, char c);
void UART_puts(uarthandler_t *huart, const char* str);
char UART_getchar(uarthandler_t *huart);
uint8_t UART_getCharNonBlocking(uarthandler_t *huart, char *c);
uint8_t UART_kbhit(uarthandler_t *huart);

#endif /* _usart_h_ */
