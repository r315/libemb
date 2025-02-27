#include "i2c.h"
#include "gd32e23x_rcu.h"
#include "gd32e23x_i2c.h"

#define I2Cx_OWN_ADDRESS7      0x72

enum {
    I2C_EVENT_CHECK_NONE = 0,
    I2C_EVENT_CHECK_ACKFAIL,
    I2C_EVENT_CHECK_STOP
};

typedef enum
{
    I2C_OK = 0,          /*!< no error */
    I2C_ERR_STEP_1,      /*!< step 1 error */
    I2C_ERR_STEP_2,      /*!< step 2 error */
    I2C_ERR_STEP_3,      /*!< step 3 error */
    I2C_ERR_STEP_4,      /*!< step 4 error */
    I2C_ERR_STEP_5,      /*!< step 5 error */
    I2C_ERR_STEP_6,      /*!< step 6 error */
    I2C_ERR_STEP_7,      /*!< step 7 error */
    I2C_ERR_STEP_8,      /*!< step 8 error */
    I2C_ERR_STEP_9,      /*!< step 9 error */
    I2C_ERR_STEP_10,     /*!< step 10 error */
    I2C_ERR_STEP_11,     /*!< step 11 error */
    I2C_ERR_STEP_12,     /*!< step 12 error */
    I2C_ERR_START,       /*!< start error */
    I2C_ERR_ADDR10,      /*!< addr10 error */
    I2C_ERR_ADDR,        /*!< addr error */
    I2C_ERR_STOP,        /*!< stop error */
    I2C_ERR_ACKFAIL,     /*!< ackfail error */
    I2C_ERR_TIMEOUT,     /*!< timeout error */
    I2C_ERR_INTERRUPT,   /*!< interrupt error */
} i2c_status_t;

typedef struct {
    volatile uint32_t periph;

}i2c_handle_t;

static i2c_handle_t hi2cx;

static void i2c_init_gpio(i2cbusnum_t bus_num)
{
    uint32_t port;
    uint16_t pin;

    switch (bus_num){
        case I2C_BUS0:
            rcu_periph_clock_enable(RCU_GPIOB);
            port = GPIOB;
            pin = GPIO_PIN_6 | GPIO_PIN_7;
            gpio_af_set(port, GPIO_AF_1, pin);
            break;

        case I2C_BUS1:
            rcu_periph_clock_enable(RCU_GPIOA);
            port = GPIOA;
            pin = GPIO_PIN_1 | GPIO_PIN_0;
            gpio_af_set(port, GPIO_AF_4, pin);
            break;

        case I2C_BUS2:
        case I2C_BUS3:
            rcu_periph_clock_enable(RCU_GPIOF);
            port = GPIOF;
            pin = GPIO_PIN_6 | GPIO_PIN_7;
            gpio_af_set(port, GPIO_AF_0, pin);
            break;

        default:
            return;
    }

    gpio_mode_set(port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin);
    gpio_output_options_set(port, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, pin);
}

static i2c_status_t i2c_wait_flag(uint32_t i2c_periph, uint32_t flag, uint32_t event_check, uint32_t timeout)
{
    if (flag == I2C_FLAG_I2CBSY) {
        while (i2c_flag_get(i2c_periph, flag) != RESET) {
            /* check timeout */
            if ((timeout--) == 0) {
                return I2C_ERR_TIMEOUT;
            }
        }
    } else {
        while (i2c_flag_get(i2c_periph, flag) == RESET) {
            /* check the ack fail flag */
            if (event_check & I2C_EVENT_CHECK_ACKFAIL) {
                if (i2c_flag_get(i2c_periph, I2C_FLAG_AERR) != RESET) {
                    /* generate stop condtion */
                    i2c_stop_on_bus(i2c_periph);

                    /* clear ack fail flag */
                    i2c_flag_clear(i2c_periph, I2C_FLAG_AERR);
                    return I2C_ERR_ACKFAIL;
                }
            }

            /* check the stop flag */
            if (event_check & I2C_EVENT_CHECK_STOP) {
                if (i2c_flag_get(i2c_periph, I2C_FLAG_STPDET) != RESET)
                {
                    /* clear stop flag */
                    i2c_flag_clear(i2c_periph, I2C_FLAG_STPDET);
                    return I2C_ERR_STOP;
                }
            }

            /* check timeout */
            if ((timeout--) == 0) {
                return I2C_ERR_TIMEOUT;
            }
        }
    }
    return I2C_OK;
}

static i2c_status_t i2c_send_start_and_addr(uint32_t i2c_periph, uint16_t addr,  uint32_t ack, uint32_t timeout)
{
     /* wait until I2C bus is idle */
    if (i2c_wait_flag(i2c_periph, I2C_FLAG_I2CBSY, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK){
        return I2C_ERR_STEP_1;
    }

    /* enable ack */
    i2c_ack_config(i2c_periph, ack);

    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2c_periph);

    /* wait until SBSEND bit is set */
    if(i2c_wait_flag(i2c_periph, I2C_FLAG_SBSEND, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK){
        return I2C_ERR_STEP_2;
    }

    /* send slave address to I2C bus */
    I2C_DATA(i2c_periph) = addr;

    /* wait until ADDSEND bit is set */
    if(i2c_wait_flag(i2c_periph, I2C_FLAG_ADDSEND, I2C_EVENT_CHECK_ACKFAIL, timeout) != I2C_OK){
        return I2C_ERR_STEP_3;
    }

    /* clear ADDSEND bit */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);

    return I2C_OK;
}

