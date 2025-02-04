#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "gd32e23x_rcu.h"
#include "gd32e23x_usart.h"

#define UART_BUFFER_SIZE        FIFO_SIZE

typedef struct {
    uint32_t uart;
    uint32_t tx_dma;
    uint32_t rx_dma;
    volatile uint16_t rx_rd;
    uint8_t *txbuffer;
    uint8_t *rxbuffer;
}uartgd_t;

static uartgd_t uart0 = {USART0, DMA_CH1, DMA_CH2, 0, NULL, NULL};
static uartgd_t uart1 = {USART1, DMA_CH3, DMA_CH4, 0, NULL, NULL};

/**
 * API
 * */
void UART_Init(serialbus_t *serialbus)
{
    dma_parameter_struct dma_init_struct;
    uartgd_t *huart;

    switch(serialbus->bus){
        case UART_BUS0:
            huart = &uart0;
            //PA9/10
            rcu_periph_clock_enable(RCU_GPIOA);
            rcu_periph_clock_enable(RCU_USART0);
            gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);
            gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);
            gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
            gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);
            gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
            gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);
            break;

        case UART_BUS1:
            huart = &uart1;
            //PA2/3
            rcu_periph_clock_enable(RCU_GPIOA);
            rcu_periph_clock_enable(RCU_USART1);
            gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);
            gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3);
            gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
            gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);
            gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
            gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3);
            break;

        case UART_BUS2:
            huart = &uart0;
            //PB6/7
            rcu_periph_clock_enable(RCU_GPIOB);
            rcu_periph_clock_enable(RCU_USART0);
            gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_6);
            gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_7);
            gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
            gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_6);
            gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
            gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_7);
            break;

        case UART_BUS3:
            huart = &uart1;
            //PA8/PB0
            rcu_periph_clock_enable(RCU_GPIOA);
            rcu_periph_clock_enable(RCU_GPIOB);
            rcu_periph_clock_enable(RCU_USART1);
            gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_8);
            gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_0);
            gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
            gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_8);
            gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0);
            gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_0);
            break;

        default:
            return;
    }

    huart->txbuffer = serialbus->txfifo.buf;
    huart->rxbuffer = serialbus->rxfifo.buf;
    serialbus->ctrl = (void*)huart;

    rcu_periph_clock_enable(RCU_DMA);

    usart_deinit(huart->uart);
    usart_baudrate_set(huart->uart, serialbus->speed);
    usart_receive_config(huart->uart, USART_RECEIVE_ENABLE);
    usart_transmit_config(huart->uart, USART_TRANSMIT_ENABLE);

    usart_enable(huart->uart);

    dma_deinit(huart->tx_dma);
    dma_deinit(huart->rx_dma);

    /* RX DMA */
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)huart->rxbuffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = UART_BUFFER_SIZE;
    dma_init_struct.periph_addr = (uint32_t)&USART_RDATA(huart->uart);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(huart->rx_dma, &dma_init_struct);

    dma_circulation_enable(huart->rx_dma);
    dma_memory_to_memory_disable(huart->rx_dma);
    dma_channel_enable(huart->rx_dma);

    usart_dma_transmit_config(huart->uart, USART_DENT_ENABLE);
    usart_dma_receive_config(huart->uart, USART_DENR_ENABLE);
}

uint32_t UART_Available(serialbus_t *serialbus)
{
    uartgd_t *huart = serialbus->ctrl;
    uint16_t idx = UART_BUFFER_SIZE - DMA_CHCNT(huart->rx_dma);
    return (idx > huart->rx_rd) ? idx - huart->rx_rd : huart->rx_rd - idx;
}

uint32_t UART_Read(serialbus_t *serialbus, uint8_t *data, uint32_t len)
{
   uint32_t count = len;
   uartgd_t *huart = serialbus->ctrl;

	while(count--){
        while(UART_Available(serialbus) == 0);
        *data++ = huart->rxbuffer[huart->rx_rd++];
        if(huart->rx_rd == UART_BUFFER_SIZE){
            huart->rx_rd = 0;
        }
    }

    return len;
}

uint32_t UART_Write(serialbus_t *serialbus, const uint8_t *buf, uint32_t len)
{
    dma_parameter_struct dma_init_struct;
    uartgd_t *huart = serialbus->ctrl;

    if(len == 1){
        /* Single byte, is faster just write it*/
        usart_data_transmit(huart->uart, buf[0]);
        while(SET == usart_flag_get(huart->uart, USART_FLAG_BSY));
    }else {
        /* TX_DMA */

        if(DMA_CHCTL(huart->tx_dma) & DMA_CHXCTL_CHEN){
            while(RESET == dma_flag_get(huart->tx_dma, DMA_FLAG_FTF));
            dma_channel_disable(huart->tx_dma);
            dma_flag_clear(huart->tx_dma, DMA_FLAG_G);
        }
        /**
         * @brief Copy buffer, this avoids corruption
         * if buffer is changed while DMA occurs
         */
        memcpy(huart->txbuffer, buf, len);

        dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
        dma_init_struct.memory_addr = (uint32_t)huart->txbuffer;
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.number = len;
        dma_init_struct.periph_addr = (uint32_t)&USART_TDATA(huart->uart);
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority = DMA_PRIORITY_LOW;
        dma_init(huart->tx_dma, &dma_init_struct);
        /* configure DMA mode */
        dma_circulation_disable(huart->tx_dma);
        dma_memory_to_memory_disable(huart->tx_dma);
        dma_channel_enable(huart->tx_dma);
    }

    return len;
}