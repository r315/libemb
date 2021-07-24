#include "uart.h"
#include "stm32f1xx.h"

#ifdef ENABLE_UART

#define UART_DMA

#define GPIO_IN_PU   ((2 << 2)|(0 << 0))  // CNF | MODE
#define GPIO_OUT_AF  ((2 << 2)|(2 << 0))

uarthandler_t huart1, huart2, huart3;

void UART_Init(uarthandler_t *huart){
    USART_TypeDef *usart = (USART_TypeDef*)huart->usart;

    if(huart == &huart1){
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        asm("nop");
        RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
        asm("nop");
        RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

        /**
         * PA9 -> TX
         * PA10 <- RX
         * */
        GPIOA->CRH = (GPIOA->CRH & ~(0xFF << 4)) | (GPIO_IN_PU << 8) | (GPIO_OUT_AF << 4);
        GPIOA->BSRR = GPIO_PIN_10;
        usart = USART1;
    }else if(huart == &huart2){
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        asm("nop");
        RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
        asm("nop");
        RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

        /**
         * PA9 -> TX
         * PA10 <- RX
         * */
        GPIOA->CRH = (GPIOA->CRH & ~(0xFF << 4)) | (GPIO_IN_PU << 8) | (GPIO_OUT_AF << 4);
        GPIOA->BSRR = GPIO_PIN_10;
        usart = USART2;
    }else if(huart == &huart3){
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        asm("nop");
        RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
        asm("nop");
        RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

        /**
         * PA9 -> TX
         * PA10 <- RX
         * */
        GPIOA->CRH = (GPIOA->CRH & ~(0xFF << 4)) | (GPIO_IN_PU << 8) | (GPIO_OUT_AF << 4);
        GPIOA->BSRR = GPIO_PIN_10;
        usart = USART3;
    }else{
        return;
    }

    usart->BRR = 0x271;        //115200
    usart->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

#ifdef UART_DMA
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel4->CCR = 0;
    DMA1_Channel4->CPAR = (uint32_t)&USART1->DR;
    //DMA1_Channel4->CMAR = (uint32_t)tx_buf;
    DMA1_Channel4->CNDTR = 0;
#else
    usart->CR1 |= USART_CR1_RXNEIE;
    NVIC_EnableIRQ(USART1_IRQn);
#endif
}

#ifdef UART_DMA
void UART_putchar(uarthandler_t *huart, char c){
    while( (USART1->SR & USART_SR_TXE) == 0);
    USART1->DR = c;
}

void UART_puts(uarthandler_t *huart, const char* str){
    while(*str){
        UART_putchar(huart, *(uint8_t*)str++);
    }
}

char UART_getchar(uarthandler_t *huart){
    return 0;
}

uint8_t UART_getCharNonBlocking(uarthandler_t *huart, char *c){
    return 0;
}
uint8_t UART_kbhit(uarthandler_t *huart){
    return 0;
}
#else

void UART_putchar(uarthandler_t *huart, char c){
    if(fifo_put(&serial_tx, c))
    usart->CR1 |= USART_CR1_TXEIE;
}

void UART_puts(uarthandler_t *huart, const char* str){
    while(*str){
        fifo_put(&serial_tx, *(uint8_t*)str++);
    }
    usart->CR1 |= USART_CR1_TXEIE;
}  

char UART_getchar(uarthandler_t *huart){
    uint8_t c;
    fifo_get(&serial_rx, &c);
    return (char)c;
}

uint8_t UART_getCharNonBlocking(uarthandler_t *huart, char *c){
    if(fifo_avail(&serial_rx))
        return fifo_get(&serial_rx, (uint8_t*)c);
    return 0;
}

uint8_t UART_kbhit(uarthandler_t *huart){
    return fifo_avail(&serial_rx);
}

static void usart_handler(uarthandler_t *huart)
USART_TypeDef *usart = (USART_TypeDef*)huart->uart;
volatile uint32_t status = usart->SR;

    // Data received
    if (status & USART_SR_RXNE) {
        usart->SR &= ~USART_SR_RXNE;
        fifo_put(&serial_rx, (uint8_t)usart->DR);
    }
    
    // Check if data transmiter if empty 
    if (status & USART_SR_TXE) {
        usart->SR &= ~USART_SR_TXE;	          // clear interrupt
        // Check if data is available to send
        if(fifo_avail(&serial_tx) > 0){
            uint8_t data;
            fifo_get(&serial_tx, &data);
            usart->DR = data;            
        }else{
               // No more data, disable interrupt
            usart->CR1 &= ~USART_CR1_TXEIE;		      // disable TX interrupt if nothing to send
        }
    }    
}

void USART1_IRQHandler(void){
    usart_handler(huart1);
}

void USART2_IRQHandler(void){
    usart_handler(huart2);
}

void USART3_IRQHandler(void){
    usart_handler(huart3);
}

#endif

#endif
