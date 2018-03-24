#include <gpio.h>

void GPIO_ResetPINSEL(void){
	PINCON->PINSEL0 = 0;
	PINCON->PINSEL1 = 0;
	PINCON->PINSEL2 = 0;
	PINCON->PINSEL3 = 0;
	PINCON->PINSEL4 = 0;
	PINCON->PINSEL7 = 0;
}

void GPIO_ConfigDir(GPIO_TypeDef *gpio, uint8_t pin,  uint8_t dir){
    if(dir == GPIO_INPUT)
        gpio->FIODIR &= ~(1<<pin);
    else
        gpio->FIODIR |= (1<<pin);
}


void GPIO_ConfigPin(uint8_t port, uint8_t pin, uint8_t dir, uint8_t state){
    switch(port){
        case GPIO_P0:
            GPIO_ConfigDir(GPIO0, pin, dir);
            break;
        case GPIO_P1:
            GPIO_ConfigDir(GPIO1, pin, dir);
            break;
        case GPIO_P2:
            GPIO_ConfigDir(GPIO2, pin, dir);
            break;
        case GPIO_P3:
            GPIO_ConfigDir(GPIO3, pin, dir);
            break;  
        case GPIO_P4:
            GPIO_ConfigDir(GPIO4, pin, dir);
            break;    
        default: break;          
    }
    if(dir == GPIO_OUTPUT)
        GPIO_SetState(port, pin, state);
}

void GPIO_ConfigState(GPIO_TypeDef *gpio, uint8_t pin,  uint8_t state){
    if(state == GPIO_LOW)
        gpio->FIOCLR = (1<<pin);
    else
        gpio->FIOSET = (1<<pin);
}

void GPIO_SetState(uint8_t port, uint8_t pin, uint8_t state){
    switch(port){
        case GPIO_P0:
            GPIO_ConfigState(GPIO0, pin, state);
            break;
        case GPIO_P1:
            GPIO_ConfigState(GPIO1, pin, state);
            break;       
        case GPIO_P2:
            GPIO_ConfigState(GPIO2, pin, state);
            break;
        case GPIO_P3:
            GPIO_ConfigState(GPIO3, pin, state);
            break;
        case GPIO_P4:
            GPIO_ConfigState(GPIO4, pin, state);
            break;  
       default: break;             
    }
}

uint8_t GPIO_GetState(uint8_t port, uint8_t pin){
    switch(port){
        case GPIO_P0:
            return (GPIO0->FIOPIN & (1<<pin))? GPIO_HIGH: GPIO_LOW;
        case GPIO_P1:
            return (GPIO1->FIOPIN & (1<<pin))? GPIO_HIGH: GPIO_LOW;
        case GPIO_P2:
            return (GPIO2->FIOPIN & (1<<pin))? GPIO_HIGH: GPIO_LOW;
        case GPIO_P3:
            return (GPIO3->FIOPIN & (1<<pin))? GPIO_HIGH: GPIO_LOW;
        case GPIO_P4:
            return (GPIO4->FIOPIN & (1<<pin))? GPIO_HIGH: GPIO_LOW;
       default: return GPIO_LOW;             
    }
}

/**
 * There are 4 external interrupts and the GPIO0/2 is shared EINT3
 * so to activate interrupt in any GPIO0/2 pin the EINT3_IRQHandler must be enabled
 **/
void GPIO_SetInt(uint8_t port, uint8_t pin, uint8_t level){

    switch(port){
        case GPIO_P0:
            LPC_GPIOINT->IO0IntEnF |= (level & GPIO_INT_LOW) ? (1<<pin) : 0;
            LPC_GPIOINT->IO0IntEnR |= (level & GPIO_INT_HIGH) ? (1<<pin) : 0;
            GPIO_ConfigPin(port, pin, GPIO_INPUT, GPIO_HIGH);
            NVIC_EnableIRQ(EINT3_IRQn);
            break;
            
        case GPIO_P2:
            LPC_GPIOINT->IO2IntEnF |= (level & GPIO_INT_LOW) ? (1<<pin) : 0;
            LPC_GPIOINT->IO2IntEnR |= (level & GPIO_INT_HIGH) ? (1<<pin) : 0;
            GPIO_ConfigPin(port, pin, GPIO_INPUT, GPIO_HIGH);
            NVIC_EnableIRQ(EINT3_IRQn);
            break;
            
        default:
            break;
    }

}
