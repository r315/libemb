#include "stm32f1xx.h"


void BOARD_Init(void){
    // general
}


#ifdef ENABLE_UART
#include "uart.h"

void BOARD_Init_Uart(void){
    UART_Init(&huart1);
}

void BOARD_Uart_putchar(char c){
    UART_putchar(&huart1, c);
}
void BOARD_Uart_puts(const char* str){
    UART_puts(&huart1, str);
}

char BOARD_Uart_getchar(void){
    return UART_getchar(&huart1);
}

uint8_t BOARD_Uart_getCharNonBlocking(char *c){
    return UART_getCharNonBlocking(&huart1, c);
}

uint8_t BOARD_Uart_kbhit(void){
    return UART_kbhit(&huart1);
}

stdout_t uart = {
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
