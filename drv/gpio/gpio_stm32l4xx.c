#include "stm32l4xx.h"
#include "gpio_stm32l4xx.h"

static const uint32_t ports[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE};

/**
 * @brief Configure GPIO pin
 * @param port : pin port
 * @param pin : pin number 0 - 15
 * @param mode : See macros
 * 
 * */
void GPIO_Config(pinName_e name, uint8_t mode) {
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[GPIO_NAME_TO_PORT(name)];
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    uint32_t tmp = mode & 3;
    port->MODER = (port->MODER & ~(3 << (pin << 1))) | (tmp << (pin << 1));

    tmp = !!(mode & (1 << 6));
    port->OTYPER = (port->OTYPER & ~(1 << pin)) | (tmp << pin);
    
    tmp = (mode >> 2) & 3;
    port->OSPEEDR = (port->OSPEEDR & ~(3 << (pin << 1))) | (tmp << (pin << 1));

    tmp = (mode >> 4) & 3;
    port->PUPDR = (port->PUPDR & ~(3 << (pin << 1))) | (tmp << (pin << 1));
}

void GPIO_Function(uint32_t name, uint32_t func)
{
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[GPIO_NAME_TO_PORT(name)];
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    uint32_t tmp = func & 15;
    if(pin < 8){
        pin <<= 2;
        port->AFR[0] = (port->AFR[0] & ~(15 << pin)) | (tmp << pin);
    }else{
        pin = (pin & 7) << 2;
        port->AFR[1] = (port->AFR[1] & ~(15 << pin)) | (tmp << pin);
    }
}

uint32_t GPIO_Read(uint32_t name){
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[GPIO_NAME_TO_PORT(name)];
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    return !!(port->IDR & (1 << pin));
}

void GPIO_Write(pinName_e name, uint8_t state){
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[GPIO_NAME_TO_PORT(name)];
    uint8_t pin = GPIO_NAME_TO_PIN(name);
    port->BSRR = (state != 0)? (1<<pin) : (1<<(pin+16));
}

void GPIO_Toggle(pinName_e name){
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[GPIO_NAME_TO_PORT(name)];
    uint8_t pin = GPIO_NAME_TO_PIN(name);
    port->ODR = port->IDR ^ (1<<pin);
}

void GPIO_PORT_Write(pinName_e name, uint32_t value){
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[GPIO_NAME_TO_PORT(name)];
    port->ODR = value;
}
uint32_t GPIO_PORT_Read(pinName_e name){
    GPIO_TypeDef *port = (GPIO_TypeDef*)ports[GPIO_NAME_TO_PORT(name)];
    return port->IDR;
}