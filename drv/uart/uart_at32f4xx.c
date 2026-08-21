#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "at32f4xx.h"
#include "gpio_at32f4xx.h"
#include "dma_at32f4xx.h"
#include "uart.h"
#include "gpio.h"
#include "fifo.h"
#include "clock.h"
#include "dma.h"

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

/**
 * @brief
 *
 * @param serialbus
 */
static void configGpios(serialbus_t *serialbus)
{
    switch(serialbus->bus){
        case UART_BUS0:
            GPIO_Config(PA_9, GPIO_USART1_TX);
            GPIO_Config(PA_10, GPIO_USART1_RX);
            break;
        case UART_BUS1:
            GPIO_Config(PA_2, GPIO_USART2_TX);
            GPIO_Config(PA_3, GPIO_USART2_RX);
            break;
        case UART_BUS2:
            GPIO_Config(PB_10, GPIO_USART3_TX);
            GPIO_Config(PB_11, GPIO_USART3_RX);
            break;
        default:
            break;
    }
}

/**
 * @brief
 * @param serialbus
 */
void UART_Init(serialbus_t *serialbus)
{
    huart_t *huart;
    uint32_t pclk;

    sysclock_t clocks;
    CLOCK_GetAll(&clocks);

#if UART_RX_MODE == UART_MODE_DMA
    uint32_t rx_req;
#endif

#if UART_TX_MODE == UART_MODE_DMA
    uint32_t tx_req;
#endif

    switch(serialbus->bus){
        case UART_BUS0:
            RCC->APB2EN |= RCC_APB2EN_USART1EN;
            RCC->APB2RST |= RCC_APB2RST_USART1RST;
            RCC->APB2RST &= ~RCC_APB2RST_USART1RST;
            pclk = clocks.clk2;
            huart = &huart1;
            huart->usart = USART1;
        #if UART_RX_MODE == UART_MODE_DMA
            rx_req = DMA1_REQ_USART1_RX;
        #endif
        #if UART_TX_MODE == UART_MODE_DMA
            tx_req = DMA1_REQ_USART1_TX;
        #endif
            break;

        case UART_BUS1:
            RCC->APB1EN |= RCC_APB1EN_USART2EN;
            RCC->APB1RST |= RCC_APB1RST_USART2RST;
            RCC->APB1RST &= ~RCC_APB1RST_USART2RST;
            pclk = clocks.clk1;
            huart = &huart2;
            huart->usart = USART2;
        #if UART_RX_MODE == UART_MODE_DMA
            rx_req = DMA1_REQ_USART2_RX;
        #endif
        #if UART_TX_MODE == UART_MODE_DMA
            tx_req = DMA1_REQ_USART2_TX;
        #endif
            break;

        case UART_BUS2:
            RCC->APB1EN |= RCC_APB1EN_USART3EN;
            RCC->APB1RST |= RCC_APB1RST_USART3RST;
            RCC->APB1RST &= ~RCC_APB1RST_USART3RST;
            pclk = clocks.clk1;
            huart = &huart3;
            huart->usart = USART3;
        #if UART_RX_MODE == UART_MODE_DMA
            rx_req = DMA1_REQ_USART3_RX;
        #endif
        #if UART_TX_MODE == UART_MODE_DMA
            tx_req = DMA1_REQ_USART3_TX;
        #endif
            break;

        default:
            return;
    }

    huart->usart->BAUDR = pclk / serialbus->speed;

    uint32_t tmp = serialbus->datalength == UART_CFG_9BIT ? USART_CTRL1_LEN : 0;

    if(serialbus->parity != UART_CFG_PARITY_NONE){
        tmp = tmp | USART_CTRL1_PCEN | ((serialbus->parity == UART_CFG_PARITY_ODD) ? USART_CTRL1_PSEL : 0);
    }

    if(serialbus->stopbit == UART_CFG_STOP_2BIT){
        huart->usart->CTRL2 |= (2 << 12);
    }

    huart->usart->CTRL1 =   tmp |
                            USART_CTRL1_REN | USART_CTRL1_TEN;


#if UART_RX_MODE == UART_MODE_DMA
    huart->rx_rd = 0;
    huart->dma_rx.dir = DMA_DIR_P2M;
    huart->dma_rx.src = (void*)&huart->usart->DT;
    huart->dma_rx.dst = (void*)huart->rx_buf;
    huart->dma_rx.ssize = DMA_DATA_SIZE_8;
    huart->dma_rx.dsize = DMA_DATA_SIZE_8;
    huart->dma_rx.len = UART_DMA_BUF_SIZE;
    huart->dma_rx.single = 0;
    huart->dma_rx.eot = NULL;
    DMA_Config(&huart->dma_rx, rx_req);
    DMA_Start(&huart->dma_rx);
    huart->usart->CTRL3 |= USART_CTRL3_DMAREN;
#else
    huart->usart->CTRL1 |= USART_CTRL1_RDNEIEN;
    fifo_init(&huart->rxfifo);
#endif

#if UART_TX_MODE == UART_MODE_DMA
    huart->dma_tx.dir = DMA_DIR_M2P;
    huart->dma_tx.src = (void*)huart->tx_buf;
    huart->dma_tx.dst = (void*)&huart->usart->DT;
    huart->dma_tx.ssize = DMA_DATA_SIZE_8;
    huart->dma_tx.dsize = DMA_DATA_SIZE_8;
    huart->dma_tx.len = 0;
    huart->dma_tx.single = 1;
    huart->dma_tx.eot = NULL;
    huart->usart->CTRL3 |= USART_CTRL3_DMATEN;
    DMA_Config(&huart->dma_tx, tx_req);
#else
    fifo_init(&huart->txfifo);
#endif

    if(serialbus->cfg_gpio){
        configGpios(serialbus);
    }

    huart->usart->CTRL1 |= USART_CTRL1_UEN;

    serialbus->handle = huart;

#if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
    NVIC_EnableIRQ(USART1_IRQn + serialbus->bus);
#endif
}

