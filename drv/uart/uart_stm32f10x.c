#include "uart.h"
#include "stm32f1xx.h"

#define GPIO_IN_PU   ((2 << 2)|(0 << 0))  // CNF | MODE
#define GPIO_OUT_AF  ((2 << 2)|(2 << 0))

static serialbus_t *serial1, *serial2, *serial3;

static void setBaudrate(serialbus_t *huart)
{
    USART_TypeDef *usart = (USART_TypeDef*)huart->ctrl;
    
    float uartdiv = (float)(SystemCoreClock/huart->speed) / 32;

    if(usart == USART1){
        uartdiv *= 2;
    }

    uint16_t mantissa = (int)uartdiv;
    uartdiv -= mantissa;
    uint8_t frac = uartdiv * 16;

    usart->BRR = (mantissa << 4) | frac;
}

void UART_Init(serialbus_t *serialbus){
    USART_TypeDef *usart = NULL;
	IRQn_Type irq;
    
    switch(serialbus->bus){

    case UART_BUS1:
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        asm("nop");
        RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
        asm("nop");
        RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

#ifdef UART_DMA
        RCC->AHBENR |= RCC_AHBENR_DMA1EN;
        DMA1_Channel4->CCR = 0;
        DMA1_Channel4->CPAR = (uint32_t)&USART1->DR;
        //DMA1_Channel4->CMAR = (uint32_t)tx_buf;
        DMA1_Channel4->CNDTR = 0;        
#endif
        /**
         * PA9 -> TX
         * PA10 <- RX
         * */
        GPIOA->CRH = (GPIOA->CRH & ~(0xFF << 4)) | (GPIO_IN_PU << 8) | (GPIO_OUT_AF << 4);
        GPIOA->BSRR = GPIO_PIN_10;

        usart = USART1;
        serial1 = serialbus;
        irq = USART1_IRQn;
        break;
    
    case UART_BUS2:
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

        usart = USART2;
        serial2 = serialbus;
        irq = USART1_IRQn;
        break;

    case UART_BUS3:
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
        
        usart = USART1; // check this and comment
        serial3 = serialbus;
        irq = USART1_IRQn;
        break;
    
    default:
        return;
    }

    
    serialbus->ctrl = usart;

    setBaudrate(serialbus);

    usart->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
    usart->CR1 |= USART_CR1_RXNEIE;

    NVIC_EnableIRQ(irq);
}

uint32_t UART_Write(serialbus_t *huart, const uint8_t *buf, uint32_t len){
    USART_TypeDef *usart = (USART_TypeDef*)huart->ctrl;
    const uint8_t *end = buf + len;

    while(buf < end){
        if(fifo_put(&huart->txfifo, *buf)){
            buf++;
        }else{
            usart->CR1 |= USART_CR1_TXEIE;
            while(fifo_free(&huart->txfifo) == 0);
        }        
    }
    
    usart->CR1 |= USART_CR1_TXEIE;
    return len;
}  

uint32_t UART_Read(serialbus_t *huart, uint8_t *buf, uint32_t len){
    uint32_t count = len;
    while(count--){        
        while(!fifo_get(&huart->rxfifo, buf));
        buf++;
    }
    return len;
}

uint32_t UART_Available(serialbus_t *huart){
    return fifo_avail(&huart->rxfifo);
}

void UART_IRQHandler(void *ptr){
    serialbus_t *serialbus;
    USART_TypeDef *usart; 
    
    if(ptr == NULL){
        return;
    }
    
    serialbus = (serialbus_t*)ptr;
    usart = (USART_TypeDef*)serialbus->ctrl;
    volatile uint32_t status = usart->SR;

    // Data received
    if (status & USART_SR_RXNE) {
        usart->SR &= ~USART_SR_RXNE;
        fifo_put(&serialbus->rxfifo, (uint8_t)usart->DR);
    }
    
    // Check if data transmiter if empty 
    if (status & USART_SR_TXE) {
        usart->SR &= ~USART_SR_TXE;	          // clear interrupt
        // Check if data is available to send
        if(fifo_avail(&serialbus->txfifo) > 0){
            uint8_t data;
            fifo_get(&serialbus->txfifo, &data);
            usart->DR = data;            
        }else{
               // No more data, disable interrupt
            usart->CR1 &= ~USART_CR1_TXEIE;		      // disable TX interrupt if nothing to send
        }
    }    
}

void USART1_IRQHandler(void){
    UART_IRQHandler(serial1);
}

void USART2_IRQHandler(void){
    UART_IRQHandler(serial2);
}

void USART3_IRQHandler(void){
    UART_IRQHandler(serial3);
}