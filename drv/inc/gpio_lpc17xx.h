#ifndef _pinName_h_
#define _pinName_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "LPC17xx.h"

#define GPIO_P0  0
#define GPIO_P1  1
#define GPIO_P2  2
#define GPIO_P3  3
#define GPIO_P4  4

#define GPIO_INT_LOW  1
#define GPIO_INT_HIGH 2

#define ON 1
#define OFF 0
#define GPIO_INPUT  0
#define GPIO_OUTPUT 1
#define GPIO_HIGH   1
#define GPIO_LOW    0


#define GPIO0 LPC_GPIO0
#define GPIO1 LPC_GPIO1
#define GPIO2 LPC_GPIO2
#define GPIO3 LPC_GPIO3
#define GPIO4 LPC_GPIO4


#define SETPIN GPIO0->FIOSET
#define CLRPIN GPIO0->FIOCLR
#define SETOUTPUT(x) GPIO0->FIODIR |= x
#define SETINPUT(x) GPIO0->FIODIR &= ~(x)

/* ****** PINSEL bit positions ****** */
typedef enum pinName{
  P0_0 = 0,
  P0_1,
  P0_2,
  P0_3,
  P0_4,
  P0_5,
  P0_6,
  P0_7,
  P0_8,
  P0_9,
  P0_10,
  P0_11,
  GPIO_RSV0,
  GPIO_RSV1,
  GPIO_RSV2,
  P0_15,
  P0_16,
  P0_17,
  P0_18,
  P0_19,
  P0_20,
  P0_21,
  P0_22,
  P0_23,
  P0_24,
  P0_25,
  P0_26,
  P0_27,
  P0_28,
  P0_29,
  P0_30,
  GPIO_RSV3,
  P1_0,
  P1_1,
  GPIO_RSV4,
  GPIO_RSV5,
  P1_4,
  GPIO_RSV6,
  GPIO_RSV7,
  GPIO_RSV8,
  P1_8,
  P1_9,
  P1_10,
  GPIO_RSV9,
  GPIO_RSV10,
  GPIO_RSV11,
  P1_14,
  P1_15,
  P1_16,
  P1_17,
  P1_18,
  P1_19,
  P1_20,
  P1_21,
  P1_22,
  P1_23,
  P1_24,
  P1_25,
  P1_26,
  P1_27,
  P1_28,
  P1_29,
  P1_30,
  P1_31,
  P2_0,
  P2_1,
  P2_2,
  P2_3,
  P2_4,
  P2_5,
  P2_6,
  P2_7,
  P2_8,
  P2_9,
  P2_10,
  P2_11,
  P2_12,
  P2_13,
  P3_25 = 121,
  P3_26 = 122,
  P4_28 = 156,
  P4_29 = 157
}pinName_e;

/** 
  * b7 | b6 | b5 | b4  | b3 b2 | b1 b0
  *         | od | dir | mode  | func
  */
#define PIN_FUNC_POS        0
#define PIN_MODE_POS        2
#define PIN_DIR_POS         4
#define PIN_OD_POS          5

#define PIN_FUNC0           0
#define PIN_FUNC1           1
#define PIN_FUNC2           2
#define PIN_FUNC3           3
#define PIN_MODE_PU         0
#define PIN_MODE_REPEATER   1
#define PIN_MODE_FLOAT      2
#define PIN_MODE_PD         3
#define PIN_MODE_PP         4
#define PIN_MODE_OD         8

#define PIN_FUNC_MASK       (3 << 0)
#define PIN_IN_PU           (PIN_MODE_PU << PIN_MODE_POS)
#define PIN_IN_REPEATER     (PIN_MODE_REPEATER << PIN_MODE_POS)
#define PIN_IN_FLOAT        (PIN_MODE_FLOAT << PIN_MODE_POS)
#define PIN_IN_PD           (PIN_MODE_PD << PIN_MODE_POS)
#define PIN_OUT_PP          (PIN_MODE_PP << PIN_MODE_POS)
#define PIN_OUT_OD          (PIN_MODE_OD << PIN_MODE_POS)