i2c_status_t i2c_master_transmit(uint32_t i2c_periph, uint16_t slave, const uint8_t* pdata, uint16_t len, uint32_t timeout)
{
    if(i2c_send_start_and_addr(i2c_periph, (slave << 1) & I2C_TRANSMITTER, I2C_ACK_DISABLE, timeout)){
        return I2C_ERR_STEP_1;
    }

    for(uint16_t i = 0; i < len; i++) {
        /* wait until the transmit data buffer is empty */
        if(i2c_wait_flag(i2c_periph, I2C_FLAG_TBE, I2C_EVENT_CHECK_ACKFAIL, timeout)!= I2C_OK){
            i2c_stop_on_bus(i2c_periph);
            return I2C_ERR_STEP_4;
        }

        /* data transmission */
        i2c_data_transmit(i2c_periph, pdata[i]);
    }

    if(i2c_wait_flag(i2c_periph, I2C_FLAG_BTC, 0, timeout) != I2C_OK){
        i2c_stop_on_bus(i2c_periph);
        return I2C_ERR_STEP_5;
    }

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(i2c_periph);

    return I2C_OK;
}

i2c_status_t i2c_master_receive(uint32_t i2c_periph, uint16_t slave, uint8_t *pdata, uint16_t len, uint32_t timeout)
{
    if(i2c_send_start_and_addr(i2c_periph, (slave << 1) | I2C_RECEIVER, I2C_ACK_ENABLE, timeout)){
        return I2C_ERR_STEP_1;
    }

    if (len == 1){
        /* Only one byte to be recieved, disable ack for first byte */
        i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
        i2c_stop_on_bus(i2c_periph);
    } else if (len == 2) {
        /* ack acts on the next byte */
        i2c_ackpos_config(i2c_periph, I2C_ACKPOS_NEXT);
        i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
    } else {
        i2c_ack_config(i2c_periph, I2C_ACK_ENABLE);
    }

    while (len > 0)
    {
        if (len <= 3) {
            /* 1 byte to read */
            if (len == 1) {
                if (i2c_wait_flag(i2c_periph, I2C_FLAG_RBNE, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK) {
                    i2c_stop_on_bus(i2c_periph);
                    return I2C_ERR_STEP_3;
                }
                (*pdata++) = i2c_data_receive(i2c_periph);
            }
            /* 2 bytes */
            else if (len == 2) {
                /* wait for second data byte to be transfered */
                if (i2c_wait_flag(i2c_periph, I2C_FLAG_BTC, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK) {
                    i2c_stop_on_bus(i2c_periph);
                    return I2C_ERR_STEP_4;
                }
                /* Send stop condtion after second data byte*/
                i2c_stop_on_bus(i2c_periph);
                /* Get data from data register, data on shit register
                 * is transfered to data register after read */
                (*pdata++) = i2c_data_receive(i2c_periph);
                len--;
                /* Get second data byte from data register*/
                (*pdata++) = i2c_data_receive(i2c_periph);
            }
            /* 3 last bytes */
            else {
                if (i2c_wait_flag(i2c_periph, I2C_FLAG_BTC, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK) {
                    i2c_stop_on_bus(i2c_periph);
                    return I2C_ERR_STEP_5;
                }
                i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
                (*pdata++) = i2c_data_receive(i2c_periph);
                len--;

                if (i2c_wait_flag(i2c_periph, I2C_FLAG_BTC, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK) {
                    i2c_stop_on_bus(i2c_periph);
                    return I2C_ERR_STEP_6;
                }
                i2c_stop_on_bus(i2c_periph);

                (*pdata++) = i2c_data_receive(i2c_periph);
                len--;
                (*pdata++) = i2c_data_receive(i2c_periph);
            }
        } else {
            if (i2c_wait_flag(i2c_periph, I2C_FLAG_RBNE, I2C_EVENT_CHECK_NONE, timeout) != I2C_OK) {
                i2c_stop_on_bus(i2c_periph);
                return I2C_ERR_STEP_7;
            }

            (*pdata++) = i2c_data_receive(i2c_periph);
        }
        len--;
    }
    return I2C_OK;
}

void I2C_Init(i2cbus_t *i2c)
{
    switch(i2c->bus_num){
        case I2C_BUS0:
        case I2C_BUS2:
            rcu_periph_clock_enable(RCU_I2C0);
            hi2cx.periph = I2C0;
            break;

        case I2C_BUS1:
        case I2C_BUS3:
            rcu_periph_clock_enable(RCU_I2C1);
            hi2cx.periph = I2C1;
            break;

        default:
            return;
    }

    i2c_init_gpio(i2c->bus_num);

    i2c_deinit(hi2cx.periph);
    /* configure I2C clock */
    i2c_clock_config(hi2cx.periph, i2c->speed, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(hi2cx.periph, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2Cx_OWN_ADDRESS7);
    /* enable I2C0 */
    i2c_enable(hi2cx.periph);
}

uint32_t I2C_Write(i2cbus_t *i2c, const uint8_t *data, uint32_t size)
{
    return i2c_master_transmit(hi2cx.periph, i2c->addr, data, size, 1000) != I2C_OK ? 0 : size;
}

uint32_t I2C_Read(i2cbus_t *i2c, uint8_t *data, uint32_t size)
{
    return i2c_master_receive(hi2cx.periph, i2c->addr, data, size, 1000) != I2C_OK ? 0 : size;
}
