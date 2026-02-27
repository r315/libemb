#include "stm32l4xx.h"
#include "qspi.h"

#ifndef QUADSPI_CR_FTHRES_4
#define QUADSPI_CR_FTHRES_4             (3 << 8)
#endif
#define QUADSPI_CCR_FMODE_IND_WRITE     (0 << 26) /* Write in indirect mode */
#define QUADSPI_CCR_FMODE_IND_READ      (1 << 26) /* Read in indirect mode */
#define QUADSPI_CCR_FMODE_AUTO_POLL     (2 << 26) /* Automatic pollin mode */
#define QUADSPI_CCR_FMODE_MAPPED        (3 << 26) /* Memory mapped mode */
#define QSPI_DUMMY_CYCLES(n)            (n << 18)

#define QSPI_CS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define QSPI_CLK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define QSPI_D0_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_D1_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_D2_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define QSPI_D3_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()

#define QSPI_CLK_ENABLE()               __HAL_RCC_QSPI_CLK_ENABLE()
#define QSPI_CLK_DISABLE()              __HAL_RCC_QSPI_CLK_DISABLE()
#define QSPI_FORCE_RESET()              __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()            __HAL_RCC_QSPI_RELEASE_RESET()

/**
 * PINS
 */
#define ENABLE_BK1 1
#define QSPI_CLK_PIN            GPIO_PIN_3 // AF 10
#define QSPI_CLK_GPIO_PORT      GPIOA
#define QSPI_CLK_GPIO_AF        GPIO_AF10_QUADSPI

// bank 1
#define QSPI_BK1_CS_PIN         GPIO_PIN_2 // AF 10
#define QSPI_BK1_CS_GPIO_PORT   GPIOA
#define QSPI_BK1_CS_GPIO_AF     GPIO_AF10_QUADSPI

#define QSPI_BK1_D0_PIN         GPIO_PIN_1 // AF 10
#define QSPI_BK1_D0_GPIO_PORT   GPIOB
#define QSPI_BK1_D0_GPIO_AF     GPIO_AF10_QUADSPI

#define QSPI_BK1_D1_PIN         GPIO_PIN_0 // AF 10
#define QSPI_BK1_D1_GPIO_PORT   GPIOB
#define QSPI_BK1_D1_GPIO_AF     GPIO_AF10_QUADSPI

#define QSPI_BK1_D2_PIN         GPIO_PIN_7 // AF 10
#define QSPI_BK1_D2_GPIO_PORT   GPIOA
#define QSPI_BK1_D2_GPIO_AF     GPIO_AF10_QUADSPI

#define QSPI_BK1_D3_PIN         GPIO_PIN_6 // AF 10
#define QSPI_BK1_D3_GPIO_PORT   GPIOA
#define QSPI_BK1_D3_GPIO_AF     GPIO_AF10_QUADSPI

typedef struct {
    uint32_t inst   : 8;
    uint32_t imode  : 2;
    uint32_t admode : 2;
    uint32_t adsize : 2;
    uint32_t abmode : 2;
    uint32_t absize : 2;
    uint32_t dcyc   : 6;
    uint32_t dmode  : 2;
    uint32_t fmode  : 2;
    uint32_t sido   : 1;
    uint32_t res    : 1;
    uint32_t dhhc   : 1;
    uint32_t ddrm   : 1;
}ccr_t;

/**
 * @brief Wait for flag of QUADSPI peripheral
 * @param flag      QUADSPI SR flag to wait on
 * @param state     Wanted flag state
 * @param ms        Timeout in milliseconds
 * @return          QSPI_OK or QSPI_ERROR on timeout
 */
static qspires_t QSPI_WaitFlag(uint32_t flag, int state, uint32_t ms)
{
    uint32_t start_tick = HAL_GetTick();

    // Wait until flag is in expected state
    while (!!(QUADSPI->SR & flag) != state)
    {
        if (HAL_GetTick() - start_tick > ms)
        {
            return QSPI_TIMEOUT;
        }
    }

    return QSPI_OK;
}

