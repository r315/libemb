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
#include "lpc17xx_hal.h"
#include "i2c.h"

static I2C_Controller i2cController[I2C_MAX_ITF];
int8_t I2C_StartStateMachine(I2C_Controller *i2cifc, uint8_t op, uint8_t *data, uint32_t size);

static void i2cClock(LPC_I2C_TypeDef *i2cx, uint32_t freq){
	LPC_SC->PCLKSEL0 &= ~(3<<14);
	LPC_SC->PCLKSEL0 |=  (2<<14); //CCLK / 2

	i2cx->SCLH = ((SystemCoreClock/2)/freq)>>2;
	i2cx->SCLL = ((SystemCoreClock/2)/freq)>>2;
}

void I2C_Init(i2cbus_t *i2c){
	LPC_I2C_TypeDef *i2cx;
	I2C_Controller *ctrl;
	IRQn_Type irq;

	switch(i2c->bus_num){
		default :
		case I2C_IF0:
			i2cx = LPC_I2C0;
			PCONP_I2C0_ENABLE;
			I2C0_ConfigPins();
			irq = I2C0_IRQn;
			break;
		case I2C_IF1:
			i2cx = LPC_I2C1;
			PCONP_I2C1_ENABLE;
			I2C1_ConfigPins();
			irq = I2C1_IRQn;
			break;
		case I2C_IF2:
			i2cx = LPC_I2C2;
			I2C2_ConfigPins();
			irq = I2C2_IRQn;
			break;
	}

	ctrl = &i2cController[i2c->bus_num];

	i2cx->CONCLR = I2C_CONCLR_I2ENC;

	i2cClock(i2cx, I2C_DEFAULT_CLOCK);

	i2cx->CONSET = I2C_CONSET_I2EN;
	i2cx->CONCLR = I2C_CONCLR_STAC | I2C_CONCLR_SIC;

	ctrl->interface = i2cx;
	ctrl->state = I2C_IDLE;

	i2c->peripheral = ctrl;

	NVIC_EnableIRQ(irq);
}


void I2C_ReadIT(i2cbus_t *i2c, uint8_t *data, uint32_t size, void (*cb)(void)){
	((I2C_Controller*)i2c->peripheral)->cb = cb;
	I2C_StartStateMachine( i2c->peripheral, DATA_READ, data, size);
}

void I2C_WriteIT(i2cbus_t *i2c, const uint8_t *data, uint32_t size, void (*cb)(void)){
	((I2C_Controller*)i2c->peripheral)->cb = cb;
	I2C_StartStateMachine( i2c->peripheral, DATA_WRITE, (uint8_t*)data, size);
}

uint16_t I2C_Write(i2cbus_t *i2c, uint8_t addr, const uint8_t *data, uint16_t size){
	I2C_Controller *ctrl = (I2C_Controller*)i2c->peripheral;
	ctrl->device = addr;
	I2C_WriteIT(i2c, data, size, NULL);
	while(ctrl->state != I2C_IDLE){
       if(ctrl->state == ERROR_SLA_NACK){
            return 0;
        }
    }
	return ctrl->size;
}

uint16_t I2C_Read(i2cbus_t *i2c, uint8_t addr, uint8_t *data, uint16_t size){
	I2C_Controller *ctrl = (I2C_Controller*)i2c->peripheral;
	ctrl->device = addr;
	I2C_ReadIT(i2c, data, size, NULL);
	while(ctrl->state != I2C_IDLE ){
        if(ctrl->state == ERROR_SLA_NACK){
            return 0;
        }
    }
	return ctrl->size;
}


int8_t I2C_StartStateMachine(I2C_Controller *i2cifc, uint8_t op, uint8_t *data, uint32_t size){
	uint32_t timeout = I2C_MAX_TIMEOUT;

	i2cifc->count = 0;
	i2cifc->size = size;
	i2cifc->data = data;
	i2cifc->operation = op;

	i2cifc->interface->CONSET = I2C_CONSET_STA;

	while(timeout){
		if ( i2cifc->state != I2C_IDLE ){
			return i2cifc->count;
		}
		timeout--;
	}

	i2cifc->interface->CONSET = I2C_CONSET_STO;
  	i2cifc->interface->CONCLR = I2C_CONCLR_SIC;

	return 0;
}

