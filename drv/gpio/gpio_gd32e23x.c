#include <stdlib.h>
#include "gd32e23x.h"
#include "gpio_gd32e23x.h"

static const uint32_t ports[] = {GPIOA, GPIOB, GPIOC, 0, 0, GPIOF};

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

