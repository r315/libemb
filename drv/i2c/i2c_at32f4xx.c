/**
 * @file		i2c.c
 * @brief	Contains API source code for controlling a i2c bus
 *
 * @version	1.0
 * @date		6 Abr, 2017
 * @author	Hugo Reis
 **********************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "at32f4xx.h"
#include "at32f4xx_rcc.h"
#include "dma_at32f4xx.h"
#include "i2c.h"
#include "dma.h"

#define I2C_TIMEOUT     0x10000;

typedef enum i2c_state{
    I2C_STATE_IDLE = 0,
    I2C_STATE_START,
    I2C_STATE_ADDR,
    I2C_STATE_DATA,
    I2C_STATE_STOP,
}i2c_state_t;

typedef struct{
    I2C_Type *i2c;
    void (*eot)(void);
    i2c_state_t state;
    dmatype_t dma_tx;
    uint8_t device_addr;
}hi2c_t;

static hi2c_t hi2ca = {
    .i2c = I2C1,
    .eot = NULL
}, hi2cb = {
    .i2c = I2C2,
    .eot = NULL
};

static void i2c_send_start(I2C_Type *i2c){i2c->CTRL1 |= I2C_CTRL1_STARTGEN;}
static void i2c_send_stop(I2C_Type *i2c){ i2c->CTRL1 |= I2C_CTRL1_STOPGEN; }
static void i2c_send_ack(I2C_Type *i2c){ i2c->CTRL1 |= I2C_CTRL1_ACKEN; }
static void i2c_send_nack_stop(I2C_Type *i2c){ i2c->CTRL1 = (i2c->CTRL1 & ~I2C_CTRL1_ACKEN) | I2C_CTRL1_STOPGEN; }
static void i2c_data_snd(I2C_Type *i2c, const uint8_t *data){ i2c->DT = *data; }
static void i2c_data_rcv(I2C_Type *i2c, uint8_t *data){ *data = (uint8_t)i2c->DT; }
static uint32_t i2c_slave_ack(I2C_Type *i2c){ return !(i2c->STS1 & I2C_STS1_ACKFAIL); }
static void i2c_en(I2C_Type *i2c){ i2c->CTRL1 = I2C_CTRL1_PEN; }

/**
 * @brief
 * @param hi2c
 */
static void i2c_eot_handler(hi2c_t *hi2c)
{
    (void)hi2c;
}

static void i2c_eota(void){i2c_eot_handler(&hi2ca);}
static void i2c_eotb(void){i2c_eot_handler(&hi2cb);}

/**
 * @brief Waits until timeout, while BUSY flag is set
 * @param i2c
 * @return
 */
static uint32_t i2c_wait_busy(I2C_Type *i2c)
{
    uint32_t timeout = I2C_TIMEOUT;

    if(i2c->STS1 & I2C_STS1_BUSERR){
        i2c->CTRL1 = 0;
        return I2C_ERR_BUS;
    }

    while(i2c->STS2 & I2C_STS2_BUSYF){
        if(--timeout == 0){
            i2c->CTRL1 = 0;
            return I2C_ERR_BUSY;
        }
    }

    return I2C_SUCCESS;
}

/**
 * @brief Status flag timeout wait
 * @param reg
 * @param mask
 * @return
 */
static uint32_t i2c_flag_wait(volatile uint16_t *reg, uint16_t mask)
{
    uint32_t timeout = I2C_TIMEOUT;

    while(!(*reg & mask)){
        if(--timeout == 0){
            return I2C_ERR_TIMEOUT;
        }
    }

    return I2C_SUCCESS;
}

static uint32_t i2c_wait_start(I2C_Type *i2c){ return i2c_flag_wait(&i2c->STS1, I2C_STS1_STARTF); }
static uint32_t i2c_wait_tde(I2C_Type *i2c){ return i2c_flag_wait(&i2c->STS1, I2C_STS1_TDE); }
static uint32_t i2c_wait_btf(I2C_Type *i2c){ return i2c_flag_wait(&i2c->STS1, I2C_STS1_BTFF); }
static uint32_t i2c_wait_rdne(I2C_Type *i2c){ return i2c_flag_wait(&i2c->STS1, I2C_STS1_RDNE); }

/**
 * @brief Initiates i2c communication and
 * handles errors.
 *
 * @param i2c
 * @param addr  8-bit address with RW bit
 * @return
 */
static uint32_t i2c_master_transmit(I2C_Type *i2c, uint8_t addr)
{
    uint32_t timeout, res = I2C_ERR_TIMEOUT;
    volatile uint32_t status;

    i2c_send_start(i2c);

    if(i2c_wait_start(i2c) != I2C_SUCCESS){
        i2c_send_stop(i2c);
        return I2C_ERR_START;
    }

    // EV1
    i2c->DT = addr;

    timeout = I2C_TIMEOUT;

    do{
        status = i2c->STS1;
        if(status & I2C_STS1_ADDRF){
            // Address sent and slave sent ACK
            // clear ADDRF flag (EV2)
            status = i2c->STS2;
            return I2C_SUCCESS;
        }

        if(status & I2C_STS1_ACKFAIL){
            // Slave sent NACK
            res = I2C_ERR_ACK;
            i2c->STS1 &= ~I2C_STS1_ACKFAIL;
            break;
        }

        if(status & I2C_STS1_BUSERR){
            i2c->CTRL1 = 0;
            return I2C_ERR_BUS;
        }
    }while(--timeout > 0);

    // Failed to sent Slave address
    // SCL/SDA lines stuck?
    i2c_send_stop(i2c);

    return res;
}

