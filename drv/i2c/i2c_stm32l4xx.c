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
#include "stm32l4xx.h"
#include "i2c.h"
#include "board.h"
#include "assert.h"

#define I2C_TIMING      ((19 << 28) | /* PRESC */ \
                        (8 << 20) | \
                        (8 << 16) | \
                        (128 << 8) | \
                        (128 << 0))

static I2C_HandleTypeDef hi2c[I2C_MAX_ITF];

void I2C_Init (i2cbus_t *i2c){

    GPIO_InitTypeDef GPIO_InitStruct;

    I2C_HandleTypeDef *hi2cx = hi2c + i2c->bus_num;
    
    hi2cx->Init.OwnAddress1 = 0;
    hi2cx->Init.OwnAddress2 = 0xFF;
    hi2cx->Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2cx->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2cx->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2cx->Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    hi2cx->Init.Timing = (15 << I2C_TIMINGR_PRESC_Pos) | // Timings valid for PCLK1 = 80MHz
                         (24 << I2C_TIMINGR_SCLL_Pos) | 
                         (19 << I2C_TIMINGR_SCLH_Pos) | 
                         (3 << I2C_TIMINGR_SDADEL_Pos) |
                         (5 << I2C_TIMINGR_SCLDEL_Pos);

  
	switch(i2c->bus_num){
		default :
		case I2C_IF0:
			hi2cx->Instance = I2C1;
            i2c->ctrl = hi2cx;
			__HAL_RCC_I2C1_CLK_ENABLE();
            __HAL_RCC_GPIOB_CLK_ENABLE();

            GPIO_InitStruct.Pin       = GPIO_PIN_7 | GPIO_PIN_6;
            GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
            GPIO_InitStruct.Pull      = GPIO_PULLUP;
            GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
			break;
	}

    if(HAL_I2C_Init(hi2cx) != HAL_OK)
    {
        return;
    }

    HAL_I2CEx_ConfigAnalogFilter(hi2cx, I2C_ANALOGFILTER_ENABLE);
}

uint32_t I2C_Write(i2cbus_t *i2c, uint8_t *data, uint32_t size){
    return (HAL_I2C_Master_Transmit(i2c->ctrl, i2c->addr, data, size, 100) == HAL_OK) ? size : 0;
}

uint32_t I2C_Read(i2cbus_t *i2c, uint8_t *data, uint32_t size){
	return (HAL_I2C_Master_Receive(i2c->ctrl, i2c->addr, data, size, 100) == HAL_OK) ? size : 0;
}
