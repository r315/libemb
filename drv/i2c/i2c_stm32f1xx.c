#include <stdint.h>
#include <stddef.h>
#include "stm32f1xx.h"
#include "i2c.h"
#include "dma.h"
#include "clock.h"

#define I2C_TIMEOUT     0x10000;

typedef struct{
    I2C_TypeDef *i2c;
    dmatype_t dma_tx;
    uint32_t last_error;
    void (*eot)(void);
}hi2c_t;

static hi2c_t hi2ca = {
    .i2c = I2C1,
    .eot = NULL
}, hi2cb = {
    .i2c = I2C2,
    .eot = NULL
};

/**
 * @brief Configure i2c speed and timming
 * @param i2c       pointer to peripheral registers
 * @param speed     speed in kHz
 * @return          1: sucess, 0: otherwise
 */
static uint32_t i2c_set_speed(I2C_TypeDef *i2c, uint32_t speed, uint32_t dutycycle)
{
    uint32_t pclk = CLOCK_Get(CLOCK_PCLK1);
    uint32_t freq_range;
    uint32_t rise_time;
    uint32_t ccr;

    freq_range = pclk / 1000000UL;

    if(speed <= 100){
        if(pclk < 2000000UL){
            return I2C_ERR;
        }

        rise_time = freq_range + 1;

        ccr = (pclk - 1) / (speed * 1000UL * 2UL); // Convert to Hz and multiply by coefficient
        ccr = (ccr < 4UL) ? 4UL : ccr;
    }else{
        if(pclk < 4000000UL){
            return I2C_ERR;
        }

        rise_time = (((freq_range) * 300U) / 1000U) + 1U;

        if(!dutycycle){
            ccr = (pclk - 1) / (speed * 1000UL * 3UL);
        }else{
            ccr = (pclk - 1) / (speed * 1000UL * 25UL) | I2C_CCR_DUTY;
        }

        if(!(ccr & I2C_CCR_CCR)){
            ccr = 1;
        }else{
            ccr |= I2C_CCR_FS;
        }
    }

    /* Frequency range */
    i2c->CR2 = (i2c->CR2 & ~I2C_CR2_FREQ) | freq_range;
    /* Rise Time */
    i2c->TRISE = (i2c->TRISE & ~I2C_TRISE_TRISE) | rise_time;
    /* Speed */
    i2c->CCR = ccr;

    return I2C_OK;
}

static uint32_t i2c_flag_wait(volatile uint32_t *reg, uint32_t mask)
{
    uint32_t timeout = I2C_TIMEOUT;

    while(!(*reg & mask)){
        if(--timeout == 0){
            return I2C_ERR_TIMEOUT;
        }
    }

    return I2C_OK;
}

static uint32_t i2c_wait_start(I2C_TypeDef *i2c)
{
    return i2c_flag_wait(&i2c->SR1, I2C_SR1_SB);
}

static uint32_t i2c_wait_txe(I2C_TypeDef *i2c)
{
    return i2c_flag_wait(&i2c->SR1, I2C_SR1_TXE);
}

static uint32_t i2c_wait_btf(I2C_TypeDef *i2c)
{
    return i2c_flag_wait(&i2c->SR1, I2C_SR1_BTF);
}

static void i2c_send_start(I2C_TypeDef *i2c)
{
    i2c->CR1 |= I2C_CR1_START;
}

static void i2c_send_stop(I2C_TypeDef *i2c)
{
    i2c->CR1 |= I2C_CR1_STOP;
}

static uint32_t i2c_slave_ack(I2C_TypeDef *i2c)
{
    return !(i2c->SR1 & I2C_SR1_AF);
}

/**
 * @brief
 * @param i2c
 * @param addr  8-bit address with RW bit
 * @return
 */
static uint32_t i2c_master_send_addr(I2C_TypeDef *i2c, uint8_t addr)
{
    uint32_t timeout;

    i2c_send_start(i2c);

    if(i2c_wait_start(i2c) != I2C_OK){
        return I2C_ERR_START;
    }

    // (EV5)
    i2c->DR = addr;

    timeout = I2C_TIMEOUT;

    do{
        uint32_t sr = i2c->SR1;
        if(sr & I2C_SR1_ADDR){
            // Address sent and slave sent ACK
            // clear ADDR flag (EV6)
            i2c->SR2 = i2c->SR2;
            return I2C_OK;
        }

        if(sr & I2C_SR1_AF){
            // Slave sent NACK
            return I2C_ERR_ACK;
        }
    }while(--timeout > 0);

    // Failed to sent Slave adddress
    // SCL/SDA lines stuck?
    return I2C_ERR_ADDR;
}

uint32_t I2C_Init(i2cbus_t *i2cbus)
{
    hi2c_t *hi2c;

    if(!i2cbus){
        return I2C_ERR_PARM;
    }

    switch(i2cbus->bus_num){
        case I2C_BUS0:
            RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
            hi2c = &hi2ca;
            break;

        case I2C_BUS1:
            RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
            hi2c = &hi2cb;
            break;

        default:
            return I2C_ERR_PARM;
    }

    i2cbus->handle = hi2c;

    I2C_Reset(i2cbus);

    hi2c->i2c->CR1 = I2C_CR1_PE;

    return i2c_set_speed(hi2c->i2c, i2cbus->speed, 0);
}

/**
 * @brief Write data to I2C bus
 * @param i2cbus    i2cbus handle
 * @param addr      device address
 * @param data      data to be sent
 * @param size      number of bytes
 * @return          number of bytes sent
 */
uint16_t I2C_Write(i2cbus_t *i2cbus, uint8_t addr, const uint8_t *data, uint16_t size)
{
    hi2c_t *hi2c;
    I2C_TypeDef *i2c;
    const uint8_t *end;

    if(!i2cbus){
        return 0;
    }

    hi2c = (hi2c_t*)i2cbus->handle;
    i2c = hi2c->i2c;

    /* Send 8-bit slave address + W */
    hi2c->last_error = i2c_master_send_addr(i2c, addr << 1);

    if (hi2c->last_error != I2C_OK){
        i2c_send_stop(i2c);
        return 0;
    }

    end = data + size;

    while (data < end){
        /* Wait until TXE flag is set (EV8)*/
        hi2c->last_error = i2c_wait_txe(i2c);
        if (hi2c->last_error != I2C_OK){
            i2c_send_stop(i2c);
            return size - (end - data);
        }

        if(!i2c_slave_ack(i2c)){
            i2c_send_stop(i2c);
            return size - (end - data);
        }

        /* Write data to DR */
        i2c->DR = *data++;
    }
    // Wait BTF (EV8_2)
    i2c_wait_btf(i2c);
    i2c_send_stop(i2c);

    return size - (end - data);
}

uint16_t I2C_Read(i2cbus_t *i2c, uint8_t addr, uint8_t *data, uint16_t size)
{
    (void)i2c;
    (void)addr;
    (void)data;
    (void)size;
    return 0;
}

/**
 * @brief Software Reset
 * @param i2c pointer to i2c handle structure
 */
void I2C_Reset(i2cbus_t *i2cbus)
{
    I2C_TypeDef *i2c = ((hi2c_t*)i2cbus->handle)->i2c;
    i2c->CR1 |= I2C_CR1_SWRST;
    i2c->CR1 &= ~I2C_CR1_SWRST;
}

