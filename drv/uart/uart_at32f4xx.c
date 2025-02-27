#include <stdint.h>
#include <stdio.h>
#include "at32f415_crm.h"
#include "at32f415_usart.h"
#include "uart.h"

/******************  Bit definition for USART_CTRL1 register  *******************/
#define  USART_CTRL1_SBRK                       ((uint16_t)0x0001)            /*!< Send Break */
#define  USART_CTRL1_RECMUTE                    ((uint16_t)0x0002)            /*!< Receiver wakeup */
#define  USART_CTRL1_REN                        ((uint16_t)0x0004)            /*!< Receiver Enable */
#define  USART_CTRL1_TEN                        ((uint16_t)0x0008)            /*!< Transmitter Enable */
#define  USART_CTRL1_IDLEIEN                    ((uint16_t)0x0010)            /*!< IDLE Interrupt Enable */
#define  USART_CTRL1_RDNEIEN                    ((uint16_t)0x0020)            /*!< RXNE Interrupt Enable */
#define  USART_CTRL1_TRACIEN                    ((uint16_t)0x0040)            /*!< Transmission Complete Interrupt Enable */
#define  USART_CTRL1_TDEIEN                     ((uint16_t)0x0080)            /*!< Transmit data register empty Interrupt Enable */
#define  USART_CTRL1_PERRIEN                    ((uint16_t)0x0100)            /*!< PE Interrupt Enable */
#define  USART_CTRL1_PSEL                       ((uint16_t)0x0200)            /*!< Parity Selection */
#define  USART_CTRL1_PCEN                       ((uint16_t)0x0400)            /*!< Parity Control Enable */
#define  USART_CTRL1_WUMODE                     ((uint16_t)0x0800)            /*!< Wakeup method */
#define  USART_CTRL1_LEN                        ((uint16_t)0x1000)            /*!< Word length */
#define  USART_CTRL1_UEN                        ((uint16_t)0x2000)            /*!< USART Enable */
#define  USART_CTRL1_OVER8                      ((uint16_t)0x8000)            /*!< USART Oversmapling 8-bits */

/*******************  Bit definition for USART_STS register  *******************/
#define  USART_STS_PERR                         ((uint16_t)0x0001)            /*!< Parity Error */
#define  USART_STS_FERR                         ((uint16_t)0x0002)            /*!< Framing Error */
#define  USART_STS_NERR                         ((uint16_t)0x0004)            /*!< Noise Error Flag */
#define  USART_STS_ORERR                        ((uint16_t)0x0008)            /*!< OverRun Error */
#define  USART_STS_IDLEF                        ((uint16_t)0x0010)            /*!< IDLE line detected */
#define  USART_STS_RDNE                         ((uint16_t)0x0020)            /*!< Read Data Register Not Empty */
#define  USART_STS_TRAC                         ((uint16_t)0x0040)            /*!< Transmission Complete */
#define  USART_STS_TDE                          ((uint16_t)0x0080)            /*!< Transmit Data Register Empty */
#define  USART_STS_LBDF                         ((uint16_t)0x0100)            /*!< LIN Break Detection Flag */
#define  USART_STS_CTSF                         ((uint16_t)0x0200)            /*!< CTS Flag */

#ifdef UART_INT

static serialbus_t *serial1, *serial2, *serial3;

void UART_Init(serialbus_t *serialbus)
{
    usart_type *uart = NULL;
    IRQn_Type irq;
    crm_clocks_freq_type clocks;
    uint32_t pclk;

    crm_clocks_freq_get(&clocks);

    switch(serialbus->bus){
        case UART_BUS0:
            crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
            crm_periph_reset(CRM_USART1_PERIPH_RESET, TRUE);
            crm_periph_reset(CRM_USART1_PERIPH_RESET, FALSE);

            pclk = clocks.apb2_freq;

            uart = USART1;
            serial1 = serialbus;
            irq = USART1_IRQn;
            break;

        case UART_BUS1:
            crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);
            crm_periph_reset(CRM_USART2_PERIPH_RESET, TRUE);
            crm_periph_reset(CRM_USART2_PERIPH_RESET, FALSE);

            pclk = clocks.apb1_freq;

            uart = USART2;
            serial2 = serialbus;
            irq = USART2_IRQn;
            break;

        case UART_BUS2:
            crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK, TRUE);
            crm_periph_reset(CRM_USART3_PERIPH_RESET, TRUE);
            crm_periph_reset(CRM_USART3_PERIPH_RESET, FALSE);

            pclk = clocks.apb1_freq;

            uart = USART3;
            serial3 = serialbus;
            irq = USART3_IRQn;
            break;

        default:
            return;
    }


    uart->baudr = pclk / serialbus->speed;

    uart->ctrl1 = USART_CTRL1_REN | USART_CTRL1_TEN | USART_CTRL1_UEN | USART_CTRL1_RDNEIEN;


    fifo_init(&serialbus->txfifo);
    fifo_init(&serialbus->rxfifo);

    serialbus->ctrl = uart;

    NVIC_EnableIRQ(irq);
}

