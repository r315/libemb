#ifndef _gpio_stm32l4xx_h_
#define _gpio_stm32l4xx_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    PA_0  = 0x00,
    PA_1  = 0x01,
    PA_2  = 0x02,
    PA_3  = 0x03,
    PA_4  = 0x04,
    PA_5  = 0x05,
    PA_6  = 0x06,
    PA_7  = 0x07,
    PA_8  = 0x08,
    PA_9  = 0x09,
    PA_10 = 0x0A,
    PA_11 = 0x0B,
    PA_12 = 0x0C,
    PA_13 = 0x0D,
    PA_14 = 0x0E,
    PA_15 = 0x0F,

    PB_0  = 0x10,
    PB_1  = 0x11,
    PB_2  = 0x12,
    PB_3  = 0x13,
    PB_4  = 0x14,
    PB_5  = 0x15,
    PB_6  = 0x16,
    PB_7  = 0x17,
    PB_8  = 0x18,
    PB_9  = 0x19,
    PB_10 = 0x1A,
    PB_11 = 0x1B,
    PB_12 = 0x1C,
    PB_13 = 0x1D,
    PB_14 = 0x1E,
    PB_15 = 0x1F,

    PC_0  = 0x20,
    PC_1  = 0x21,
    PC_2  = 0x22,
    PC_3  = 0x23,
    PC_4  = 0x24,
    PC_5  = 0x25,
    PC_6  = 0x26,
    PC_7  = 0x27,
    PC_8  = 0x28,
    PC_9  = 0x29,
    PC_10 = 0x2A,
    PC_11 = 0x2B,
    PC_12 = 0x2C,
    PC_13 = 0x2D,
    PC_14 = 0x2E,
    PC_15 = 0x2F,

// Arduino pins
    D0 = PA_10,
    D1 = PA_9,
    D2 = PA_12,
    D3 = PB_0,
    D4 = PB_7,
    D5 = PB_6,
    D6 = PB_1,
    D7 = PC_14,
    D8 = PC_15,
    D9 = PA_8,
    D10 = PA_11,
    D11 = PB_5,
    D12 = PB_4,
    D13 = PB_3,

    A0 = PA_0,
    A1 = PA_1,
    A2 = PA_3,
    A3 = PA_4,
    A4 = PA_5,
    A5 = PA_6,
    A6 = PA_7,
    A7 = PA_2,

    LD3 = PB_3
}pinName_e;

#define PORTA   PA_0
#define PORTB   PB_0
#define PORTC   PC_0
#define PORTD   PD_0


/**
 * Pin configuration
 * 
 * OD[0] | PULL[1:0] | SPEED[1:0] | MODE[1:0]
 * 
 * MODE[1:0]    0: Input, 1: Output, 2: AF, 3: AN
 * SPEED[1:0]   0: LS, 1: MS, 2: HS, 3: VHS
 * PULL[0]      0: Float, 1: PU, 2: PD, 3:Reserved
 * OD[0]        0: PP, 1: OD
 * 
 * */
#define GPIO_IOM_IN                 (0 << 0)
#define GPIO_IOM_PP                 (1 << 0)
#define GPIO_IOM_AF                 (2 << 0)
#define GPIO_IOM_AN                 (3 << 0)

#define GPIO_IOS_LS                 (0 << 2)
#define GPIO_IOS_MS                 (1 << 2)
#define GPIO_IOS_HS                 (2 << 2)
#define GPIO_IOS_VHS                (3 << 2)

#define GPIO_IOP_FL                 (0 << 4)
#define GPIO_IOP_PU                 (1 << 4)
#define GPIO_IOP_PD                 (2 << 4)

#define GPIO_IOT_PP                 (0 << 6)
#define GPIO_IOT_OD                 (1 << 6)

#define GPI_ANALOG                  (GPIO_IOM_AN)
#define GPI_FLOAT                   (GPIO_IOP_FL | GPIO_IOM_IN)
#define GPI_PU                      (GPIO_IOP_PU | GPIO_IOM_IN)
#define GPI_PD                      (GPIO_IOP_PD | GPIO_IOM_IN)

#define GPO_LS                      (GPIO_IOS_LS | GPIO_IOM_PP)
#define GPO_MS                      (GPIO_IOS_MS | GPIO_IOM_PP)
#define GPO_HS                      (GPIO_IOS_HS | GPIO_IOM_PP)
#define GPO_VHS                     (GPIO_IOS_VHS | GPIO_IOM_PP)
#define GPO_LS_AF                   (GPIO_IOS_LS | GPIO_IOM_AF)
#define GPO_MS_AF                   (GPIO_IOS_MS | GPIO_IOM_AF)
#define GPO_HS_AF                   (GPIO_IOS_HS | GPIO_IOM_AF)
#define GPO_VHS_AF                  (GPIO_IOS_VHS | GPIO_IOM_AF)

#define GPIO_PIN_FAST_RESET(_port, _pin) _port->BRR = (1 << _pin)
#define GPIO_PIN_FAST_SET(_port, _pin)   _port->BSRR = (1 << _pin)

#define GPIO_NAME_TO_PORT(name)          (name >> 4)
#define GPIO_NAME_TO_PIN(name)           (name & 0x0f)

#define GPIO_CFG_MODE_MASK(cfg)     (cfg & 3)
#define GPIO_CFG_SPEED_MASK(cfg)    ((cfg >> 2) & 3)
#define GPIO_CFG_PULL_MASK(cfg)     ((cfg >> 4) & 3)
#define GPIO_CFG_TYPE_MASK(cfg)     (cfg & GPIO_IOT_OD)
#define GPIO_CFG_FUNC_MASK(cfg)     ((cfg >> 8) & 0xF)

#define GPIO_AF_SPI1_SPI2           ((5 << 8) | GPO_HS_AF)

void GPIO_PORT_Write(pinName_e name, uint32_t value);
uint32_t GPIO_PORT_Read(pinName_e name);

#ifdef __cplusplus
}
#endif

#endif
