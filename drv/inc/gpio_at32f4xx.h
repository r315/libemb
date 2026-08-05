#ifndef _at32f4xx_h_
#define _at32f4xx_h_

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

// Mode Option
#define GPIO_IOM_IN         0 // Input
#define GPIO_IOM_LARGE      1 // Large output source
#define GPIO_IOM_NOR        2 // Normal output source
#define GPIO_IOM_MAX        3 // Maximum output source
// Configuration option input
#define GPIO_IOF_AN         (0 << 2)
#define GPIO_IOF_FLT        (1 << 2)
#define GPIO_IOF_PD         (2 << 2)
#define GPIO_IOF_PU         (3 << 2)
// Configuration option output
#define GPIO_IOF_PP         (0 << 2)
#define GPIO_IOF_OD         (1 << 2)
#define GPIO_IOF_AF         (2 << 2)
#define GPIO_IOF_AF_OD      (3 << 2)

#define GPI_ANALOG          (GPIO_IOM_IN)
#define GPI_FLOAT           (GPIO_IOF_FLT | GPIO_IOM_IN)
#define GPI_PD              (GPIO_IOF_PD | GPIO_IOM_IN)
#define GPI_PU              (GPIO_IOF_PU | GPIO_IOM_IN)
#define GPIO_RESET_STATE    GPI_FLOAT

#define GPO_LS              (GPIO_IOM_NOR)
#define GPO_MS              (GPIO_IOM_LARGE)
#define GPO_HS              (GPIO_IOM_MAX)
#define GPO_LS_OD           (GPIO_IOF_OD | GPIO_IOM_NOR)
#define GPO_MS_OD           (GPIO_IOF_OD | GPIO_IOM_LARGE)
#define GPO_HS_OD           (GPIO_IOF_OD | GPIO_IOM_MAX)
#define GPO_LS_AF           (GPIO_IOF_AF | GPIO_IOM_NOR)
#define GPO_MS_AF           (GPIO_IOF_AF | GPIO_IOM_LARGE)
#define GPO_HS_AF           (GPIO_IOF_AF | GPIO_IOM_MAX)
#define GPO_LS_AF_OD        (GPIO_IOF_AF_OD | GPIO_IOM_NOR)
#define GPO_MS_AF_OD        (GPIO_IOF_AF_OD | GPIO_IOM_LARGE)
#define GPO_HS_AF_OD        (GPIO_IOF_AF_OD | GPIO_IOM_MAX)
#define GPO_PP              GPO_LS


#define GPIO_NAME_TO_PORT(name)         ((name >> 4) & 15)
#define GPIO_NAME_TO_PIN(name)          (name & 15)
#define GPIO_CTRL_MASK(cfg)             (cfg & 15)
#define GPIO_CTRL_MODE_MASK(cfg)        (cfg & 3)
#define GPIO_CTRL_FUNC_MASK(cfg)        ((cfg >> 2) & 3)
#define GPIO_ANIN_TO_PIN_NAME(ANIN)     ((ANIN < 8) ? ANIN & 7 : PB_0 + (ANIN & 1))

#define GPIO_SPI1_CS        (GPO_MS_AF)
#define GPIO_SPI1_SCK       (GPO_HS_AF)
#define GPIO_SPI1_MOSI      (GPO_HS_AF)
#define GPIO_SPI1_MISO      (GPO_HS_AF)
#define GPIO_SPI2_CS        (GPO_MS_AF)
#define GPIO_SPI2_SCK       (GPO_HS_AF)
#define GPIO_SPI2_MOSI      (GPO_HS_AF)
#define GPIO_SPI2_MISO      (GPO_HS_AF)
#define GPIO_USART1_TX      (GPO_MS_AF)
#define GPIO_USART1_RX      (GPI_FLOAT)
#define GPIO_USART2_TX      (GPO_MS_AF)
#define GPIO_USART2_RX      (GPO_MS_AF)
#define GPIO_USART3_TX      (GPO_MS_AF)
#define GPIO_USART3_RX      (GPO_MS_AF)
#define GPIO_SPI1_SD        (GPO_MS_AF)
#define GPIO_SPI1_WS        (GPO_MS_AF)
#define GPIO_SPI1_CK        (GPO_MS_AF)
#define GPIO_I2C1_SCL       (GPO_MS_AF_OD)
#define GPIO_I2C1_SDA       (GPO_MS_AF_OD)


/**
 * IOMUX | REMAP | BIT  | GMUX | IOF | IOM
 *   16  | 15:13 | 12:8 |  6:4 | 3:2 | 1:0
 *  */
#define GPIO_IOMUX_CFG_Pos              16
#define GPIO_IOMUX_REMAP_Pos            13
#define GPIO_IOMUX_BIT_Pos              8
#define GPIO_IOMUX_GMUX_Pos             4
#define GPIO_IOMUX_CFG                  (1 << GPIO_IOMUX_CFG_Pos)
#define GPIO_IOMUX_REGMAP(cfg)          ((cfg >> GPIO_IOMUX_REMAP_Pos) & 7) /* Offset starting from &AFIO->MAP */
#define GPIO_IOMUX_GMUX(cfg)            ((cfg >> GPIO_IOMUX_GMUX_Pos) & 15)
#define GPIO_IOMUX_BIT(cfg)             ((cfg >> GPIO_IOMUX_BIT_Pos) & 31)

#define TMR9_GMUX_PB14_PB15             (GPIO_IOMUX_CFG | (7 << GPIO_IOMUX_REMAP_Pos) | (0 << GPIO_IOMUX_BIT_Pos) | (2 << GPIO_IOMUX_GMUX_Pos))
#define TMR11_GMUX_PA7                  (GPIO_IOMUX_CFG | (7 << GPIO_IOMUX_REMAP_Pos) | (8 << GPIO_IOMUX_BIT_Pos) | (2 << GPIO_IOMUX_GMUX_Pos))

#define GPIO_AF_TMR9_CH2                (TMR9_GMUX_PB14_PB15 | GPO_HS_AF)
#define GPIO_AF_TMR11_CH1               (TMR11_GMUX_PA7 | GPO_HS_AF)

#ifdef __cplusplus
}
#endif

#endif