/**
 * @brief
 * @param flag
 * @param state
 * @param ms
 * @return
 */
static qspires_t QSPI_WaitFifo(QUADSPI_TypeDef *hqspi, uint8_t level, uint32_t ms)
{
    uint32_t start_tick = HAL_GetTick();

    level <<= 8;

    while (1) {
        uint32_t status = hqspi->SR;

        if((status & 0xF00) >= level){
            return QSPI_OK;
        }

        if(status & QSPI_FLAG_TC){
            return QSPI_ERR;
        }

        if (HAL_GetTick() - start_tick > ms){
            return QSPI_TIMEOUT;
        }
    }

    return QSPI_OK;
}

/**
 * @brief
 * @param khz
 */
static void QSPI_InitSpeed(QUADSPI_TypeDef *hqspi, uint32_t khz)
{
    uint32_t prescaller;
    uint32_t clk = HAL_RCC_GetHCLKFreq() / 1000UL;

    prescaller = (clk / khz) - 1;

    if (prescaller > 255)
    {
        prescaller = 255;
    }

    /* Configure QSPI Clock Prescaler and Sample Shift */
    hqspi->CR &= ~(QUADSPI_CR_PRESCALER | QUADSPI_CR_SSHIFT);
    hqspi->CR |= ((prescaller << 24) | QSPI_SAMPLE_SHIFTING_HALFCYCLE);
}


static void QSPI_InitGpio(void)
{
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable GPIO clocks */
    QSPI_CS_GPIO_CLK_ENABLE();
    QSPI_CLK_GPIO_CLK_ENABLE();
    QSPI_D0_GPIO_CLK_ENABLE();
    QSPI_D1_GPIO_CLK_ENABLE();
    QSPI_D2_GPIO_CLK_ENABLE();
    QSPI_D3_GPIO_CLK_ENABLE();

    /* QSPI CLK GPIO pin configuration  */
    gpio_init_structure.Pin = QSPI_CLK_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = QSPI_CLK_GPIO_AF;
    HAL_GPIO_Init(QSPI_CLK_GPIO_PORT, &gpio_init_structure);

    // Bank 1
#if ENABLE_BK1
    /* QSPI CS GPIO pin configuration  */
    gpio_init_structure.Pin = QSPI_BK1_CS_PIN;
    gpio_init_structure.Alternate = QSPI_BK1_CS_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK1_CS_GPIO_PORT, &gpio_init_structure);

    /* QSPI D0 GPIO pin configuration  */
    gpio_init_structure.Pin = QSPI_BK1_D0_PIN;
    gpio_init_structure.Alternate = QSPI_BK1_D0_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK1_D0_GPIO_PORT, &gpio_init_structure);

    /* QSPI D1 GPIO pin configuration  */
    gpio_init_structure.Pin = QSPI_BK1_D1_PIN;
    gpio_init_structure.Alternate = QSPI_BK1_D1_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK1_D1_GPIO_PORT, &gpio_init_structure);

    /* QSPI D2 GPIO pin configuration  */
    gpio_init_structure.Pin = QSPI_BK1_D2_PIN;
    gpio_init_structure.Alternate = QSPI_BK1_D2_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK1_D2_GPIO_PORT, &gpio_init_structure);

    /* QSPI D3 GPIO pin configuration  */
    gpio_init_structure.Pin = QSPI_BK1_D3_PIN;
    gpio_init_structure.Alternate = QSPI_BK1_D3_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK1_D3_GPIO_PORT, &gpio_init_structure);
#endif

    // Bank 2

