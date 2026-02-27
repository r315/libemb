#ifndef _pinName_h_
#define _pinName_h_

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

    PD_0  = 0x30,
    PD_1  = 0x31,
    PD_2  = 0x32,

    /**** USB pins ****/
    USB_DM = PA_11,
    USB_DP = PA_12,

    /**** OSCILLATOR pins ****/
    RCC_OSC32_IN = PC_14,
    RCC_OSC32_OUT = PC_15,
    RCC_OSC_IN = PD_0,
    RCC_OSC_OUT = PD_1,

    /**** DEBUG pins ****/
    SYS_JTCK_SWCLK = PA_14,
    SYS_JTDI = PA_15,
    SYS_JTDO_TRACESWO = PB_3,
    SYS_JTMS_SWDIO = PA_13,
    SYS_NJTRST = PB_4,
    SYS_WKUP = PA_0

}pinName_e;

#define PORTA   PA_0
#define PORTB   PB_0
#define PORTC   PC_0
#define PORTD   PD_0

#define GPIO_NAME_TO_PORT(name)     (name >> 4)
#define GPIO_NAME_TO_PIN(name)      (name & 15)

#define GPIO_CFG_MASK(cfg)          (cfg & 0x1F)
#define GPIO_CFG_MASK_MODE(cfg)     (cfg & 3)
#define GPIO_CFG_MASK_TYPE(cfg)     ((cfg >> 2) & 1)

//Pin modes
#define GPIO_IOM_INPUT              0
#define GPIO_IOM_GPIO               1
#define GPIO_IOM_AF                 2
#define GPIO_IOM_ANALOG             3
// Pin type
#define GPIO_TYPE_PP                0
#define GPIO_TYPE_OD                (1 << 2)
// Pin function (output)
#define GPIO_IOF_PP                 (0 << 2)
#define GPIO_IOF_OD                 (1 << 2)
#define GPIO_IOF_AF                 (2 << 2)
#define GPIO_IOF_AF_OD              (3 << 2)
// Pin function (input)
#define GPIO_IOF_AN                 (0 << 2)
#define GPIO_IOF_FLT                (1 << 2)
#define GPIO_IOF_PD                 (2 << 2)
#define GPIO_IOF_PU                 (3 << 2)

#define GPO_LS                      (GPIO_IOM_GPIO)
#define GPO_MS                      (GPIO_IOM_GPIO)
#define GPO_HS                      (GPIO_IOM_GPIO)
#define GPO_LS_OD                   (GPIO_IOM_GPIO | GPIO_TYPE_OD)
#define GPO_MS_OD                   (GPIO_IOM_GPIO | GPIO_TYPE_OD)
#define GPO_HS_OD                   (GPIO_IOM_GPIO | GPIO_TYPE_OD)

#define GPI_ANALOG                  (GPIO_IOM_ANALOG)


void GPIO_PORT_Write(pinName_e name, uint32_t value);
uint32_t GPIO_PORT_Read(pinName_e name);

#ifdef __cplusplus
}
#endif

#endif
