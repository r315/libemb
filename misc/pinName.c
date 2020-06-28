
#include "pinName.h"

#ifdef STM32F103xB

GPIO_TypeDef *pinPorts[] = {GPIOA, GPIOB, GPIOC, GPIOD};

void pinInit(pinName_e name, uint8_t mode) {
GPIO_TypeDef *port = pinPorts[PIN_NAME_TO_PORT(name)];
uint8_t pin = PIN_NAME_TO_PIN(name);
    
    if(mode == GPI_PD){
        port->BRR = (1 << pin);
    }

    if(mode == GPI_PU){
        port->BSRR = (1 << pin);
    }    

    mode &= 0x0f;

    if(pin <  8){ 
        port->CRL = (port->CRL & ~(15 << (pin << 2))) | (mode << (pin << 2));
    }else{ 
        port->CRH = (port->CRH & ~(15 << ((pin - 8) << 2))) | (mode << ((pin - 8) << 2)); 
    }
}


void pinWrite(pinName_e name, uint8_t state){
GPIO_TypeDef *port = pinPorts[PIN_NAME_TO_PORT(name)];
uint8_t pin = PIN_NAME_TO_PIN(name);
    port->BSRR = (state != 0)? (1<<pin) : (1<<(pin+16));
}

void pinToggle(pinName_e name){
GPIO_TypeDef *port = pinPorts[PIN_NAME_TO_PORT(name)];
uint8_t pin = PIN_NAME_TO_PIN(name);
    port->ODR = port->IDR ^ (1<<pin);
}
#endif
