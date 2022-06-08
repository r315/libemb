/**
 * @file		i2c.c
 * @brief	Contains API source code for controlling a i2c bus
 *
 * @version	1.0
 * @date		6 Abr, 2017
 * @author	Hugo Reis
 **********************************************************************/

#include <stdint.h>
#include "i2c_lpc17xx.h"
#include <debug.h>

static I2C_Controller i2cController[I2C_MAX_ITF];

I2C_Controller *I2C_Init(uint8_t ifNumber, uint8_t dev){

	switch(ifNumber){
	default :
	case I2C_IF0:
		i2cController[ifNumber].interface = LPC_I2C0;
		LPC_SC->PCONP |= I2C0_ON;
		I2C0_ConfigPins();
		NVIC_EnableIRQ(I2C0_IRQn);
		break;
	case I2C_IF1:
		i2cController[ifNumber].interface = LPC_I2C1;
		LPC_SC->PCONP |= I2C1_ON;
		I2C1_ConfigPins();
		NVIC_EnableIRQ(I2C1_IRQn);
		break;
	case I2C_IF2:
		i2cController[ifNumber].interface = LPC_I2C2;
		LPC_SC->PCONP |= I2C2_ON;
		I2C2_ConfigPins();
		NVIC_EnableIRQ(I2C2_IRQn);
		break;
	}

	LPC_SC->PCLKSEL0 &= ~(3<<14);
	LPC_SC->PCLKSEL0 |=  (2<<14); //CCLK / 2

	i2cController[ifNumber].device = dev;
	i2cController[ifNumber].state = I2C_IDLE;

	i2cController[ifNumber].interface->CONCLR = I2C_I2EN;

	i2cController[ifNumber].interface->SCLH = ((SystemCoreClock/2)/I2C_DEFAULT_CLOCK)>>2;
	i2cController[ifNumber].interface->SCLL = ((SystemCoreClock/2)/I2C_DEFAULT_CLOCK)>>2;

	i2cController[ifNumber].interface->CONSET = I2C_I2EN;
	i2cController[ifNumber].interface->CONCLR = I2C_STA | I2C_STO | I2C_SI;

	return &i2cController[ifNumber];
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
		i2cifc->interface->CONCLR = I2C_STA;
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
			i2cifc->interface->CONSET = I2C_AA;
			i2cifc->count++;
		}else{
			i2cifc->state = CALL_CB;
			i2cifc->interface->CONSET = I2C_STO;
		}
		break;

		//SLA+R was transmitted, ACK from slave was received
		//set ACK bit
	case I2C_SLA_R_ACK:
		i2cifc->state = DATA_READ;
		i2cifc->interface->CONSET = I2C_AA;
		break;
		//Data was received and ACK transmitted,
		//receive remaining data
	case I2C_DTA_R_ACK:
		*(i2cifc->data + i2cifc->count) = i2cifc->interface->DAT;
		i2cifc->count++;
		if(i2cifc->count < i2cifc->size){
			i2cifc->interface->CONSET = I2C_AA;
		}else{
			i2cifc->state = CALL_CB;
			i2cifc->interface->CONCLR = I2C_AA;
		}
		break;

    case I2C_SLA_R_NACK:
	case I2C_SLA_W_NACK:
		i2cifc->state = ERROR_SLA_NACK;
		i2cifc->interface->CONCLR = I2C_STA;
		i2cifc->interface->CONSET = I2C_STO;
		break;

	case I2C_DTA_W_NACK:
		i2cifc->state = I2C_IDLE;
		i2cifc->interface->CONSET = I2C_STO;
		break;

	case I2C_DTA_R_NACK:
		i2cifc->state = I2C_IDLE;
		i2cifc->interface->CONSET = I2C_STO;
		break;

	case I2C_SLA_LOST:
		i2cifc->state = I2C_IDLE;
		i2cifc->interface->CONCLR = I2C_STO;
		break;

	case I2C_NO_INFO:
	default:
		break;
	}

	i2cifc->interface->CONCLR = I2C_SI;

	if (i2cifc->state == CALL_CB){
		if( i2cifc->cb != NULL){
			i2cifc->cb(i2cifc->data);
		}
		i2cifc->state = I2C_IDLE;
	}	
}

int8_t I2C_StartStateMachine(I2C_Controller *i2cifc, uint8_t *data, uint32_t size){
uint32_t timeout = I2C_MAX_TIMEOUT;
	
	i2cifc->count = 0;
	i2cifc->size = size;
	i2cifc->data = data;

	i2cifc->interface->CONSET = I2C_STA;

	while(timeout){
		if ( i2cifc->state != I2C_IDLE ){
			return i2cifc->count;
		}
		timeout--;
	}
	
	i2cifc->interface->CONSET = I2C_STO;
  	i2cifc->interface->CONCLR = I2C_SI;
	
	return 0;	
}

int32_t I2C_ReadAsync(uint8_t ifNumber, uint8_t *data, uint32_t size, void (*cb)(void*)){
	I2C_Controller *i2citf = &i2cController[ifNumber];
	i2citf->operation = DATA_READ;
	i2citf->cb = cb;
	return I2C_StartStateMachine( i2citf, data, size);
}

int32_t I2C_WriteAsync(uint8_t ifNumber, uint8_t *data, uint32_t size, void (*cb)(void*)){
	I2C_Controller *i2citf = &i2cController[ifNumber];
	i2citf->operation = DATA_WRITE;
	i2citf->cb = cb;
	return I2C_StartStateMachine( i2citf, data, size);
}

int32_t I2C_Write(uint8_t ifNumber, uint8_t *data, uint32_t size){
	I2C_WriteAsync(ifNumber, data, size, NULL);
	while( i2cController[ifNumber].state != I2C_IDLE){
       if(i2cController[ifNumber].state == ERROR_SLA_NACK){
            return (ERROR_SLA_NACK<<8);
        }  
    }
	return i2cController[ifNumber].size;
}

int32_t I2C_Read(uint8_t ifNumber, uint8_t *data, uint32_t size){
	I2C_ReadAsync(ifNumber, data, size, NULL);
	while( i2cController[ifNumber].state != I2C_IDLE ){
        if(i2cController[ifNumber].state == ERROR_SLA_NACK){
            return (ERROR_SLA_NACK<<8);
        }
    }
	return i2cController[ifNumber].size;
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