/**
 * @brief
 * @param serialbus
 * @param buf
 * @param len
 * @return
 */
uint32_t UART_Write(serialbus_t *serialbus, const uint8_t *buf, uint32_t len)
{
    huart_t *huart = (huart_t*)serialbus->handle;

#if UART_TX_MODE == UART_MODE_DMA
    while(DMA_GetTransfers(&huart->dma_tx) != huart->dma_tx.len);
    memcpy(huart->tx_buf, buf, len);
    huart->dma_tx.len = len;
    DMA_Start(&huart->dma_tx);
#else
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
#endif
    return len;
}

/**
 * @brief
 * @param serialbus
 * @param data
 * @param len
 * @return
 */
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

/**
 * @brief Read first byte on queue without removing it
 *
 * @param serialbus
 * @param data
 * @return
 */
uint32_t UART_Peek(serialbus_t *serialbus, uint8_t *data)
{
    huart_t *huart = (huart_t*)serialbus->handle;

    uint32_t available = UART_Available(serialbus);

    if(available){
    #if UART_RX_MODE == UART_MODE_DMA
        *data = huart->rx_buf[huart->rx_rd];
    #else
        *data = fifo_peek(&huart->rxfifo);
    #endif
    }

    return available;
}

/**
 * @brief
 * @param serialbus
 * @return
 */
uint32_t UART_Available(serialbus_t *serialbus){
    huart_t *huart = (huart_t*)serialbus->handle;
#if UART_RX_MODE == UART_MODE_DMA
    DMA_Channel_Type *stream = (DMA_Channel_Type *)huart->dma_rx.handle;
    uint16_t idx = huart->dma_rx.len - stream->TCNT;
    return (idx > huart->rx_rd) ? idx - huart->rx_rd : huart->rx_rd - idx;
#else
    return fifo_avail(&huart->rxfifo);
#endif
}

/**
 * @brief
 * @param huart
 */
static void UART_IRQHandler(huart_t *huart)
{
    volatile uint32_t isrflags = huart->usart->STS;
    uint32_t errorflags = isrflags & (uint32_t)(USART_STS_PERR | USART_STS_FERR | USART_STS_ORERR | USART_STS_NERR);

#if UART_RX_MODE == UART_MODE_FIFO || UART_TX_MODE == UART_MODE_FIFO
    uint32_t ctrl = huart->usart->CTRL1;
#endif

    if (errorflags){
        huart->usart->STS = ~(errorflags & (uint32_t)(USART_STS_CTSF | USART_STS_LBDF | USART_STS_TRAC | USART_STS_RDNE));
        isrflags = huart->usart->DT;
        return;
    }

#if (UART_RX_MODE == UART_MODE_FIFO)
    if (((ctrl & USART_CTRL1_REN) != 0U) && ((isrflags & USART_STS_RDNE) != 0U))	{
            fifo_put(&huart->rxfifo, (uint8_t)huart->usart->DT);
    }
#endif

#if UART_TX_MODE == UART_MODE_FIFO
    if ((ctrl & USART_CTRL1_TDEIEN) && (isrflags & USART_STS_TDE)){
        /* TX empty, send more data or finish transmission */
        if(fifo_get(&huart->txfifo, (uint8_t*)&huart->usart->DT) == 0U){
            huart->usart->CTRL1 &= ~USART_CTRL1_TDEIEN;   // Disable empty interrupt
            huart->usart->STS &= ~USART_STS_TRAC;         // Clear Completion bit since no write to DT ocurred
        }
    }
#endif
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

