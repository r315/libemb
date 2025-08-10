#include <stdlib.h>
#include "stm32f103xb.h"
#include "gpio_stm32f1xx.h"

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
    volatile uint32_t *cr;
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    if ((port = getPort(name)) == NULL) {
        return;
    }

    cfg = GPIO_CFG_MASK(cfg);

    cr = (pin <  8) ? &port->CRL : &port->CRH;

    uint32_t tmp = (*cr & ~(15 << ((pin & 7) << 2)));

    if(cfg == GPI_PD){
        port->BRR = (1 << pin);
    }else if(cfg == GPI_PU){
        port->BSRR = (1 << pin);
        cfg &= ~(1 << 2);
    }

    *cr = tmp | (cfg << ((pin & 7) << 2));
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