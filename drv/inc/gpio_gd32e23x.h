#ifndef _gd32e23xx_h_
#define _gd32e23x_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    PA_0 = 0,
    PA_1,
    PA_2,
    PA_3,
    PA_4,
    PA_5,
    PA_6,
    PA_7,
    PA_8,
    PA_9,
    PA_10,
    PA_11,
    PA_12,
    PA_13,
    PA_14,
    PA_15,

    PB_0,
    PB_1,
    PB_2,
    PB_3,
    PB_4,
    PB_5,
    PB_6,
    PB_7,
    PB_8,
    PB_9,
    PB_10,
    PB_11,
    PB_12,
    PB_13,
    PB_14,
    PB_15,

    PC_0,
    PC_1,
    PC_2,
    PC_3,
    PC_4,
    PC_5,
    PC_6,
    PC_7,
    PC_8,
    PC_9,
    PC_10,
    PC_11,
    PC_12,
    PC_13,
    PC_14,
    PC_15,

    PD_0,
    PD_1,
    PD_2,
    PD_3,
    PD_4,
    PD_5,
    PD_6,
    PD_7,
    PD_8,
    PD_9,
    PD_10,
    PD_11,
    PD_12,
    PD_13,
    PD_14,
    PD_15,

    PE_0,
    PE_1,
    PE_2,
    PE_3,
    PE_4,
    PE_5,
    PE_6,
    PE_7,
    PE_8,
    PE_9,
    PE_10,
    PE_11,
    PE_12,
    PE_13,
    PE_14,
    PE_15,

    PF_0,
    PF_1,
    PF_2,
    PF_3,
    PF_4,
    PF_5,
    PF_6,
    PF_7,
    PF_8,
    PF_9,
    PF_10,
    PF_11,
    PF_12,
    PF_13,
    PF_14,
    PF_15,
}pinName_e;

#define GPIO_NAME_TO_PORT(name)     (name >> 4)
#define GPIO_NAME_TO_PIN(name)      (name & 15)

#define GPIO_CFG_MASK(cfg)          (cfg & 0x7F)
#define GPIO_CFG_MASK_MODE(cfg)     ((cfg >> 0) & 3)
#define GPIO_CFG_MASK_SPEED(cfg)    ((cfg >> 2) & 3)
#define GPIO_CFG_MASK_PUPD(cfg)     ((cfg >> 4) & 3)
#define GPIO_CFG_MASK_TYPE(cfg)     ((cfg >> 6) & 1)

//Pin modes
#define GPIO_IOM_INPUT              (0 << 0)
#define GPIO_IOM_OUTPUT             (1 << 0)
#define GPIO_IOM_AF                 (2 << 0)
#define GPIO_IOM_ANALOG             (3 << 0)
// Pin type
#define GPIO_TYPE_PP                (0 << 6)
#define GPIO_TYPE_OD                (1 << 6)
// Pin speed
#define GPIO_SPEED_2M               (0 << 2)
#define GPIO_SPEED_10M              (2 << 2)
#define GPIO_SPEED_50M              (3 << 2)
// Pin pull
//#define GPIO_PUPD_NONE              (0 << 4)
#define GPIO_PUPD_PU                (1 << 4)
#define GPIO_PUPD_PD                (2 << 4)

#define GPO_LS                      (GPIO_IOM_OUTPUT | GPIO_SPEED_2M | GPIO_TYPE_PP)
#define GPO_MS                      (GPIO_IOM_OUTPUT | GPIO_SPEED_10M | GPIO_TYPE_PP)
#define GPO_HS                      (GPIO_IOM_OUTPUT | GPIO_SPEED_50M | GPIO_TYPE_PP)
#define GPO_LS_OD                   (GPIO_IOM_OUTPUT | GPIO_SPEED_2M | GPIO_TYPE_OD)
#define GPO_MS_OD                   (GPIO_IOM_OUTPUT | GPIO_SPEED_10M | GPIO_TYPE_OD)
#define GPO_HS_OD                   (GPIO_IOM_OUTPUT | GPIO_SPEED_50M | GPIO_TYPE_OD)

#define GPI_LS                      (GPIO_IOM_INPUT | GPIO_SPEED_2M | GPIO_TYPE_PP)
#define GPI_MS                      (GPIO_IOM_INPUT | GPIO_SPEED_10M | GPIO_TYPE_PP)
#define GPI_HS                      (GPIO_IOM_INPUT | GPIO_SPEED_50M | GPIO_TYPE_PP)
#define GPI_LS_OD                   (GPIO_IOM_INPUT | GPIO_SPEED_2M | GPIO_TYPE_OD)
#define GPI_MS_OD                   (GPIO_IOM_INPUT | GPIO_SPEED_10M | GPIO_TYPE_OD)
#define GPI_HS_OD                   (GPIO_IOM_INPUT | GPIO_SPEED_50M | GPIO_TYPE_OD)
#define GPI_ANALOG                  (GPIO_IOM_ANALOG)

#define GPIO_AF0                    ((uint8_t)0x00)
#define GPIO_AF1                    ((uint8_t)0x01)
#define GPIO_AF2                    ((uint8_t)0x02)
#define GPIO_AF3                    ((uint8_t)0x03)
#define GPIO_AF4                    ((uint8_t)0x04)
#define GPIO_AF5                    ((uint8_t)0x05)
#define GPIO_AF6                    ((uint8_t)0x06)
#define GPIO_AF7                    ((uint8_t)0x07)

#ifdef __cplusplus
}
#endif

#endif
