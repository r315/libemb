#ifndef _gpio_h_
#define _gpio_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define GPIO_PIN_HIGH           1
#define GPIO_PIN_LOW            0
#define GPIO_INT_EDGE_RISING    (1 << 0)
#define GPIO_INT_EDGE_FALLING   (1 << 1)
#define GPIO_INT_EDGE_NONE      0
#define GPIO_INT_EDGE_BOTH      3


void GPIO_Config(uint32_t name, uint32_t cfg);
uint32_t GPIO_Read(uint32_t name);
void GPIO_Write(uint32_t name, uint32_t state);
void GPIO_Toggle(uint32_t name);
void GPIO_Function(uint32_t name, uint32_t func);
void GPIO_Mode(uint32_t name, uint32_t mode);
void GPIO_Direction(uint32_t name, uint32_t mode);
void GPIO_AttachInt(uint32_t name, uint8_t edge, void(*cb)(void));
void GPIO_RemoveInt(uint32_t name);

#ifdef __cplusplus
}
#endif

#endif