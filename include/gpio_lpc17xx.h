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

/** @defgroup GPIO Alternative functions
  * @{
  */

#define GPIO_FUNC0          0
#define GPIO_FUNC1          1
#define GPIO_FUNC2          2
#define GPIO_FUNC3          3

#define P0_4_I2SRX_CLK      GPIO_FUNC1
#define P0_5_I2SRX_WS       GPIO_FUNC1
#define P0_6_I2SRX_SDA      GPIO_FUNC1
#define P0_7_I2STX_CLK      GPIO_FUNC1
#define P0_8_I2STX_WS       GPIO_FUNC1
#define P0_9_I2STX_SDA      GPIO_FUNC1

#define P0_23_I2SRX_CLK     GPIO_FUNC2
#define P0_24_I2SRX_WS      GPIO_FUNC2
#define P0_25_I2SRX_SDA     GPIO_FUNC2

#define P2_11_I2STX_CLK     GPIO_FUNC3
#define P2_12_I2STX_WS      GPIO_FUNC3
#define P2_13_I2STX_SDA     GPIO_FUNC3

#define P4_28_RX_MCLK       GPIO_FUNC1
#define P4_29_TX_MCLK       GPIO_FUNC1

#define P0_23_GPIO       0
#define P0_23_AD0_0      1
#define P0_23_CAP3_0     3

#define P0_24_GPIO       0
#define P0_24_AD0_2      1
#define P0_24_CAP3_1     3

//Port 2 pin functions
#define P2_0_GPIO      0
#define P2_0_PWM1_1    1
#define P2_0_TXD1      2

#define P2_1_GPIO      0
#define P2_1_PWM1_2    1
#define P2_1_RXD1      2

#define P2_2_GPIO      0
#define P2_2_PWM1_3    1
#define P2_2_CTS1      2

#define P2_3_GPIO      0
#define P2_3_PWM1_4    1
#define P2_3_DCD1      2

#define P2_4_GPIO      0
#define P2_4_PWM1_5    1
#define P2_4_DSR1      2

#define P2_5_GPIO      0
#define P2_5_PWM1_6    1
#define P2_5_DTR1      2

#define P2_6_GPIO      0
#define P2_6_PACP1_0   1
#define P2_6_RI1       2

#define P2_7_GPIO      0
#define P2_7_RD2       1
#define P2_7_RTS1      2

/**
  * @}
  */

/* ****** Pin modes ****** */
#define PIN_OUT_MASK        (1 << 2)
#define PIN_OUT_PP          (PIN_OUT_MASK | (0 << 0))
#define PIN_OUT_OD          (PIN_OUT_MASK | (1 << 0))
#define PIN_IN_PU           (0 << 0)
#define PIN_IN_REPEATER     (1 << 0)
#define PIN_IN_FLOAT        (2 << 0)
#define PIN_IN_PD           (3 << 0)


void GPIO_Config(pinName_e name, uint8_t mode);
void GPIO_Write(pinName_e name, uint8_t state);
void GPIO_Toggle(pinName_e name);
//void GPIO_PORT_Write(pinName_e name, uint32_t value);
uint32_t GPIO_Read(pinName_e name);
void GPIO_Function(pinName_e pin, uint8_t func);

#ifdef __cplusplus
}
#endif
#endif
