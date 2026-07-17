#include <stdlib.h>
#include "stm32f103xb.h"
#include "gpio_stm32f1xx.h"

typedef void(*gpio_int_handler_t)(void);

static const uint32_t ports[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE, GPIOE_BASE};
static gpio_int_handler_t gpio_int_handler[15];

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

void GPIO_AttachInt(pinName_e name, uint8_t edge, void(*cb)(void))
{
    GPIO_TypeDef *port = getPort(name);

    if(cb == NULL || port == NULL){
        return;
    }

    uint8_t pin = GPIO_NAME_TO_PIN(name);
    uint32_t port_idx = GPIO_NAME_TO_PORT(name);
    uint32_t offset = pin >> 2;
    uint32_t shift = (pin & 3) << 2;

    AFIO->EXTICR[offset] |= (port_idx << shift); // Assign int to pin

    if(edge & 1){
        EXTI->RTSR |= (1 << pin);
    }

    if(edge & 2){
        EXTI->FTSR |= (1 << pin);
    }

    gpio_int_handler[pin] = cb;

    EXTI->IMR  |= (1 << pin);

    IRQn_Type irqn;

    if(pin < 5){
        irqn = EXTI0_IRQn + pin;
    }else if(pin < 10){
        irqn = EXTI9_5_IRQn;
    }else{
        irqn = EXTI15_10_IRQn;
    }

    NVIC_EnableIRQ(irqn);
}

void GPIO_RemoveInt(uint32_t name)
{
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    EXTI->EMR  &= ~(1 << pin);
}

static void exti_handler(void)
{
    uint32_t pnd = EXTI->PR;

    for(int i = 5; i < 16; i++){
        if(pnd & (1 << i)){
            gpio_int_handler[i]();
        }
    }

    EXTI->PR = pnd;
}

void EXTI0_IRQHandler(void){gpio_int_handler[0]();  EXTI->PR = 1;}
void EXTI1_IRQHandler(void){gpio_int_handler[1]();  EXTI->PR = 2;}
void EXTI2_IRQHandler(void){gpio_int_handler[2]();  EXTI->PR = 4;}
void EXTI3_IRQHandler(void){gpio_int_handler[3]();  EXTI->PR = 8;}
void EXTI4_IRQHandler(void){gpio_int_handler[4]();  EXTI->PR = 16;}
void EXTI9_5_IRQHandler(void){ exti_handler(); }
void EXTI15_10_IRQHandler(void){ exti_handler(); }