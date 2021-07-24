#include "stm32f1xx.h"


void BOARD_Init(void){
    // general
}


#ifdef ENABLE_UART
#include "uart.h"

#define BOARD_UART &huart3

void BOARD_Init_Uart(void){
    uarthandler_t *huart = (uarthandler_t *)BOARD_UART;
    huart->baudrate = 115200;

    UART_Init(BOARD_UART);
}

void BOARD_Uart_putchar(char c){
    UART_putchar(BOARD_UART, c);
}
void BOARD_Uart_puts(const char* str){
    UART_puts(BOARD_UART, str);
}

char BOARD_Uart_getchar(void){
    return UART_getchar(BOARD_UART);
}

uint8_t BOARD_Uart_getCharNonBlocking(char *c){
    return UART_getCharNonBlocking(BOARD_UART, c);
}

uint8_t BOARD_Uart_kbhit(void){
    return UART_kbhit(BOARD_UART);
}

StdOut uart = {
    .init = BOARD_Init_Uart,
    .xgetchar = BOARD_Uart_getchar,
    .xputchar = BOARD_Uart_putchar,
    .xputs = BOARD_Uart_puts,
    .getCharNonBlocking = BOARD_Uart_getCharNonBlocking,
    .kbhit = BOARD_Uart_kbhit,
    .user_ctx = NULL
};
#endif

#ifdef ENABLE_I2C


#endif
