#ifndef _uart_h_
#define _uart_h_

#include <stdint.h>

typedef enum uartx{
    UART_BUS0 = 0,
    UART_BUS1,
    UART_BUS2,
    UART_BUS3
}uart_e;

typedef struct serialbus {
    void *handle;           // Pointer to internal uart handle
    uint8_t  bus;   		// bus number 0,1...
    uint32_t speed;
    union{
        struct {
            uint8_t parity;
            uint8_t stopbit;
            uint8_t datalength;
        };
        uint32_t cfg;
    };
}serialbus_t;

void UART_Init(serialbus_t *huart);
uint32_t UART_Write(serialbus_t *huart, const uint8_t *data, uint32_t len);
uint32_t UART_Read(serialbus_t *huart, uint8_t *data, uint32_t len);
uint32_t UART_Available(serialbus_t *huart);

#endif /* _usart_h_ */
