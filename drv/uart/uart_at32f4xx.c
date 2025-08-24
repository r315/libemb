#include <stdint.h>
#include <stdio.h>
#include "at32f4xx_rcc.h"
#include "gpio_at32f4xx.h"
#include "uart.h"
#include "gpio.h"
#include "fifo.h"

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

#if UART_RX_MODE != UART_MODE_FIFO || UART_TX_MODE != UART_MODE_FIFO
#error "USART only implemented fifo mode"
#endif

typedef struct {
    USART_Type *usart;
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


void UART_Init(serialbus_t *serialbus){
    huart_t *huart;
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

            huart = &huart1;
            huart->usart = USART1;
            irq = USART1_IRQn;
            GPIO_Config(PA_9, GPIO_USART1_TX);
            GPIO_Config(PA_10, GPIO_USART1_RX);
            break;

        case UART_BUS1:
            RCC_APB1PeriphClockCmd(RCC_APB1EN_USART2EN, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_USART2RST, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_USART2RST, DISABLE);

            pclk = clocks.APB1CLK_Freq;

            huart = &huart2;
            huart->usart = USART2;
            irq = USART2_IRQn;
            GPIO_Config(PA_2, GPIO_USART2_TX);
            GPIO_Config(PA_3, GPIO_USART2_RX);
            break;

        case UART_BUS2:
            RCC_APB1PeriphClockCmd(RCC_APB1EN_USART3EN, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_USART3RST, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1RST_USART3RST, DISABLE);

            pclk = clocks.APB1CLK_Freq;

            huart = &huart3;
            huart->usart = USART3;
            irq = USART3_IRQn;
            GPIO_Config(PB_10, GPIO_USART3_TX);
            GPIO_Config(PB_11, GPIO_USART3_RX);
            break;

        default:
            return;
    }


    huart->usart->BAUDR = pclk / serialbus->speed;

    huart->usart->CTRL1 = USART_CTRL1_REN | USART_CTRL1_TEN | USART_CTRL1_UEN | USART_CTRL1_RDNEIEN;


    fifo_init(&huart->txfifo);
    fifo_init(&huart->rxfifo);

    serialbus->handle = huart;

    NVIC_EnableIRQ(irq);
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

uint32_t UART_Write(serialbus_t *serialbus, const uint8_t *buf, uint32_t len)
{
    huart_t *huart = (huart_t*)serialbus->handle;
    const uint8_t *end = buf + len;

    while(buf < end){
        if(fifo_put(&huart->txfifo, *buf)){
            buf++;
        }else{
            huart->usart->CTRL1 |= USART_CTRL1_TDEIEN;
            while(fifo_free(&huart->txfifo) == 0);
        }
    }

    huart->usart->CTRL1 |= USART_CTRL1_TDEIEN;
    return len;
}

uint32_t UART_Read(serialbus_t *serialbus, uint8_t *data, uint32_t len)
{
    huart_t *huart = (huart_t*)serialbus->handle;
    uint32_t count = len;

	while(count--){
        while(!fifo_get(&huart->rxfifo, data));
        data++;
    }
    return len;
}

static void UART_IRQHandler(huart_t *huart)
{
    uint32_t isrflags = huart->usart->STS;
    uint32_t ctrl = huart->usart->CTRL1;
    uint32_t errorflags = isrflags & (uint32_t)(USART_STS_PERR | USART_STS_FERR | USART_STS_ORERR | USART_STS_NERR);

    if (errorflags){
        huart->usart->STS = ~(errorflags & (uint32_t)(USART_STS_CTSF | USART_STS_LBDF | USART_STS_TRAC | USART_STS_RDNE));
        return;
    }

    if (((ctrl & USART_CTRL1_REN) != 0U) && ((isrflags & USART_STS_RDNE) != 0U))	{
            fifo_put(&huart->rxfifo, (uint8_t)huart->usart->DT);
    }

    if ((ctrl & USART_CTRL1_TDEIEN) && (isrflags & USART_STS_TDE)){
        /* TX empty, send more data or finish transmission */
        if(fifo_get(&huart->txfifo, (uint8_t*)&huart->usart->DT) == 0U){
            huart->usart->CTRL1 &= ~USART_CTRL1_TDEIEN;   // Disable empty interrupt
            huart->usart->STS &= ~USART_STS_TRAC;         // Clear Completion bit since no write to DT ocurred
        }
    }
}


/**
  * @brief This function handles USART2 global interrupt.
  */
void USART3_IRQHandler(void)
{
    UART_IRQHandler(&huart3);
}

void USART2_IRQHandler(void)
{
    UART_IRQHandler(&huart2);
}

void USART1_IRQHandler(void)
{
    UART_IRQHandler(&huart1);
}