#if ENABLE_BK2
    /* QSPI CS GPIO pin configuration  */
    gpio_init_structure.Pin = QSPI_BK2_CS_PIN;
    gpio_init_structure.Alternate = QSPI_BK2_CS_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK2_CS_GPIO_PORT, &gpio_init_structure);

    /* QSPI D0 GPIO pin configuration  */
    gpio_init_structure.Pin = QSPI_BK2_D0_PIN;
    gpio_init_structure.Alternate = QSPI_BK2_D0_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK2_D0_GPIO_PORT, &gpio_init_structure);

    /* QSPI D1 GPIO pin configuration  */
    gpio_init_structure.Pin = QSPI_BK2_D1_PIN;
    gpio_init_structure.Alternate = QSPI_BK2_D1_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK2_D1_GPIO_PORT, &gpio_init_structure);

    /* QSPI D2 GPIO pin configuration  */
    gpio_init_structure.Pin = QSPI_BK2_D2_PIN;
    gpio_init_structure.Alternate = QSPI_BK2_D2_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK2_D2_GPIO_PORT, &gpio_init_structure);

    /* QSPI D3 GPIO pin configuration  */
    gpio_init_structure.Pin = QSPI_BK2_D3_PIN;
    gpio_init_structure.Alternate = QSPI_BK2_D3_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK2_D3_GPIO_PORT, &gpio_init_structure);
#endif
}

/**
 * @brief
 * @param
 */
static void QSPI_DeInit(void)
{
    /* De-Configure QSPI pins */
    HAL_GPIO_DeInit(QSPI_CLK_GPIO_PORT, QSPI_CLK_PIN);

    HAL_GPIO_DeInit(QSPI_BK1_CS_GPIO_PORT, QSPI_BK1_CS_PIN);
    HAL_GPIO_DeInit(QSPI_BK1_D0_GPIO_PORT, QSPI_BK1_D0_PIN);
    HAL_GPIO_DeInit(QSPI_BK1_D1_GPIO_PORT, QSPI_BK1_D1_PIN);
    HAL_GPIO_DeInit(QSPI_BK1_D2_GPIO_PORT, QSPI_BK1_D2_PIN);
    HAL_GPIO_DeInit(QSPI_BK1_D3_GPIO_PORT, QSPI_BK1_D3_PIN);
#if ENABLE_BK2
    HAL_GPIO_DeInit(QSPI_BK2_CS_GPIO_PORT, QSPI_BK2_CS_PIN);
    HAL_GPIO_DeInit(QSPI_BK2_D0_GPIO_PORT, QSPI_BK2_D0_PIN);
    HAL_GPIO_DeInit(QSPI_BK2_D1_GPIO_PORT, QSPI_BK2_D1_PIN);
    HAL_GPIO_DeInit(QSPI_BK2_D2_GPIO_PORT, QSPI_BK2_D2_PIN);
    HAL_GPIO_DeInit(QSPI_BK2_D3_GPIO_PORT, QSPI_BK2_D3_PIN);
#endif
    /* Reset the QuadSPI memory interface */
    QSPI_FORCE_RESET();
    QSPI_RELEASE_RESET();

    /* Disable the QuadSPI memory interface clock */
    QSPI_CLK_DISABLE();
}

/**
 * @brief
 * @param qspibus
 * @return
 */
qspires_t QSPI_Init(qspibus_t *qspibus)
{
    QUADSPI_TypeDef *hqspi = QUADSPI;

    QSPI_DeInit();

    /* Enable the QuadSPI memory interface clock */
    QSPI_CLK_ENABLE();

    QSPI_InitGpio();

    hqspi->CR = 0;
    /**
     * Configure QSPI FIFO Threshold to
     * set FTF flag when 4 or more bytes are available
     */
    hqspi->CR |= QUADSPI_CR_FTHRES_4;

    QSPI_InitSpeed(hqspi, qspibus->freq);

    /* Configure QSPI Flash Size, CS High Time and Clock Mode */
    hqspi->DCR = (((POSITION_VAL(qspibus->size) - 1) << 16) |
                     QSPI_CS_HIGH_TIME_4_CYCLE |
                     QSPI_CLOCK_MODE_0);

    /* Enable the QSPI peripheral */
    hqspi->CR |= QUADSPI_CR_EN;

    qspibus->handle = hqspi;

    return QSPI_OK;
}

