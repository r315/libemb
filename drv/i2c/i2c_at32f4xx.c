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
#include "i2c.h"

static void i2c_gpio_init(i2cbus_t *i2cbus)
{
    switch (i2cbus->bus_num){
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

void I2C_Init (i2cbus_t *i2cbus){
    I2C_Type *i2c;
    I2C_InitType  init;

	switch(i2cbus->bus_num){
		case I2C_BUS0:
        case I2C_BUS2:
            RCC->APB1EN |= RCC_APB1EN_I2C1EN;
            i2c = I2C1;
            break;

        case I2C_BUS1:
        case I2C_BUS3:
            RCC->APB1EN |= RCC_APB1EN_I2C2EN;
            i2c = I2C2;
            break;

		default:
			return;
	}

    i2cbus->peripheral = i2c;

    init.I2C_FmDutyCycle = I2C_FmDutyCycle_2_1;
    init.I2C_OwnAddr1 = I2C_SLAVE_ADDRESS7;
    init.I2C_Ack = I2C_Ack_Enable;
    init.I2C_AddrMode = I2C_AddrMode_7bit;
    init.I2C_BitRate = i2cbus->speed;
    init.I2C_Mode = I2C_Mode_I2CDevice;

    I2C_Cmd(i2c, ENABLE);

    I2C_Initialize(i2c, &init);

    i2c_gpio_init(i2cbus);
}

void static i2c_reset_on_error(i2cbus_t *i2cbus, I2C_StatusType lastres){
    I2C_Type *i2c = (I2C_Type*)i2cbus->peripheral;

    if(lastres == I2C_ERROR_STEP_1){
        // fail waiting on busy flag
        I2C_Reset(i2cbus);
        return;
    }

    uint16_t sts = i2c->STS2;
    if(sts & I2C_STS2_BUSYF){
        sts = i2c->STS1;
        if(sts & (I2C_STS1_BUSERR | I2C_STS1_ARLOST)){
            I2C_Reset(i2cbus);
        }
    }
}

uint16_t I2C_Write(i2cbus_t *i2cbus, uint8_t addr, const uint8_t *data, uint16_t size){
    I2C_StatusType res = I2C_Master_Transmit(i2cbus->peripheral, addr, (uint8_t*)data, size, 1000);

    if(res != I2C_OK){
        i2c_reset_on_error(i2cbus, res);
        return 0;
    }

    return size;
}

uint16_t I2C_Read(i2cbus_t *i2cbus, uint8_t addr, uint8_t *data, uint16_t size){
	I2C_StatusType res = I2C_Master_Receive(i2cbus->peripheral, addr, data, size, 1000);

    if(res != I2C_OK){
        i2c_reset_on_error(i2cbus, res);
        return 0;
    }

    return size;
}

void I2C_Reset(i2cbus_t *i2cbus){
    I2C_Type *i2c = (I2C_Type*)i2cbus->peripheral;
    I2C_DeInit(i2c);
    I2C_Init(i2cbus);
}

