#include "stm32f103xb.h"
#include "gpio_stm32f1xx.h"

static const uint32_t ports[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE, GPIOE_BASE};

/**
 * @brief Configure GPIO pin
 * @param port : pin port
 * @param pin : pin number 0 - 15
 * @param mode : see header
 *           
 * */
void GPIO_Config(pinName_e name, uint8_t cfg) {
    uint8_t pin = GPIO_NAME_TO_PIN(name);
    name = GPIO_NAME_TO_PORT(name);

    if(name > 3){
        return;     // invalid port
    }
    
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[name];

    cfg = GPIO_CFG_MASK(cfg);

    if(pin <  8){ 
        port->CRL = (port->CRL & ~(15 << (pin << 2))) | (cfg << (pin << 2));
    }else{ 
        port->CRH = (port->CRH & ~(15 << ((pin - 8) << 2))) | (cfg << ((pin - 8) << 2)); 
    }
    
    if(cfg == GPI_PD){
        port->BRR = (1 << pin);
    }else if(cfg == GPI_PU){
        port->BSRR = (1 << pin);
    }
}

uint32_t GPIO_Read(uint32_t name){
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[GPIO_NAME_TO_PORT(name)];
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    return !!(port->IDR & (1 << pin));
}

void GPIO_Write(pinName_e name, uint8_t state){
    uint8_t pin = GPIO_NAME_TO_PIN(name);
    name = GPIO_NAME_TO_PORT(name);

    if(name > 3){
        return;     // invalid port
    }
    
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[name];

    port->BSRR = (state != 0)? (1<<pin) : (1<<(pin+16));
}

void GPIO_Toggle(pinName_e name){    
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    name = GPIO_NAME_TO_PORT(name);

    if(name > 3){
        return;     // invalid port
    }
    
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[name];

    port->ODR = port->IDR ^ (1<<pin);
}

void GPIO_PORT_Write(pinName_e name, uint32_t value){
    name = GPIO_NAME_TO_PORT(name);

    if(name > 3){
        return;     // invalid port
    }
    
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[name];

    port->ODR = value;
}

uint32_t GPIO_PORT_Read(pinName_e name){
    name = GPIO_NAME_TO_PORT(name);

    if(name > 3){
        return -1;     // invalid port
    }
    
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[name];

    return port->IDR;
}