#define P0_4_I2SRX_CLK      PIN_FUNC1
#define P0_5_I2SRX_WS       PIN_FUNC1

#define P0_6_GPIO           PIN_FUNC0
#define P0_6_I2SRX_SDA      PIN_FUNC1
#define P0_6_SSEL1          PIN_FUNC2
#define P0_6_MAT2_0         PIN_FUNC3

#define P0_7_GPIO           PIN_FUNC0
#define P0_7_I2STX_CLK      PIN_FUNC1
#define P0_7_SCK1           PIN_FUNC2
#define P0_7_MAT2_1         PIN_FUNC3

#define P0_8_GPIO           PIN_FUNC0
#define P0_8_I2STX_WS       PIN_FUNC1
#define P0_8_MISO1          PIN_FUNC2
#define P0_8_MAT2_2         PIN_FUNC3

#define P0_9_GPIO           PIN_FUNC0
#define P0_9_I2STX_SDA      PIN_FUNC1
#define P0_9_MOSI1          PIN_FUNC2
#define P0_9_MA2_3          PIN_FUNC3

#define P0_15_GPIO          PIN_FUNC0
#define P0_15_TXD1          PIN_FUNC1
#define P0_15_SCK0          PIN_FUNC2
#define P0_15_SCK           PIN_FUNC3

#define P0_16_GPIO          PIN_FUNC0
#define P0_16_RXD1          PIN_FUNC1
#define P0_16_SSEL0         PIN_FUNC2
#define P0_16_SSEL          PIN_FUNC3

#define P0_17_GPIO          PIN_FUNC0
#define P0_17_CTS1          PIN_FUNC1
#define P0_17_MISO0         PIN_FUNC2
#define P0_17_MISO          PIN_FUNC3

#define P0_18_GPIO          PIN_FUNC0
#define P0_18_DCD1          PIN_FUNC1
#define P0_18_MOSI0         PIN_FUNC2
#define P0_18_MOSI          PIN_FUNC3

#define P0_23_I2SRX_CLK     PIN_FUNC2
#define P0_24_I2SRX_WS      PIN_FUNC2
#define P0_25_I2SRX_SDA     PIN_FUNC2

#define P0_23_GPIO          PIN_FUNC0
#define P0_23_AD0_0         PIN_FUNC1
#define P0_23_I2SRX_CLK     PIN_FUNC2
#define P0_23_CAP3_0        PIN_FUNC3

#define P0_24_GPIO          PIN_FUNC0
#define P0_24_AD0_2         PIN_FUNC1
#define P0_24_CAP3_1        PIN_FUNC3

//Port 1 pin functions
#define P1_20_GPIO          PIN_FUNC0
#define P1_20_MCI0          PIN_FUNC1
#define P1_20_PWM1_2        PIN_FUNC2
#define P1_20_SCK0          PIN_FUNC3

#define P1_21_GPIO          PIN_FUNC0
#define P1_21_MCABORT       PIN_FUNC1
#define P1_21_PWM1_3        PIN_FUNC2
#define P1_21_SSEL0         PIN_FUNC3

#define P1_22_GPIO          PIN_FUNC0
#define P1_22_MCOB0         PIN_FUNC1
#define P1_22_USB_PWRD      PIN_FUNC2
#define P1_22_MAT1_0        PIN_FUNC3

#define P1_23_GPIO          PIN_FUNC0
#define P1_23_MCI1          PIN_FUNC1
#define P1_23_PWM1_4        PIN_FUNC2
#define P1_23_MISO0         PIN_FUNC3

#define P1_24_GPIO          PIN_FUNC0
#define P1_24_MCI2          PIN_FUNC1
#define P1_24_PWM1_5        PIN_FUNC2
#define P1_24_MOSI0         PIN_FUNC3

