#include <stdlib.h>
#include "gd32e23x.h"
#include "gpio_gd32e23x.h"

typedef void(*gpio_int_handler_t)(void);
static const uint32_t ports[] = {GPIOA, GPIOB, GPIOC, 0, 0, GPIOF};
static gpio_int_handler_t gpio_int_handler[16];

void GPIO_Config(pinName_e name, uint8_t cfg)
{
    uint32_t port = ports[GPIO_NAME_TO_PORT(name)];
    uint8_t pin = GPIO_NAME_TO_PIN(name);
    if (port == 0) { return; }

    gpio_mode_set(port, GPIO_CFG_MASK_MODE(cfg), GPIO_CFG_MASK_PUPD(cfg), (1 << pin));
    gpio_output_options_set(port, GPIO_CFG_MASK_TYPE(cfg), GPIO_CFG_MASK_SPEED(cfg), (1 << pin));
}

void GPIO_Function(uint32_t name, uint32_t func)
{
    uint32_t port = ports[GPIO_NAME_TO_PORT(name)];
    uint8_t pin = GPIO_NAME_TO_PIN(name);
    if (port == 0) { return; }

    gpio_mode_set(port, GPIO_MODE_AF, GPIO_PUPD_NONE, (1 << pin));
    gpio_af_set(port, func, (1 << pin));
}

void GPIO_Write(pinName_e name, uint8_t state)
{
    uint32_t port = ports[GPIO_NAME_TO_PORT(name)];
    uint8_t pin = GPIO_NAME_TO_PIN(name);
    if (port == 0) { return; }

    GPIO_BOP(port) = (state != 0)? (1 << pin) : (0x10000 << pin);
}

void GPIO_Toggle(pinName_e name)
{
    uint32_t port = ports[GPIO_NAME_TO_PORT(name)];
    uint8_t pin = GPIO_NAME_TO_PIN(name);
    if (port == 0) { return; }

    uint16_t port_value = GPIO_OCTL(port);

    GPIO_OCTL(port) = port_value ^ (1 << pin);
}


/**
 * @brief
 * @param name
 * @param edge
 * @param cb
 */
void GPIO_AttachInt(pinName_e name, uint8_t edge, void(*cb)(void))
{
    if(cb == NULL){
        return;
    }

    uint8_t pin = GPIO_NAME_TO_PIN(name);
    uint32_t port_idx = GPIO_NAME_TO_PORT(name);
    uint32_t offset = pin >> 2;
    uint32_t shift = (pin & 3) << 2;

    // Select pin port to exti
    uint32_t *exti_sel = (uint32_t*)(SYSCFG + 0x08);
    exti_sel[offset] |= (port_idx << shift);

    if(edge & 1){
        EXTI_RTEN |= (1 << pin);
    }

    if(edge & 2){
        EXTI_FTEN |= (1 << pin);
    }

    gpio_int_handler[pin] = cb;

    EXTI_INTEN  |= (1 << pin);

    if(pin < 2){
        NVIC_EnableIRQ(EXTI0_1_IRQn);
    }else if(pin < 4){
        NVIC_EnableIRQ(EXTI2_3_IRQn);
    }else{
        NVIC_EnableIRQ(EXTI4_15_IRQn);
    }
}

void GPIO_RemoveInt(pinName_e name)
{
    uint8_t pin = GPIO_NAME_TO_PIN(name);

    EXTI_INTEN  &= ~(1 << pin);
}

static void extiHandler(uint8_t start, uint8_t end)
{
    for(uint8_t i = start; i <= end; i++){
        if(EXTI_PD & (1 << i)){
            gpio_int_handler[i]();
            EXTI_PD = 1 << i;
        }
    }
}

void EXTI0_1_IRQHandler(void) { extiHandler(0, 1); }
void EXTI2_3_IRQHandler(void) { extiHandler(2, 3); }
void EXTI4_15_IRQHandler(void){ extiHandler(4, 15); }

