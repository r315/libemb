#include <stddef.h>
#include <string.h>
#include "uart.h"
#include "stm32f1xx.h"
#include "dma.h"
#include "dma_stm32f1xx.h"
#include "fifo.h"


#define GPIO_IN_PU              ((2 << 2)|(0 << 0))  // CNF | MODE
#define GPIO_OUT_AF             ((2 << 2)|(2 << 0))

#define UART_MODE_BLOCKING      0
#define UART_MODE_FIFO          1
#define UART_MODE_DMA           2

#ifndef UART_RX_MODE
#define UART_RX_MODE            UART_MODE_DMA
#endif

#ifndef UART_TX_MODE
#define UART_TX_MODE            UART_MODE_DMA
#endif

#ifndef UART_DMA_BUF_SIZE
#define UART_DMA_BUF_SIZE       128
#endif

typedef struct {
    USART_TypeDef *usart;
#if UART_RX_MODE == UART_MODE_DMA
    dmatype_t dma_rx;
    volatile uint16_t rx_rd;
    uint8_t rx_buf[UART_DMA_BUF_SIZE];
#elif UART_RX_MODE == UART_MODE_FIFO
    fifo_t rxfifo;
#endif
#if UART_TX_MODE == UART_MODE_DMA
    dmatype_t dma_tx;
    uint8_t tx_buf[UART_DMA_BUF_SIZE];
#elif UART_TX_MODE == UART_MODE_FIFO
    fifo_t txfifo;
#endif
}huart_t;


static huart_t huart1, huart2, huart3;

static void uart_set_baudrate(USART_TypeDef *usart, uint32_t speed)
{
    float uartdiv = (float)(SystemCoreClock/speed) / 32;

    if(usart == USART1){
        uartdiv *= 2;
    }

    uint16_t mantissa = (int)uartdiv;
    uartdiv -= mantissa;
    uint8_t frac = uartdiv * 16;

    usart->BRR = (mantissa << 4) | frac;
}