//Port 2 pin functions
#define P2_0_GPIO           PIN_FUNC0
#define P2_0_PWM1_1         PIN_FUNC1
#define P2_0_TXD1           PIN_FUNC2

#define P2_1_GPIO           PIN_FUNC0
#define P2_1_PWM1_2         PIN_FUNC1
#define P2_1_RXD1           PIN_FUNC2

#define P2_2_GPIO           PIN_FUNC0
#define P2_2_PWM1_3         PIN_FUNC1
#define P2_2_CTS1           PIN_FUNC2

#define P2_3_GPIO           PIN_FUNC0
#define P2_3_PWM1_4         PIN_FUNC1
#define P2_3_DCD1           PIN_FUNC2

#define P2_4_GPIO           PIN_FUNC0
#define P2_4_PWM1_5         PIN_FUNC1
#define P2_4_DSR1           PIN_FUNC2

#define P2_5_GPIO           PIN_FUNC0
#define P2_5_PWM1_6         PIN_FUNC1
#define P2_5_DTR1           PIN_FUNC2

#define P2_6_GPIO           PIN_FUNC0
#define P2_6_PACP1_0        PIN_FUNC1
#define P2_6_RI1            PIN_FUNC2

#define P2_7_GPIO           PIN_FUNC0
#define P2_7_RD2            PIN_FUNC1
#define P2_7_RTS1           PIN_FUNC2

#define P2_11_I2STX_CLK     PIN_FUNC3
#define P2_12_I2STX_WS      PIN_FUNC3
#define P2_13_I2STX_SDA     PIN_FUNC3

#define P4_28_RX_MCLK       PIN_FUNC1
#define P4_29_TX_MCLK       PIN_FUNC1

#define P0_6_FUNC_POS      12
#define P0_6_FUNC_MASK     (3 << P0_6_FUNC_POS)
#define P0_7_FUNC_POS      14
#define P0_7_FUNC_MASK     (3 << P0_7_FUNC_POS)
#define P0_8_FUNC_POS      16
#define P0_8_FUNC_MASK     (3 << P0_8_FUNC_POS)
#define P0_9_FUNC_POS      18
#define P0_9_FUNC_MASK     (3 << P0_9_FUNC_POS)

#define P0_15_FUNC_POS      30
#define P0_15_FUNC_MASK     (3 << P0_15_FUNC_POS)
#define P0_16_FUNC_POS      0
#define P0_16_FUNC_MASK     (3 << P0_16_FUNC_POS)
#define P0_17_FUNC_POS      2
#define P0_17_FUNC_MASK     (3 << P0_17_FUNC_POS)
#define P0_18_FUNC_POS      4
#define P0_18_FUNC_MASK     (3 << P0_18_FUNC_POS)

#define P1_20_FUNC_POS      8
#define P1_20_FUNC_MASK     (3 << P0_15_FUNC_POS)
#define P1_21_FUNC_POS      10
#define P1_21_FUNC_MASK     (3 << P0_16_FUNC_POS)
#define P1_22_FUNC_POS      12
#define P1_22_FUNC_MASK     (3 << P0_17_FUNC_POS)
#define P1_23_FUNC_POS      14
#define P1_23_FUNC_MASK     (3 << P0_18_FUNC_POS)
#define P1_24_FUNC_POS      16
#define P1_24_FUNC_MASK     (3 << P0_18_FUNC_POS)


void GPIO_Init(pinName_e name, uint8_t cfg);
void GPIO_Function(pinName_e name, uint8_t func);
void GPIO_Mode(pinName_e name, uint8_t mode);
void GPIO_Direction(pinName_e name, uint8_t mode);
void GPIO_Write(pinName_e name, uint8_t state);
void GPIO_Toggle(pinName_e name);
uint32_t GPIO_Read(pinName_e name);

#ifdef __cplusplus
}
#endif
#endif
