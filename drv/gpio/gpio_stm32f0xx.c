#include <stdlib.h>
#include "stm32f071xb.h"
#include "gpio_stm32f0xx.h"

static const uint32_t ports[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE, GPIOE_BASE};

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
 * @param mode : see header
 *
 * */
void GPIO_Config(pinName_e name, uint8_t cfg) {
    GPIO_TypeDef *port;
    uint8_t pin;

    if ((port = getPort(name)) == NULL) {
        return;
    }

    pin = GPIO_NAME_TO_PIN(name);
    uint8_t shift = (pin << 1);

    uint32_t tmp = port->MODER & ~(3 << shift);

    tmp |= (GPIO_CFG_MASK_MODE(cfg) << shift);

    port->MODER = tmp;
    if(GPIO_CFG_MASK_TYPE(cfg)){
        port->OTYPER |= 1 << pin;
    }else{
        port->OTYPER &= ~(1 << pin);
    }
}

/**
 * @brief Set alternative function of a pin
 * @param name
 * @param func
 */
void GPIO_Function(uint32_t name, uint32_t func)
{
    GPIO_TypeDef *port;
    uint8_t pin = GPIO_NAME_TO_PIN(name);
    volatile uint32_t *afr;
    uint8_t shift;

    if ((port = getPort(name)) == NULL) {
        return;
    }

    shift = (pin << 1);
    port->MODER = (port->MODER & ~(3 << shift)) | (GPIO_IOM_AF << shift);

    afr = (pin < 8) ? &port->AFR[0] : &port->AFR[1];

    shift = ((pin & 7) << 2);

    *afr = (*afr & ~(15 << shift)) | (func << shift);
}

uint32_t GPIO_Read(pinName_e name){
    GPIO_TypeDef *port;
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    if ((port = getPort(name)) == NULL) {
        return 0xFFFFFFFF;
    }

    return !!(port->IDR & (1 << pin));
}

void GPIO_Write(pinName_e name, uint8_t state){
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