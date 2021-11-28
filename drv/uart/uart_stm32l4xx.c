#include <stdint.h>
#include <stdio.h>
#include "board.h"
#include "uart.h"


void UART_Init(serialbus_t *serialbus){
	USART_TypeDef *uart = NULL;
	IRQn_Type irq;

	switch(serialbus->bus){
		case UART_BUS1:
			uart = USART1;
			irq = USART1_IRQn;
			break;

		case UART_BUS2:
			uart = USART2;
			irq = USART2_IRQn;
			break;

		default:
			return;
	}

	uart->CR1 |= USART_CR1_RXNEIE;
	serialbus->ctrl = uart;
	
	HAL_NVIC_EnableIRQ(irq);
}   

void UART_PutChar(serialbus_t *huart, char c){
	USART_TypeDef *uart = (USART_TypeDef*)huart->ctrl;
    fifo_put(&huart->txfifo, (uint8_t)c);
	SET_BIT(uart->CR1, USART_CR1_TXEIE);
}

void UART_Puts(serialbus_t *huart, const char *str){
	USART_TypeDef *uart = (USART_TypeDef*)huart->ctrl;

    while(*str){
		fifo_put(&huart->txfifo, *(uint8_t*)str++);
	}
	
	SET_BIT(uart->CR1, USART_CR1_TXEIE);
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

uint16_t UART_Write(serialbus_t *huart, uint8_t *data, uint16_t len){
	return 0;
}

uint16_t UART_Read(serialbus_t *huart, uint8_t *data, uint16_t len){
	return 0;
}

void UART_IRQHandler(void *ptr){
	serialbus_t *serialbus;
	USART_TypeDef *usart;

	if(ptr == NULL){
		return;
	}
	serialbus = (serialbus_t*)ptr;	
	usart = (USART_TypeDef*)serialbus->ctrl;

	uint32_t isrflags = usart->ISR;
	uint32_t cr1its = usart->CR1;

	/* If no error occurs */
	uint32_t errorflags = (isrflags & (uint32_t)(USART_ISR_PE |
			USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE | USART_ISR_RTOF));

	if (errorflags == 0U){
		if (((isrflags & USART_ISR_RXNE) != 0U)	&& ((cr1its & USART_CR1_RXNEIE) != 0U))	{
			fifo_put(&serialbus->rxfifo, (uint8_t)READ_REG(usart->RDR));
		}

		if (((isrflags & USART_ISR_TXE) != 0U)	&& ((cr1its & USART_CR1_TXEIE) != 0U)){
			if(fifo_get(&serialbus->txfifo, (uint8_t*)&usart->TDR) == 0U){
				/* No data transmitted, disable TXE interrupt */
				CLEAR_BIT(usart->CR1, USART_CR1_TXEIE);
			}
		}
	}
}