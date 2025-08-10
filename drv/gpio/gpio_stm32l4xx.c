#include <stdlib.h>
#include "stm32l4xx.h"
#include "gpio_stm32l4xx.h"

static const uint32_t ports[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE};

static GPIO_TypeDef *getPort(pinName_e name)
{
    uint8_t pidx = GPIO_NAME_TO_PORT(name);
    if(pidx > sizeof(ports)/sizeof(ports[0])){
        return NULL;
    }

    return (GPIO_TypeDef*)ports[pidx];
}

/**
 * @brief Configure GPIO pin
 * @param port : pin port
 * @param pin : pin number 0 - 15
 * @param mode : See macros
 * 
 * */
void GPIO_Config(pinName_e name, uint32_t cfg) {
    GPIO_TypeDef *port;
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    if ((port = getPort(name)) == NULL) {
        return;
    }

    uint32_t tmp = GPIO_CFG_MODE_MASK(cfg);
    port->MODER = (port->MODER & ~(3 << (pin << 1))) | (tmp << (pin << 1));

    tmp = !!GPIO_CFG_TYPE_MASK(cfg);
    port->OTYPER = (port->OTYPER & ~(1 << pin)) | (tmp << pin);
    
    tmp = GPIO_CFG_SPEED_MASK(cfg);
    port->OSPEEDR = (port->OSPEEDR & ~(3 << (pin << 1))) | (tmp << (pin << 1));

    tmp = GPIO_CFG_PULL_MASK(cfg);
    port->PUPDR = (port->PUPDR & ~(3 << (pin << 1))) | (tmp << (pin << 1));

    tmp =  GPIO_CFG_FUNC_MASK(cfg);
    if(pin < 8){
        pin <<= 2;
        port->AFR[0] = (port->AFR[0] & ~(15 << pin)) | (tmp << pin);
    }else{
        pin = (pin & 7) << 2;
        port->AFR[1] = (port->AFR[1] & ~(15 << pin)) | (tmp << pin);
    }
}

uint32_t GPIO_Read(pinName_e name){
    GPIO_TypeDef *port;
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    if ((port = getPort(name)) == NULL) {
        return 0xFFFFFFFF;
    }

    return !!(port->IDR & (1 << pin));
}

void GPIO_Write(pinName_e name, uint32_t state){
    GPIO_TypeDef *port;
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    if ((port = getPort(name)) == NULL) {
        return;
    }

    port->BSRR = (state != 0)? (1 << pin) : (0x10000 << pin);
}

void GPIO_Toggle(pinName_e name){
    GPIO_TypeDef *port;
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    if ((port = getPort(name)) == NULL) {
        return;
    }

    port->ODR = port->IDR ^ (1<<pin);
}

void GPIO_PORT_Write(pinName_e name, uint32_t value){
    GPIO_TypeDef *port;

    if ((port = getPort(name)) == NULL) {
        return;
    }
    
    port->ODR = value;
}
uint32_t GPIO_PORT_Read(pinName_e name){
    GPIO_TypeDef *port;

    if ((port = getPort(name)) == NULL) {
        return 0;
    }
    
    return port->IDR;
}