#include <stdint.h>
#include <stdio.h>
#include "at32f4xx.h"
#include "gpio_at32f4xx.h"
#include "uart.h"
#include "gpio.h"

static serialbus_t *serial1, *serial2, *serial3;


void UART_Init(serialbus_t *serialbus){
    USART_Type *uart = NULL;
    IRQn_Type irq;
    RCC_ClockType clocks;
    uint32_t pclk;

    RCC_GetClocksFreq(&clocks);

    switch(serialbus->bus){
        case UART_BUS0:
            RCC_APB2PeriphClockCmd(RCC_APB2EN_USART1EN, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2EN_USART1EN, ENABLE);
            RCC_APB2PeriphResetCmd(RCC_APB2EN_USART1EN, DISABLE);

            pclk = clocks.APB2CLK_Freq;

            uart = USART1;
            serial1 = serialbus;
            irq = USART1_IRQn;

            GPIO_Config(PA_9, GPIO_USART_TX);
            GPIO_Config(PA_10, GPIO_USART_RX);
            break;

        case UART_BUS1:
            RCC_APB1PeriphClockCmd(RCC_APB1EN_USART2EN, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_USART2RST, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_USART2RST, DISABLE);

            pclk = clocks.APB1CLK_Freq;

            uart = USART2;
            serial2 = serialbus;
            irq = USART2_IRQn;

            GPIO_Config(PA_2, GPIO_USART_TX);
            GPIO_Config(PA_3, GPIO_USART_RX);  
            break;

        case UART_BUS2:
            RCC_APB1PeriphClockCmd(RCC_APB1EN_USART3EN, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_USART3RST, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_USART3RST, DISABLE);

            pclk = clocks.APB1CLK_Freq;
            
            uart = USART3;
            serial3 = serialbus;
            irq = USART3_IRQn;

            GPIO_Config(PB_10, GPIO_USART_TX);
            GPIO_Config(PB_11, GPIO_USART_RX);  
            break;

        default:
            return;
    }


    uart->BAUDR = pclk / serialbus->speed;
    
    uart->CTRL1 = USART_CTRL1_REN | USART_CTRL1_TEN | USART_CTRL1_UEN | USART_CTRL1_RDNEIEN;

    
    fifo_init(&serialbus->txfifo);
    fifo_init(&serialbus->rxfifo);

    serialbus->ctrl = uart;

    NVIC_EnableIRQ(irq);
}   

void UART_PutChar(serialbus_t *huart, char c){
    USART_Type *uart = (USART_Type*)huart->ctrl;

    while(fifo_put(&huart->txfifo, (uint8_t)c) == 0){
        uart->CTRL1 |= USART_CTRL1_TDEIEN;
        while(fifo_free(&huart->txfifo) == 0);
    }
    
    uart->CTRL1 |= USART_CTRL1_TDEIEN;
}

int UART_Puts(serialbus_t *huart, const char *str){
    int len = 0;
    USART_Type *uart = (USART_Type*)huart->ctrl;

    while(*str){
        while(fifo_put(&huart->txfifo, *(uint8_t*)str) == 0){        
            uart->CTRL1 |= USART_CTRL1_TDEIEN;
            while(fifo_free(&huart->txfifo) == 0);
        }
        str++;
        len++;
    }	
    
    UART_PutChar(huart, '\n');
    return len + 1;
}

uint16_t UART_Write(serialbus_t *huart, uint8_t *data, uint16_t len){
    USART_Type *uart = (USART_Type*)huart->ctrl;

    for(uint16_t i = 0; i < len; i++){
        while(fifo_put(&huart->txfifo, *(uint8_t*)data) == 0){        
            uart->CTRL1 |= USART_CTRL1_TDEIEN;
            while(fifo_free(&huart->txfifo) == 0);
        }
        data++;
    }	
    
    uart->CTRL1 |= USART_CTRL1_TDEIEN;
    return len;
}

uint8_t UART_GetCharNonBlocking(serialbus_t *huart, char *c){
   return fifo_get(&huart->rxfifo, (uint8_t*)c);
}

char UART_GetChar(serialbus_t *huart){
    char c;
    while(!fifo_get(&huart->rxfifo, (uint8_t*)&c));
    return c;	
}

uint8_t UART_Kbhit(serialbus_t *huart){
    return fifo_avail(&huart->rxfifo);
}

uint16_t UART_Read(serialbus_t *huart, uint8_t *data, uint16_t len){
    while(len--){
        *data++ = UART_GetChar(huart);
    }
    return 1;
}

void UART_IRQHandler(void *ptr){
    serialbus_t *serialbus;
    USART_Type *usart;

    if(ptr == NULL){
        return;
    }
    serialbus = (serialbus_t*)ptr;	
    usart = (USART_Type*)serialbus->ctrl;

    uint32_t isrflags = usart->STS;
    uint32_t ctrl = usart->CTRL1;
    uint32_t errorflags = isrflags & (uint32_t)(USART_STS_PERR | USART_STS_FERR | USART_STS_ORERR | USART_STS_NERR);

    if (errorflags){
        usart->STS = ~(errorflags & (uint32_t)(USART_STS_CTSF | USART_STS_LBDF | USART_STS_TRAC | USART_STS_RDNE));
        return;
    }

    if (((ctrl & USART_CTRL1_REN) != 0U) && ((isrflags & USART_STS_RDNE) != 0U))	{
            fifo_put(&serialbus->rxfifo, (uint8_t)usart->DT);
    }

    if ((ctrl & USART_CTRL1_TDEIEN) && (isrflags & USART_STS_TDE)){
        /* TX empty, send more data or finish transmission */
        if(fifo_get(&serialbus->txfifo, (uint8_t*)&usart->DT) == 0U){
            usart->CTRL1 &= ~USART_CTRL1_TDEIEN;   // Disable empty interrupt
            usart->STS &= ~USART_STS_TRAC;         // Clear Completion bit since no write to DT ocurred
        }
    }
}


/**
  * @brief This function handles USART2 global interrupt.
  */
void USART3_IRQHandler(void)
{
    UART_IRQHandler(serial3);
}

void USART2_IRQHandler(void)
{
    UART_IRQHandler(serial2);
}

void USART1_IRQHandler(void)
{
    UART_IRQHandler(serial1);
}