void UART_Init(serialbus_t *serialbus){
    huart_t *huart = NULL;

#if UART_RX_MODE == UART_MODE_DMA
    uint32_t rx_req;
#endif
#if UART_TX_MODE == UART_MODE_DMA
    uint32_t tx_req;
#endif
#if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
    IRQn_Type irq = 0;
#endif

    switch(serialbus->bus){
        case UART_BUS0:
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
            RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
            RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
        #ifdef UART_DMA
            RCC->AHBENR |= RCC_AHBENR_DMA1EN;
            DMA1_Channel4->CCR = 0;
            DMA1_Channel4->CPAR = (uint32_t)&USART1->DR;
            //DMA1_Channel4->CMAR = (uint32_t)tx_buf;
            DMA1_Channel4->CNDTR = 0;
        #endif
            /* PA9 -> TX / PA10 <- RX */
            GPIOA->CRH = (GPIOA->CRH & ~(0xFF << 4)) | (GPIO_IN_PU << 8) | (GPIO_OUT_AF << 4);
            GPIOA->BSRR = GPIO_PIN_10; // PU
            huart = &huart1;
            huart->usart = USART1;
        #if UART_RX_MODE == UART_MODE_DMA
            rx_req = DMA1_REQ_USART1_RX;
        #endif
        #if UART_TX_MODE == UART_MODE_DMA
            tx_req = DMA1_REQ_USART1_TX;
        #endif
        #if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
            irq = USART1_IRQn;
        #endif
            break;

        case UART_BUS1:
            RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
            RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
            RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
        #ifdef UART_DMA // TODO DMA for this
            RCC->AHBENR |= RCC_AHBENR_DMA1EN;
            DMA1_Channel4->CCR = 0;
            DMA1_Channel4->CPAR = (uint32_t)&USART1->DR;
            //DMA1_Channel4->CMAR = (uint32_t)tx_buf;
            DMA1_Channel4->CNDTR = 0;
        #endif
            /* PA2 -> TX / PA3 <- RX */
            GPIOA->CRL = (GPIOA->CRL & ~(0xFF << 8)) | (GPIO_IN_PU << 12) | (GPIO_OUT_AF << 8);
            GPIOA->BSRR = GPIO_PIN_3;
            huart = &huart2;
            huart->usart = USART2;
        #if UART_RX_MODE == UART_MODE_DMA
            rx_req = DMA1_REQ_USART2_RX;
        #endif
        #if UART_TX_MODE == UART_MODE_DMA
            tx_req = DMA1_REQ_USART2_TX;
        #endif
        #if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
            irq = USART2_IRQn;
        #endif
            break;

        case UART_BUS2:
            RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
            RCC->APB1RSTR |= RCC_APB1RSTR_USART3RST;
            RCC->APB1RSTR &= ~RCC_APB1RSTR_USART3RST;
        #ifdef UART_DMA // TODO DMA for this
            RCC->AHBENR |= RCC_AHBENR_DMA1EN;
            DMA1_Channel4->CCR = 0;
            DMA1_Channel4->CPAR = (uint32_t)&USART1->DR;
            //DMA1_Channel4->CMAR = (uint32_t)tx_buf;
            DMA1_Channel4->CNDTR = 0;
        #endif
            /* PB10 -> TX / PB11 <- RX */
            GPIOB->CRH = (GPIOB->CRH & ~(0xFF << 8)) | (GPIO_IN_PU << 12) | (GPIO_OUT_AF << 8);
            GPIOB->BSRR = GPIO_PIN_11;
            break;
            huart = &huart3;
            huart->usart = USART3;
        #if UART_RX_MODE == UART_MODE_DMA
            rx_req = DMA1_REQ_USART3_RX;
        #endif
        #if UART_TX_MODE == UART_MODE_DMA
            tx_req = DMA1_REQ_USART3_TX;
        #endif
        #if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
            irq = USART3_IRQn;
        #endif
            break;

        default:
            return;
    }

    serialbus->handle = huart;
    uart_set_baudrate(huart->usart, serialbus->speed);

#if UART_RX_MODE == UART_MODE_DMA
    huart->dma_rx.dir = DMA_DIR_P2M;
    huart->dma_rx.src = (void*)&huart->usart->DR;
    huart->dma_rx.dst = (void*)huart->rx_buf;
    huart->dma_rx.ssize = DMA_CCR_PSIZE_8;
    huart->dma_rx.dsize = DMA_CCR_MSIZE_8;
    huart->dma_rx.len = UART_DMA_BUF_SIZE;
    huart->dma_rx.single = 0;
    huart->dma_rx.eot = NULL;
    DMA_Config(&huart->dma_rx, rx_req);
    huart->usart->CR3 |= USART_CR3_DMAR;
    DMA_Start(&huart->dma_rx);
#elif UART_RX_MODE == UART_MODE_FIFO
    huart->usart->CR1 |= USART_CR1_RXNEIE;
    fifo_init(&huart->rxfifo);
#endif

#if UART_TX_MODE == UART_MODE_DMA
    huart->dma_tx.dir = DMA_DIR_M2P;
    huart->dma_tx.src = (void*)huart->tx_buf;
    huart->dma_tx.dst = (void*)&huart->usart->DR;
    huart->dma_tx.ssize = DMA_CCR_PSIZE_8;
    huart->dma_tx.dsize = DMA_CCR_MSIZE_8;
    huart->dma_tx.len = 0;
    huart->dma_tx.single = 1;
    huart->dma_tx.eot = NULL;
    huart->usart->CR3 |= USART_CR3_DMAT;
    DMA_Config(&huart->dma_tx, tx_req);
#elif UART_TX_MODE == UART_MODE_FIFO
    fifo_init(&huart->txfifo);
#endif

    huart->usart->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

#if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
    NVIC_EnableIRQ(irq);
#endif
}

