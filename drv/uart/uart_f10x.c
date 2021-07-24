#include "uart.h"
#include "stm32f1xx.h"

#ifdef ENABLE_UART


#define GPIO_IN_PU   ((2 << 2)|(0 << 0))  // CNF | MODE
#define GPIO_OUT_AF  ((2 << 2)|(2 << 0))

uarthandler_t huart1, huart2, huart3;

static void setBaudrate(uarthandler_t *huart)
{
    USART_TypeDef *usart = (USART_TypeDef*)huart->usart;
    
    float uartdiv = (float)(SystemCoreClock/huart->baudrate) / 32;

    if(usart == USART1){
        uartdiv *= 2;
    }

    uint16_t mantissa = (int)uartdiv;
    uartdiv -= mantissa;
    uint8_t frac = uartdiv * 16;

    usart->BRR = (mantissa << 4) | frac;
}

void UART_Init(uarthandler_t *huart){
    
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
        huart->usart = USART1;

#ifdef UART_DMA
        RCC->AHBENR |= RCC_AHBENR_DMA1EN;
        DMA1_Channel4->CCR = 0;
        DMA1_Channel4->CPAR = (uint32_t)&USART1->DR;
        //DMA1_Channel4->CMAR = (uint32_t)tx_buf;
        DMA1_Channel4->CNDTR = 0;
#elif UART_IT
        NVIC_EnableIRQ(USART1_IRQn);
#endif
    }else if(huart == &huart2){
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
        asm("nop");
        RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
        asm("nop");
        RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;

        /**
         * PA2 -> TX
         * PA3 <- RX
         * */
        GPIOA->CRL = (GPIOA->CRL & ~(0xFF << 8)) | (GPIO_IN_PU << 12) | (GPIO_OUT_AF << 8);
        GPIOA->BSRR = GPIO_PIN_3;
        huart->usart = USART2;
#ifdef UART_DMA
       
#elif UART_IT
        NVIC_EnableIRQ(USART2_IRQn);
#endif
    }else if(huart == &huart3){
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        asm("nop");
        RCC->APB1RSTR |= RCC_APB1RSTR_USART3RST;
        asm("nop");
        RCC->APB1RSTR &= ~RCC_APB1RSTR_USART3RST;

        /**
         * PB10 -> TX
         * PB11 <- RX
         * */
        GPIOB->CRH = (GPIOB->CRH & ~(0xFF << 8)) | (GPIO_IN_PU << 12) | (GPIO_OUT_AF << 8);
        GPIOB->BSRR = GPIO_PIN_11;
        huart->usart = USART3;
#ifdef UART_DMA
       
#elif UART_IT
        NVIC_EnableIRQ(USART3_IRQn);
#endif        
    }else{
        return;
    }

    USART_TypeDef *usart = (USART_TypeDef*)huart->usart;

    setBaudrate(huart);

    usart->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
#if UART_IT
    usart->CR1 |= USART_CR1_RXNEIE;
#endif
}

#ifdef UART_DMA
// Implement
#elif UART_IT
void UART_putchar(uarthandler_t *huart, char c){
    USART_TypeDef *usart = (USART_TypeDef*)huart->usart;

    if(fifo_put(&huart->tx, c))
    usart->CR1 |= USART_CR1_TXEIE;
}

void UART_puts(uarthandler_t *huart, const char* str){
    USART_TypeDef *usart = (USART_TypeDef*)huart->usart;

    while(*str){
        fifo_put(&huart->tx, *(uint8_t*)str++);
    }
    
    usart->CR1 |= USART_CR1_TXEIE;
}  

char UART_getchar(uarthandler_t *huart){
    uint8_t c;
    fifo_get(&huart->rx, &c);
    return (char)c;
}

uint8_t UART_getCharNonBlocking(uarthandler_t *huart, char *c){
    if(fifo_avail(&huart->rx))
        return fifo_get(&huart->rx, (uint8_t*)c);
    return 0;
}

uint8_t UART_kbhit(uarthandler_t *huart){
    return fifo_avail(&huart->rx);
}

static void usart_handler(uarthandler_t *huart)
{
    USART_TypeDef *usart = (USART_TypeDef*)huart->usart;
    volatile uint32_t status = usart->SR;

    // Data received
    if (status & USART_SR_RXNE) {
        usart->SR &= ~USART_SR_RXNE;
        fifo_put(&huart->rx, (uint8_t)usart->DR);
    }
    
    // Check if data transmiter if empty 
    if (status & USART_SR_TXE) {
        usart->SR &= ~USART_SR_TXE;	          // clear interrupt
        // Check if data is available to send
        if(fifo_avail(&huart->tx) > 0){
            uint8_t data;
            fifo_get(&huart->tx, &data);
            usart->DR = data;            
        }else{
               // No more data, disable interrupt
            usart->CR1 &= ~USART_CR1_TXEIE;		      // disable TX interrupt if nothing to send
        }
    }    
}

void USART1_IRQHandler(void){
    usart_handler(&huart1);
}

void USART2_IRQHandler(void){
    usart_handler(&huart2);
}

void USART3_IRQHandler(void){
    usart_handler(&huart3);
}
#else
void UART_putchar(uarthandler_t *huart, char c){
    USART_TypeDef *usart = (USART_TypeDef*)huart->usart;

    while( (usart->SR & USART_SR_TXE) == 0);
    usart->DR = c;
}

void UART_puts(uarthandler_t *huart, const char* str){
    while(*str){
        UART_putchar(huart, *(uint8_t*)str++);
    }
}

char UART_getchar(uarthandler_t *huart){
    USART_TypeDef *usart = (USART_TypeDef*)huart->usart;

    while( (usart->SR & USART_SR_RXNE) == 0);
    return usart->DR;    
}

uint8_t UART_getCharNonBlocking(uarthandler_t *huart, char *c){
    USART_TypeDef *usart = (USART_TypeDef*)huart->usart;

    if((usart->SR & USART_SR_RXNE) != 0){
        *c = usart->DR;
        return 1;
    }

    return 0;
}

uint8_t UART_kbhit(uarthandler_t *huart){
    USART_TypeDef *usart = (USART_TypeDef*)huart->usart;

    return (usart->SR & USART_SR_RXNE) != 0;
}
#endif


#endif