/**
 * @brief Configures respective bus pins
 * @param bus_num
 */
static void i2c_gpio_cfg(uint8_t bus_num)
{
    switch (bus_num){
        case I2C_BUS0:
            RCC->APB2EN |= RCC_APB2EN_GPIOBEN;
            GPIOB->CTRLL = (GPIOB->CTRLL & ~(0xFF << 24)) | (0xAA << 24); // PB6/PB7 GPO_AF_PP
            break;

        case I2C_BUS1:
            RCC->APB2EN |= RCC_APB2EN_GPIOBEN;
            GPIOB->CTRLH = (GPIOB->CTRLH & ~(0xFF << 8)) | (0xAA << 8); // PB10/PB11
            break;

        case I2C_BUS2:
            RCC->APB2EN |= RCC_APB2EN_AFIOEN | RCC_APB2EN_GPIOBEN;
            AFIO->MAP5 = (AFIO->MAP & ~(7 << 4)) | AFIO_MAP5_I2C1_GRMP_01;
            GPIOB->CTRLH = (GPIOB->CTRLH & ~(0xFF << 0)) | (0xAA << 0); // PB8/PB9
            break;

        case I2C_BUS3:
            RCC->APB2EN |= RCC_APB2EN_AFIOEN | RCC_APB2EN_GPIOFEN;
            AFIO->MAP5 = (AFIO->MAP & ~(7 << 8)) | AFIO_MAP5_I2C2_GRMP_11;
            GPIOF->CTRLL = (GPIOF->CTRLL & ~(0xFF << 24)) | (0xAA << 24); // PF6/PF7
            break;

        default:
            break;
    }
}


/**
 * @brief Configure i2c speed and timming
 * @param i2c       pointer to peripheral registers
 * @param speed     speed in Hz
 * @return          1: sucess, 0: otherwise
 */
static uint32_t i2c_speed_cfg(I2C_Type *i2c, uint32_t pclk, uint32_t speed, uint32_t dutycycle)
{
    uint16_t freq_range;
    uint32_t result;

    freq_range = (uint16_t)(pclk / 1000000UL);

    if(freq_range < 2 || freq_range > 120){
        return I2C_ERR_PARM;
    }

    i2c->CTRL1 = I2C_CTRL1_SWRESET;
    /* Disable I2C peripheral to configure TRISE */
    i2c->CTRL1 = 0;
    /* Config input clock */
    i2c->CTRL2 = (i2c->CTRL2 & ~I2C_CTRL2_CLKFREQ) | freq_range;

    if(speed <= 100000UL){
        result = pclk / (speed << 1); // calculate period
        /* minimum allowed value */
        if(result < 4){ result = 4; }

        i2c->CLKCTRL = result;
        /* Set Maximum Rise Time for standard mode */
        i2c->TMRISE = freq_range + 1;
    }else{
        if(dutycycle){
            result = pclk / (speed * 25UL) | I2C_CLKCTRL_FMDUTY;
        }else{
            /* Fast mode speed calculate: Tlow/Thigh = 2 */
            result = pclk / (speed * 3UL);
        }

        /* Set minimum allowed value */
        if(!(result & I2C_CLKCTRL_CLKCTRL)){
            result |= 1;
        }
        /* Set F/S bit for fast mode */
        i2c->CLKCTRL = result | I2C_CLKCTRL_FSMODE;
        /* Set Maximum Rise Time for fast mode */
        i2c->TMRISE = (uint16_t)(((freq_range * (uint16_t)300) / (uint16_t)1000) + (uint16_t)1);
    }

    return I2C_SUCCESS;
}

/**
 * @brief
 * @param i2cbus
 * @return
 */