uint32_t UART_Write(serialbus_t *serialbus, const uint8_t *buf, uint32_t len)
{
    huart_t *huart = (huart_t*)serialbus->handle;

#if UART_TX_MODE == UART_MODE_DMA
    while(DMA_GetTransfers(&huart->dma_tx) != huart->dma_tx.len);
    DMA_Cancel(&huart->dma_tx);
    memcpy(huart->tx_buf, buf, len);
    huart->dma_tx.len = len;
    DMA_Start(&huart->dma_tx);
#elif UART_TX_MODE == UART_MODE_FIFO
    const uint8_t *end = buf + len;
    USART_TypeDef *usart = huart->usart;

    while(buf < end){
        if(fifo_put(&huart->txfifo, *buf)){
            buf++;
        }else{
            usart->CR1 |= USART_CR1_TXEIE;
            while(fifo_free(&huart->txfifo) == 0);
        }
    }

    usart->CR1 |= USART_CR1_TXEIE;
#else
    const uint8_t *end = buf + len;
    USART_TypeDef *usart = huart->usart;

    while(buf < end){
        while(!(usart->SR & USART_SR_TXE));
        usart->DR = *buf++;
    }
#endif
    return len;
}

uint32_t UART_Read(serialbus_t *serialbus, uint8_t *buf, uint32_t len)
{
    huart_t *huart = (huart_t*)serialbus->handle;
    uint32_t count = len;
#if UART_RX_MODE == UART_MODE_DMA
    while(count--){
        while(UART_Available(serialbus) == 0);
        *buf++ = huart->rx_buf[huart->rx_rd++];
        if(huart->rx_rd == huart->dma_rx.len){
            huart->rx_rd = 0;
        }
    }
 #else
    while(count--){
        while(!fifo_get(&huart->rxfifo, buf));
        buf++;
    }
#endif
    return len;
}

uint32_t UART_Available(serialbus_t *serialbus){
    huart_t *huart = (huart_t*)serialbus->handle;
#if UART_RX_MODE == UART_MODE_DMA
    DMA_Channel_TypeDef *stream = (DMA_Channel_TypeDef *)huart->dma_rx.stream;

    uint16_t idx = huart->dma_rx.len - stream->CNDTR;
    return (idx > huart->rx_rd) ? idx - huart->rx_rd : huart->rx_rd - idx;
#else
    return fifo_avail(&huart->rxfifo);
#endif
}

static void UART_IRQHandler(huart_t *huart)
{
    USART_TypeDef *usart = huart->usart;

    uint32_t status = usart->SR;
    uint32_t errorflags = status & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE);

    if (errorflags){
        errorflags = usart->DR;  // software clear sequence, read status followed by DR read
    }
#if UART_RX_MODE == UART_MODE_FIFO
    // Handle received data
    if ((usart->CR1 & USART_CR1_RE) && (status & USART_SR_RXNE)){
        fifo_put(&huart->rxfifo, (uint8_t)usart->DR);
    }
#endif
#if UART_TX_MODE == UART_MODE_FIFO
    // Handle data to be transmitted
    if ((usart->CR1 & USART_CR1_TXEIE) && (status & USART_SR_TXE)){
        /* TX empty, send more data or finish transmission */
        if(fifo_get(&huart->txfifo, (uint8_t*)&usart->DR) == 0U){
            usart->CR1 &= ~USART_CR1_TXEIE;     // Disable empty interrupt
        }
    }
#endif

    usart->SR = ~(USART_SR_RXNE | USART_SR_TXE | USART_SR_CTS | USART_SR_LBD);
}

void USART1_IRQHandler(void){
    UART_IRQHandler(&huart1);
}

void USART2_IRQHandler(void){
    UART_IRQHandler(&huart2);
}

void USART3_IRQHandler(void){
    UART_IRQHandler(&huart3);
}