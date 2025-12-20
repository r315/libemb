#include <stdlib.h>
#include "at32f4xx.h"
#include "gpio_at32f4xx.h"

#if !defined (AT32F403xx)
#define GPIOE_BASE 0
#endif

typedef void(*gpio_int_handler_t)(void);

static const uint32_t ports[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE, GPIOE_BASE, GPIOF_BASE};
static gpio_int_handler_t gpio_int_handler[15];

static uint32_t exti_get_irqn(uint8_t pin)
{
    if(pin < 5){
        return EXTI0_IRQn + pin;
    }else if(pin < 10){
        return EXTI9_5_IRQn;
    }

    return EXTI15_10_IRQn;
}

static GPIO_Type *getPort(pinName_e name)
{
    uint8_t pidx = GPIO_NAME_TO_PORT(name);
    if(pidx > sizeof(ports)/sizeof(ports[0])){
        return NULL;
    }

    return (GPIO_Type*)ports[pidx];
}

/**
 * @brief
 *
 * @param name
 * @param mode
 */
void GPIO_Config(pinName_e name, uint32_t cfg) {
    GPIO_Type *port;
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    if ((port = getPort(name)) == NULL) {
        return;
    }

    cfg = GPIO_CFG_MASK(cfg);

    if(cfg == GPI_PD){
        port->BRE = (1 << pin);  // Activate pull-down
    }else if(cfg == GPI_PU){
        port->BSRE = (1 << pin); // Activate pull-up
        cfg &= ~GPIO_IOF_FLT;    // clear reserved bit
    }

    if(pin <  8){
        port->CTRLL = (port->CTRLL & ~(15 << (pin << 2))) | (cfg << (pin << 2));
    }else{
        port->CTRLH = (port->CTRLH & ~(15 << ((pin - 8) << 2))) | (cfg << ((pin - 8) << 2));
    }
}

/**
 * @brief
 *
 * @param name
 * @return uint32_t
 */
uint32_t GPIO_Read(pinName_e name){
    GPIO_Type *port;
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    if ((port = getPort(name)) == NULL) {
        return 0xFFFFFFFF;
    }

    return !!(port->IPTDT & (1 << pin));
}

/**
 * @brief
 *
 * @param name
 * @param state
 */
void GPIO_Write(pinName_e name, uint32_t state){
    GPIO_Type *port;
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    if ((port = getPort(name)) == NULL) {
        return;
    }

    port->BSRE = (state != 0)? (1 << pin) : (0x10000 << pin);
}

/**
 * @brief
 *
 * @param name
 */
void GPIO_Toggle(pinName_e name){
    GPIO_Type *port;
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    if ((port = getPort(name)) == NULL) {
        return;
    }

    port->OPTDT ^= (1 << pin);
}

/**
 * @brief
 * @param name
 * @param edge
 * @param cb
 */
void GPIO_AttachInt(pinName_e name, uint8_t edge, void(*cb)(void))
{
    uint8_t pin = GPIO_NAME_TO_PIN(name);
    GPIO_Type *port = getPort(name);

    if(cb == NULL || port == NULL){
        return;
    }

    uint32_t port_idx = GPIO_NAME_TO_PORT(name);
    uint32_t offset = pin >> 2;
    uint32_t shift = (pin & 3) << 2;

    AFIO->EXTIC[offset] |= (port_idx << shift); // Assign int to pin

    if(edge & 1){
        EXTI->RTRSEL |= (1 << pin);
    }

    if(edge & 2){
        EXTI->FTRSEL |= (1 << pin);
    }

    gpio_int_handler[pin] = cb;

    EXTI->INTEN  |= (1 << pin);

    NVIC_EnableIRQ(exti_get_irqn(pin));
}

void GPIO_RemoveInt(uint32_t name)
{
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    EXTI->INTEN  &= ~(1 << pin);
}

static void exti_handler(void)
{
    uint32_t pnd = EXTI->PND;

    for(int i = 5; i < 16; i++){
        if(pnd & (1 << i)){
            gpio_int_handler[i]();
        }
    }

    EXTI->PND = pnd;
}

void EXTI0_IRQHandler(void){gpio_int_handler[0]();  EXTI->PND = 1;}
void EXTI1_IRQHandler(void){gpio_int_handler[1]();  EXTI->PND = 2;}
void EXTI2_IRQHandler(void){gpio_int_handler[2]();  EXTI->PND = 4;}
void EXTI3_IRQHandler(void){gpio_int_handler[3]();  EXTI->PND = 8;}
void EXTI4_IRQHandler(void){gpio_int_handler[4]();  EXTI->PND = 16;}
void EXTI9_5_IRQHandler(void){ exti_handler(); }
void EXTI15_10_IRQHandler(void){ exti_handler(); }