uint32_t I2C_Init (i2cbus_t *i2cbus){
    hi2c_t *hi2c;
    RCC_ClockType clocks;
    uint32_t pclk;

    RCC_GetClocksFreq(&clocks);
    pclk = clocks.APB1CLK_Freq;

	switch(i2cbus->bus_num){
		case I2C_BUS0:
        case I2C_BUS2:
            RCC->APB1EN |= RCC_APB1EN_I2C1EN;
            hi2c = &hi2ca;
            break;

        case I2C_BUS1:
        case I2C_BUS3:
            RCC->APB1EN |= RCC_APB1EN_I2C2EN;
            hi2c = &hi2cb;
            break;

		default:
			return I2C_ERR_PARM;
	}

    i2cbus->handle = hi2c;

    if(i2c_speed_cfg(hi2c->i2c, pclk, i2cbus->speed * 1000UL,
        i2cbus->cfg & I2C_CFG_9_16) != I2C_SUCCESS){
        return I2C_ERR_PARM;
    }

    if(i2cbus->cfg & I2C_CFG_DMA){
        hi2c->dma_tx.dst = (void*)&hi2c->i2c->DT;
        hi2c->dma_tx.dsize = DMA_DATA_SIZE_8;
        hi2c->dma_tx.src = NULL;
        hi2c->dma_tx.ssize = DMA_DATA_SIZE_8;
        hi2c->dma_tx.dir = DMA_DIR_M2P;
        hi2c->dma_tx.single = 1;
        if(hi2c->i2c == I2C1){
            hi2c->dma_tx.eot = i2c_eota;
            DMA_Config(&hi2c->dma_tx, DMA1_REQ_I2C1_TX);
            NVIC_EnableIRQ(I2C1_EV_IRQn);
            NVIC_EnableIRQ(I2C1_ER_IRQn);
        }else{
            hi2c->dma_tx.eot = i2c_eotb;
            DMA_Config(&hi2c->dma_tx, DMA1_REQ_I2C2_TX);
            NVIC_EnableIRQ(I2C2_EV_IRQn);
            NVIC_EnableIRQ(I2C2_ER_IRQn);
        }
        hi2c->state = I2C_STATE_IDLE;
    }

    if(i2cbus->cfg & I2C_CFG_PINS){
        i2c_gpio_cfg(i2cbus->bus_num);
    }

    return I2C_SUCCESS;
}

/**
 * @brief Resets and initializes i2c bus,
 * Note sure if necessary
 * @param i2cbus
 */
void I2C_Reset(i2cbus_t *i2cbus){
    I2C_Type *i2c = (I2C_Type*)i2cbus->handle;
    i2c->CTRL1 |= I2C_CTRL1_SWRESET;
    I2C_Init(i2cbus);
}

/**
 * @brief Master transmitter
 * @param i2cbus
 * @param addr
 * @param data
 * @param size
 * @return
 */
uint16_t I2C_Write(i2cbus_t *i2cbus, uint8_t addr, const uint8_t *data, uint16_t size){
    hi2c_t *hi2c;
    I2C_Type *i2c;
    const uint8_t *end;

    if(!i2cbus || !data){
        return 0;
    }

    hi2c = (hi2c_t*)i2cbus->handle;
    i2c = hi2c->i2c;

    i2c_wait_busy(i2c);
    i2c_en(i2c);

    /* Send 8-bit slave address + W */
    if (i2c_master_transmit(i2c, addr << 1) != I2C_SUCCESS){
        return 0;
    }

    end = data + size;

    while (data < end){
        /* Wait until TDE flag is set (EV3) */
        if (i2c_wait_tde(i2c) != I2C_SUCCESS){
            i2c_send_stop(i2c);
            return size - (end - data);
        }

        if(!i2c_slave_ack(i2c)){
            /* Slave ended transfer */
            i2c_send_stop(i2c);
            return size - (end - data);
        }

        /* Write data to DR (EV4) */
        i2c_data_snd(i2c, data++);
    }
    // Wait BTF (EV8_2)
    i2c_wait_btf(i2c);
    i2c_send_stop(i2c);

    return size - (end - data);
}

/**
 * @brief Master receiver with very high priority
 * @param i2cbus
 * @param addr
 * @param data
 * @param size
 * @return
 */
uint16_t I2C_Read(i2cbus_t *i2cbus, uint8_t addr, uint8_t *data, uint16_t size)
{
    hi2c_t *hi2c;
    I2C_Type *i2c;
    uint8_t remaining;

    if(!i2cbus || !data){
        return 0;
    }

    hi2c = (hi2c_t*)i2cbus->handle;
    i2c = hi2c->i2c;

    i2c_wait_busy(i2c);
    i2c_en(i2c);

    remaining = size;

    /* Send 8-bit slave address + R */
    if (i2c_master_transmit(i2c, (addr << 1) | 1) != I2C_SUCCESS){
        return 0;
    }

    while (remaining){
        if(remaining == 1){
            // (EV4)
            i2c_send_nack_stop(i2c);
        }else{
            i2c_send_ack(i2c);
        }

        /* Wait until RDNE flag is set (EV3)*/
        if (i2c_wait_rdne(i2c) != I2C_SUCCESS){
            i2c_send_stop(i2c);
            return size - remaining;
        }

        /* Read data from DT */
        i2c_data_rcv(i2c, data++);
        remaining--;
    }

    return size - remaining;
}

/**
 * @brief
 * @param i2cbus
 * @param addr
 * @param data
 * @param size
 * @return
 */
uint32_t I2C_TransmitDMA(i2cbus_t *i2cbus, uint8_t addr, const uint8_t *data, uint16_t size)
{
    (void)i2cbus;
    (void)addr;
    (void)data;
    (void)size;
    return I2C_SUCCESS;
}