uint32_t UART_Available(serialbus_t *huart)
{
    return fifo_avail(&huart->rxfifo);
}

uint32_t UART_Write(serialbus_t *huart, const uint8_t *buf, uint32_t len)
{
    usart_type *uart = (usart_type*)huart->ctrl;
    const uint8_t *end = buf + len;

    while(buf < end){
        if(fifo_put(&huart->txfifo, *buf)){
            buf++;
        }else{
            uart->ctrl1 |= USART_CTRL1_TDEIEN;
            while(fifo_free(&huart->txfifo) == 0);
        }
    }

    uart->ctrl1 |= USART_CTRL1_TDEIEN;
    return len;
}

uint32_t UART_Read(serialbus_t *huart, uint8_t *data, uint32_t len)
{
    uint32_t count = len;
	while(count--){
        while(!fifo_get(&huart->rxfifo, data));
        data++;
    }
    return len;
}

void UART_IRQHandler(void *ptr){
    serialbus_t *serialbus;
    usart_type *usart;

    if(ptr == NULL){
        return;
    }
    serialbus = (serialbus_t*)ptr;
    usart = (usart_type*)serialbus->ctrl;

    uint32_t isrflags = usart->sts;
    uint32_t ctrl = usart->ctrl1;
    uint32_t errorflags = isrflags & (uint32_t)(USART_STS_PERR | USART_STS_FERR | USART_STS_ORERR | USART_STS_NERR);

    if (errorflags){
        usart->sts = ~(errorflags & (uint32_t)(USART_STS_CTSF | USART_STS_LBDF | USART_STS_TRAC | USART_STS_RDNE));
        return;
    }

    if (((ctrl & USART_CTRL1_REN) != 0U) && ((isrflags & USART_STS_RDNE) != 0U))	{
            fifo_put(&serialbus->rxfifo, (uint8_t)usart->dt);
    }

    if ((ctrl & USART_CTRL1_TDEIEN) && (isrflags & USART_STS_TDE)){
        /* TX empty, send more data or finish transmission */
        if(fifo_get(&serialbus->txfifo, (uint8_t*)&usart->dt) == 0U){
            usart->ctrl1 &= ~USART_CTRL1_TDEIEN;   // FALSE empty interrupt
            usart->sts &= ~USART_STS_TRAC;         // Clear Completion bit since no write to DT ocurred
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

#else

#define UART_BUFFER_SIZE        128

#define UART_ENABLE_TX_FIFO     0
#define UART_ENABLE_RX_DMA      1

#define USART_CTRL1_VAL         0x200C

#if UART_ENABLE_TX_FIFO
#define TX_INT                  USART_TDBE_FLAG
static uint8_t tx_buf[UART_BUFFER_SIZE];
#else
#define TX_INT                  0
#endif

#if UART_ENABLE_RX_DMA
#define RX_INT                  USART_RDBF_FLAG
#else
#define RX_INT                  0
#endif

static uint8_t rx_buf[UART_BUFFER_SIZE];
static volatile uint16_t tx_rd, tx_wr, rx_rd, rx_wr;

/**
 * API
 * */
void UART_Init(serialbus_t *serialbus)
{
    crm_clocks_freq_type clocks;
    crm_clocks_freq_get(&clocks);
    gpio_init_type gpio_init_struct;

    // Enable clocks
    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
    crm_periph_reset(CRM_USART1_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_reset(CRM_USART1_PERIPH_CLOCK, TRUE);

    USART1->ctrl1 = USART_CTRL1_VAL | RX_INT | TX_INT;

    USART1->baudr = clocks.apb2_freq / 115200;

    // Configure GPIO PA9/PA10
    gpio_init_struct.gpio_mode           = GPIO_MODE_MUX;
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_pins           = GPIO_PINS_9;
    gpio_init(GPIOA, &gpio_init_struct);
    gpio_init_struct.gpio_mode           = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull           = GPIO_PULL_UP;
    gpio_init_struct.gpio_pins           = GPIO_PINS_10;
    gpio_init(GPIOA, &gpio_init_struct);

    rx_rd = rx_wr = tx_rd = tx_wr = 0;

#if UART_ENABLE_RX_DMA
    dma_init_type dma_init_struct;

    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
    dma_reset(DMA1_CHANNEL5);
    dma_default_para_init(&dma_init_struct);

    dma_init_struct.buffer_size = UART_BUFFER_SIZE;
    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_base_addr = (uint32_t)rx_buf;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    dma_init_struct.memory_inc_enable = TRUE;
    dma_init_struct.peripheral_base_addr = (uint32_t)&USART1->dt;
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    dma_init_struct.peripheral_inc_enable = FALSE;
    dma_init_struct.priority = DMA_PRIORITY_LOW;
    dma_init_struct.loop_mode_enable = TRUE;
    dma_init(DMA1_CHANNEL5, &dma_init_struct);

    USART1->ctrl3_bit.dmaren = TRUE;
    dma_channel_enable(DMA1_CHANNEL5, TRUE);
#endif

#if !UART_ENABLE_RX_DMA || UART_ENABLE_TX_FIFO
    NVIC_SetPriority(USART1_IRQn, 10);
    NVIC_EnableIRQ(USART1_IRQn);
#endif
}

uint32_t UART_Available(serialbus_t *huart)
{
#if UART_ENABLE_RX_DMA
    uint16_t idx = UART_BUFFER_SIZE - DMA1_CHANNEL5->dtcnt;
    return (idx > rx_rd) ? idx - rx_rd : rx_rd - idx;
#else
	return (rx_wr > rx_rd) ? rx_wr - rx_rd : rx_rd - rx_wr;
#endif
}

uint32_t UART_Write(serialbus_t *huart, const uint8_t *buf, uint32_t len)
{
	usart_type *uart = USART1;
    const uint8_t *end = buf + len;

	while(buf < end){
        #if UART_ENABLE_TX_FIFO
        uint16_t size = (tx_wr > tx_rd) ? tx_wr - tx_rd : tx_rd - tx_wr;
		if(UART_BUFFER_SIZE - size > 0){
			tx_buf[tx_wr++] = *buf++;
            if(tx_wr == UART_BUFFER_SIZE){
                tx_wr = 0;
            }
		}else{
			uart->ctrl1_bit.tdbeien = 1;
			while(tx_wr == tx_rd);
		}
        #else
        while(!(uart->sts & USART_TDC_FLAG)){
        }
        uart->dt = *buf++;
        #endif
	}

    #if UART_ENABLE_TX_FIFO
	uart->ctrl1_bit.tdbeien = 1;
    #endif
    return len;
}

uint32_t UART_Read(serialbus_t *huart, uint8_t *data, uint32_t len)
{
    uint32_t count = len;

	while(count--){
        while(UART_Available(huart) == 0);
        *data++ = rx_buf[rx_rd++];
        if(rx_rd == UART_BUFFER_SIZE){
            rx_rd = 0;
        }
    }

    return len;
}

void USART1_IRQHandler(void)
{
    uint32_t isrflags = USART1->sts;
    uint32_t errorflags = isrflags & 0x000F;

    if (errorflags){
        // read DT after STS read clears error flags
        errorflags = USART1->dt;
        return;
    }

    #if !UART_ENABLE_RX_DMA
    if (isrflags & USART_RDBF_FLAG){
        if(serial_available() < UART_BUFFER_SIZE){
            rx_buf[rx_wr++] = USART1->dt;
            if(rx_wr == UART_BUFFER_SIZE){
                rx_wr = 0;
            }else{
                errorflags = USART1->dt;
            }
        }
    }
    #endif

    #if UART_ENABLE_TX_FIFO
    if (isrflags & USART_TDBE_FLAG){
        /* TX empty, send more data or finish transmission */
        if(tx_wr == tx_rd){
            USART1->sts_bit.tdc = 0;            // Clear Transmit Data Complete bit since no write to DT ocurred
            USART1->ctrl1_bit.tdbeien = 0;      // Disable Transmit Buffer Empty interrupt
        }else{
            USART1->dt = tx_buf[tx_rd++];
            if(tx_rd == UART_BUFFER_SIZE){
                tx_rd = 0;
            }
        }
    }
    #endif
}
#endif