/**
 * @brief this state machine only process write and read sequences
 * Write  |S| dev+W | data | data+n |P|
 * Read   |S| dev+R | data | data+n |P|
 * TODO: Furder tests
 **/
static void I2C_StateMachine(I2C_Controller *i2cifc){

	//DBG("%X\n",i2cifc->interface->I2STAT);
	switch(i2cifc->interface->STAT)
	{
	case I2C_START:
	case I2C_REPEATEAD_START:
		if(i2cifc->operation == DATA_READ){
			i2cifc->interface->DAT = i2cifc->device | 1;
			i2cifc->state = SLA_READ;
		}
		else{
			i2cifc->interface->DAT = i2cifc->device;
			i2cifc->state = SLA_WRITE;
		}
		i2cifc->interface->CONCLR = I2C_CONCLR_STAC;
		break;

		//SLA+W was transmitted, ACK from slave was received
		//send first data
	case I2C_SLA_W_ACK:
		i2cifc->state = DATA_WRITE;
		//Data was transmitted and ACK received,
		//send remaining data
	case I2C_DTA_W_ACK:
		if(i2cifc->count < i2cifc->size){
			i2cifc->interface->DAT = *(i2cifc->data + i2cifc->count);
			i2cifc->interface->CONSET = I2C_CONSET_AA;
			i2cifc->count++;
		}else{
			i2cifc->state = CALL_CB;
			i2cifc->interface->CONSET = I2C_CONSET_STO;
		}
		break;

		//SLA+R was transmitted, ACK from slave was received
		//set ACK bit
	case I2C_SLA_R_ACK:
		i2cifc->state = DATA_READ;
		i2cifc->interface->CONSET = I2C_CONSET_AA;
		break;
		//Data was received and ACK transmitted,
		//receive remaining data
	case I2C_DTA_R_ACK:
		*(i2cifc->data + i2cifc->count) = i2cifc->interface->DAT;
		i2cifc->count++;
		if(i2cifc->count < i2cifc->size){
			i2cifc->interface->CONSET = I2C_CONSET_AA;
		}else{
			i2cifc->state = CALL_CB;
			i2cifc->interface->CONCLR = I2C_CONSET_AA;
		}
		break;

    case I2C_SLA_R_NACK:
	case I2C_SLA_W_NACK:
		i2cifc->state = ERROR_SLA_NACK;
		i2cifc->interface->CONCLR = I2C_CONCLR_STAC;
		i2cifc->interface->CONSET = I2C_CONSET_STO;
		break;

	case I2C_DTA_W_NACK:
		i2cifc->state = I2C_IDLE;
		i2cifc->interface->CONSET = I2C_CONSET_STO;
		break;

	case I2C_DTA_R_NACK:
		i2cifc->state = I2C_IDLE;
		i2cifc->interface->CONSET = I2C_CONSET_STO;
		break;

	case I2C_SLA_LOST:
		i2cifc->state = I2C_IDLE;
		i2cifc->interface->CONSET = I2C_CONSET_STO;
		break;

	case I2C_NO_INFO:
	default:
		break;
	}

	i2cifc->interface->CONCLR = I2C_CONCLR_SIC;

	if (i2cifc->state == CALL_CB){
		i2cifc->state = I2C_IDLE;
		if( i2cifc->cb != NULL){
			i2cifc->cb();
		}
	}
}

void I2C0_IRQHandler(void){
	I2C_StateMachine(&i2cController[I2C_IF0]);
}
void I2C1_IRQHandler(void){
	I2C_StateMachine(&i2cController[I2C_IF1]);
}
void I2C2_IRQHandler(void){
	I2C_StateMachine(&i2cController[I2C_IF2]);
}