/**
 * @brief Send command and optionally read data
 * @param qspibus
 * @param trf
 * @return
 */
static qspires_t QSPI_CommandRd(QUADSPI_TypeDef *hqspi, qspitrf_t *trf)
{
    qspires_t res;
    __IO uint8_t *fifo = (__IO uint8_t *)&hqspi->DR;

    while (trf->len)
    {
        if ((res = QSPI_WaitFifo(hqspi, 1, 1000)) != QSPI_OK){
            return res;
        }
        *trf->buffer++ = *fifo;
        trf->len--;
    }

    res = QSPI_WaitFlag(QSPI_FLAG_TC, SET, 1000);

    return res;
}

/**
 * @brief Send command and optionally write data
 * @param qspibus
 * @param trf
 * @return
 */
static qspires_t QSPI_CommandWr(QUADSPI_TypeDef *hqspi, qspitrf_t *trf)
{
    __IO uint8_t *fifo = (__IO uint8_t *)&hqspi->DR;

    // if there is data to be written, send it
    while (trf->len){
        while ((hqspi->SR & QSPI_FLAG_FT) == 0)
            ; // wait
        *fifo = *trf->buffer++;  // send byte
        trf->len--;
    }

    return QSPI_WaitFlag(QSPI_FLAG_TC, SET, 1000);
}


/**
 * @brief
 * @param qspibus
 * @param trf
 * @return
 */
qspires_t QSPI_Command(qspibus_t *qspibus, qspitrf_t *trf)
{
    ccr_t ccr;
    QUADSPI_TypeDef *hqspi = qspibus->handle;

    if (QSPI_WaitFlag(QSPI_FLAG_BUSY, RESET, 1000) != QSPI_OK){
        return QSPI_BUSY;
    }

    ccr.inst = trf->inst & 255;
    ccr.abmode = trf->abmode & 3;
    ccr.admode = trf->admode & 3;
    ccr.imode = trf->imode & 3;
    ccr.dmode = trf->dmode & 3;
    ccr.dcyc = trf->dummy & 0x3F;
    ccr.fmode = trf->op ? 1 : 0;

    hqspi->DLR = trf->len ? trf->len - 1 : 0;

    // if no data, transfer is started
    hqspi->CCR = *((uint32_t*)&ccr);

    if(ccr.admode){
        // Start transfer with address
        hqspi->AR = 0;
    }

    return (trf->op) ? QSPI_CommandRd(hqspi, trf):
                       QSPI_CommandWr(hqspi, trf);
}

/**
 * @brief
 * @param qspibus
 * @param trf
 * @return
 */
qspires_t QSPI_MapMode(qspibus_t *qspibus, qspitrf_t *trf)
{
    QUADSPI_TypeDef *hqspi = qspibus->handle;
    ccr_t ccr;

    if (QSPI_WaitFlag(QSPI_FLAG_BUSY, RESET, 1000) != QSPI_OK){
        return QSPI_BUSY;
    }

    hqspi->CR &= ~(QUADSPI_CR_EN);

    ccr.inst = trf->inst & 255;
    ccr.abmode = trf->abmode & 3;
    ccr.admode = trf->admode & 3;
    ccr.imode = trf->imode & 3;
    ccr.dmode = trf->dmode & 3;
    ccr.dcyc = trf->dummy & 0x3F;
    ccr.fmode = trf->op & 3;
    ccr.absize = trf->absize & 3;
    ccr.adsize = trf->adsize & 3;

    hqspi->ABR = trf->alternate;
    hqspi->CCR = *((uint32_t*)&ccr);

    hqspi->CR |= (QUADSPI_CR_EN);

    return QSPI_OK;
}