#include <stdint.h>
#include <stdio.h>
#include "stm32l4xx.h"
#include "uart.h"
#include "fifo.h"

#define GPIOA_PIN9_AF           (2 << GPIO_MODER_MODE9_Pos)
#define GPIOA_PIN10_AF          (2 << GPIO_MODER_MODE10_Pos)
#define GPIOA_PIN9_USART1_TX    (7 << GPIO_AFRH_AFSEL9_Pos)
#define GPIOA_PIN10_USART1_RX   (7 << GPIO_AFRH_AFSEL10_Pos)

#define GPIOA_PIN2_AF           (2 << GPIO_MODER_MODE2_Pos)
#define GPIOA_PIN15_AF          (2 << GPIO_MODER_MODE15_Pos)
#define GPIOA_PIN2_USART2_TX    (7 << GPIO_AFRL_AFSEL2_Pos)
#define GPIOA_PIN15_USART2_RX   (3 << GPIO_AFRH_AFSEL15_Pos)

typedef struct huart {
    USART_TypeDef *usart;
    fifo_t txfifo;
    fifo_t rxfifo;
}huart_t;

static huart_t huart1, huart2;
extern const uint8_t  APBPrescTable[]; // defined on system_stm32l4xx.c

/**
 * @brief Set the Baudrate object
 *
 * OVER8 = 0 :
 * baud = fck / USARTDIV
 * BRR = USARTDIV
 *
 * OVER8 = 1 :
 * baud = 2 * fck / USARTDIV
 * BRR[2:0] = USARTDIV[3:1]
 * BRR[3] = 0
 * BRR[15:4] = USARTDIV[15:4]
 *
 * @param huart
 */
static uint32_t setBaudrate(USART_TypeDef *usart, uint32_t fck, uint32_t speed)
{
    uint32_t usartdiv;

    if(usart->CR1 & USART_CR1_OVER8){
        usartdiv = (fck << 1) / speed;
        usartdiv = (usartdiv & 7) | ((usartdiv>>1) & 3);
    }else{
        usartdiv = fck / speed;
    }

    /* USARTDIV must be greater than or equal to 0d16 and smaller than or equal to ffff */
    if ((usartdiv >= 16) && (usartdiv <= 0xFFFF))
    {
        uint16_t brrtemp = (uint16_t)(usartdiv & 0xFFF0U);
        brrtemp |= (uint16_t)((usartdiv & (uint16_t)0x000FU) >> 1U);
        usart->BRR = brrtemp;
        return 1;
    }

    return 0;
}

/**
 * @brief
 *
 * @param serialbus
 */
void UART_Init(serialbus_t *serialbus){
	huart_t *huart;
	IRQn_Type irq;
    uint32_t tmp;

	switch(serialbus->bus){
		case UART_BUS1:
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
            RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
            asm("nop");
            RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

            // PA9  -> TX
            // PA10 <- RX
            tmp = GPIOA->MODER & ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE9);
            GPIOA->MODER = tmp | GPIOA_PIN10_AF | GPIOA_PIN9_AF;
            tmp = GPIOA->AFR[1] & ~(GPIO_AFRH_AFSEL10 | GPIO_AFRH_AFSEL9);
            GPIOA->AFR[1] = tmp | GPIOA_PIN10_USART1_RX | GPIOA_PIN9_USART1_TX;

			huart1.usart = USART1;
            huart = &huart1;
			irq = USART1_IRQn;

            tmp = (RCC->CFGR >> 11) & 7;
            tmp = SystemCoreClock / (1 << APBPrescTable[tmp]);
			break;

		case UART_BUS2:
            RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
            RCC->APB1RSTR1 |= RCC_APB1RSTR1_USART2RST;
            asm("nop");
            RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_USART2RST;

            // PA2  -> TX
            // PA15 <- RX
            tmp = GPIOA->MODER & ~(GPIO_MODER_MODE15 | GPIO_MODER_MODE2);
            GPIOA->MODER = tmp | GPIOA_PIN15_AF | GPIOA_PIN2_AF;
            tmp = GPIOA->AFR[0] & ~(GPIO_AFRL_AFSEL2);
            GPIOA->AFR[0] = tmp | GPIOA_PIN2_USART2_TX;
            tmp = GPIOA->AFR[1] & ~(GPIO_AFRH_AFSEL15);
            GPIOA->AFR[1] = tmp | GPIOA_PIN15_USART2_RX;

			huart2.usart = USART2;
            huart = &huart2;
			irq = USART2_IRQn;
            tmp = (RCC->CFGR >> 8) & 7;
            tmp = SystemCoreClock / (1 << APBPrescTable[tmp]);
			break;

		default:
			return;
	}

	serialbus->handle = huart;

    if(setBaudrate(huart->usart, tmp, serialbus->speed) == 0){
        return;
    }

	fifo_init(&huart->txfifo);
	fifo_init(&huart->rxfifo);

    huart->usart->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;

	NVIC_EnableIRQ(irq);
}

/**
 * @brief
 *
 * @param huart
 * @param buf
 * @param len
 * @return uint32_t
 */
uint32_t UART_Write(serialbus_t *serialbus, const uint8_t *buf, uint32_t len){
	huart_t *huart = (huart_t*)serialbus->handle;
    const uint8_t *end = buf + len;

	while(buf < end){
		if(fifo_put(&huart->txfifo, *buf)){
			buf++;
		}else{
			SET_BIT(huart->usart->CR1, USART_CR1_TXEIE);
			while(fifo_free(&huart->txfifo) == 0);
		}
	}

	SET_BIT(huart->usart->CR1, USART_CR1_TXEIE);
    return len;
}

/**
 * @brief
 *
 * @param huart
 * @param data
 * @param len
 * @return uint32_t
 */
uint32_t UART_Read(serialbus_t *serialbus, uint8_t *data, uint32_t len){
    huart_t *huart = (huart_t*)serialbus->handle;
    uint32_t count = len;
	while(count--){
        while(!fifo_get(&huart->rxfifo, data));
        data++;
    }
    return len;
}

/**
 * @brief
 *
 * @param huart
 * @return uint32_t
 */
uint32_t UART_Available(serialbus_t *serialbus){
    huart_t *huart = (huart_t*)serialbus->handle;
	return fifo_avail(&huart->rxfifo);
}

/**
 * @brief
 *
 * @param ptr
 */
void UART_IRQHandler(huart_t *huart){
	USART_TypeDef *usart = huart->usart;

	uint32_t isrflags = usart->ISR;
	uint32_t cr1its = usart->CR1;

	/* If no error occurs */
	uint32_t errorflags = (isrflags & (uint32_t)(USART_ISR_PE |
			USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE | USART_ISR_RTOF));

	if (errorflags != 0U){
        usart->ICR = errorflags;
        return;
    }

    if (((isrflags & USART_ISR_RXNE) != 0U)	&& ((cr1its & USART_CR1_RXNEIE) != 0U))	{
        fifo_put(&huart->rxfifo, (uint8_t)READ_REG(usart->RDR));
    }

    if (((isrflags & USART_ISR_TXE) != 0U)	&& ((cr1its & USART_CR1_TXEIE) != 0U)){
        if(fifo_get(&huart->txfifo, (uint8_t*)&usart->TDR) == 0U){
            /* No data transmitted, disable TXE interrupt */
            CLEAR_BIT(usart->CR1, USART_CR1_TXEIE);
        }
    }
}

/**
  * @brief USART1/2 global interrupt.
  */
void USART2_IRQHandler(void)
{
    UART_IRQHandler(&huart2);
}

void USART1_IRQHandler(void)
{
    UART_IRQHandler(&huart1);
}
