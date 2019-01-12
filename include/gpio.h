/**
* @file		gpio.h
* @brief	common functions for io port access
* @version	2.0
* @date		30 Out. 2016
* @modified 25 Mar. 2017
* @08-2017  New Api for STM32
* @author	Hugo Reis
**********************************************************************/

#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>
#include <LPC17xx.h>

#if defined(__TDSO__)

#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /* All pins selected */
#define GPIO_PIN_MASK              0x0000FFFFU         /* PIN mask for assert test */

//#define GPIO_PIN_SET               1
//#define GPIO_PIN_RESET             0

#define GPIO_Set(port, pinmask) port->BSRR = pinmask
#define GPIO_Clr(port, pinmask) port->BRR = pinmask
#define GPIO_Read(port) port->IDR
#define GPIO_Write(port) port->ODR

#define GPIO_DEBUG_SWD 			  \
{                                 \
			AFIO->MAPR &= (7<<24);\
			AFIO->MAPR |= (2<<24);\
}
#define GPIO_MAP_PA15_TIM2_CH1 	  \
{                                 \
			AFIO->MAPR &= (3<<8); \
			AFIO->MAPR |= (1<<8); \
}

#elif defined(__BB__) /* __TDSO__ */

#define P0_0

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
//PINSEL1
#define PINSEL_PIN_P0_23    14
#define PINSEL_PIN_P0_24    16

//PINSEL4
#define PINSEL_PIN_P2_0     0
#define PINSEL_PIN_P2_1     2
#define PINSEL_PIN_P2_2     4
#define PINSEL_PIN_P2_3     6
#define PINSEL_PIN_P2_4     8
#define PINSEL_PIN_P2_5     10
#define PINSEL_PIN_P2_6     12

/* ****** Port pin functions ****** */
//Port 0 pin functions
#define P0_23_GPIO       0
#define P0_23_AD0_0      1
#define P0_23_I2SRX_CLK  2
#define P0_23_CAP3_0     3

#define P0_24_GPIO       0
#define P0_24_AD0_2      1
#define P0_24_I2SRX_WS   2
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


#define PINSEL_P0_23(x) { LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(3 << PINSEL_PIN_P0_23)) | (x << PINSEL_PIN_P0_23); } 
#define PINSEL_P0_24(x) { LPC_PINCON->PINSEL1 = (LPC_PINCON->PINSEL1 & ~(3 << PINSEL_PIN_P0_24)) | (x << PINSEL_PIN_P0_24); }

#define PINSEL_P2_0(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_0)) | (x << PINSEL_PIN_P2_0); }
#define PINSEL_P2_1(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_1)) | (x << PINSEL_PIN_P2_1); }
#define PINSEL_P2_2(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_2)) | (x << PINSEL_PIN_P2_2); }
#define PINSEL_P2_3(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_3)) | (x << PINSEL_PIN_P2_3); }
#define PINSEL_P2_4(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_4)) | (x << PINSEL_PIN_P2_4); }
#define PINSEL_P2_5(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_5)) | (x << PINSEL_PIN_P2_5); }
#define PINSEL_P2_6(x)  { LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3 << PINSEL_PIN_P2_6)) | (x << PINSEL_PIN_P2_6); }

#define PINDIR_P0_23(x) { LPC_GPIO0->FIODIR = (LPC_GPIO0->FIODIR & ~(1<<23)) | (x<<23); }
#define PINDIR_P0_24(x) { LPC_GPIO0->FIODIR = (LPC_GPIO0->FIODIR & ~(1<<24)) | (x<<23); }

#define PINDIR_P2_0(x) { LPC_GPIO2->FIODIR = (LPC_GPIO2->FIODIR & ~(1 << 0)) | (x << 0); }


/**
* @brief Set pin of GPIO0 to high level
**/
#define GPIO_Set(pin) SETPIN = (1<<pin)

/**
* @brief Set pin of GPIO0 to low level
**/
#define GPIO_Clr(pin) CLRPIN = (1<<pin)

/**
* @brief Set pin of GPIO0 as output
**/
#define GPIO_SetOutput(pin)  SETOUTPUT((1<<pin))

/**
* @brief Set pin of GPIO0 as output
**/
#define GPIO_SetInput(pin)  SETINPUT((1<<pin))

/**
* @brief Initialyze pin and set his state
**/
void GPIO_ConfigPin(uint8_t port, uint8_t pin, uint8_t dir, uint8_t state);

/**
* @brief Set state of pin
**/
void GPIO_SetState(uint8_t port, uint8_t pin, uint8_t state);

/**
* @brief Get state of pin
**/
uint8_t GPIO_GetState(uint8_t port, uint8_t pin);

/**
* @brief configure external interrupt on pin
* on interrupt handler the int flag for pin should be cleared by seting bin in register LPC_GPIOINT->IOxIntClr;
* @param1   port  [P0,P2]
* @param2   pin   [0-31]
* @param3   level [low, high, both]
*/
void GPIO_SetInt(uint8_t port, uint8_t pin, uint8_t level);

/*
 * @brief Resets oin functions to default (GPIO)
 */
void GPIO_ResetPINSEL(void);

#elif defined(__LPC_H2106__) /* __BLUEBOARD__ */
#include <lpc2106.h>
/*
#define GPIO_Set(n) GPIO0->SET = (1 << n)  // change the state of only one pin
#define GPIO_Clr(n) GPIO0->CLR = (1 << n)  // by giving his number

#define GPIO_SetN(x) GPIO0->SET = x        // change selected pins
#define GPIO_ClrN(x) GPIO0->CLR = x        // by giving the pretended bts

#define GPIO_Write(x) GPIO0->PIN = x		//change all pins
#define GPIO_Read()   GPIO0->PIN

#define GPIO_SetOutput(n) GPIO0->DIR |= (1 << n) //affect single pin
#define GPIO_SetInput(n)  GPIO0->DIR &= ~(1 << n)

#define GPIO_SetOutputN(n) GPIO0->DIR |= n //affect multiple pin

#define GPIO_SetDir(n)  GPIO0->DIR = x //affect all pins
*/

#define GPIO_Set(n) FIO0->SET = (1 << n)        // change the state of only
#define GPIO_Clr(n) FIO0->CLR = (1 << n)        // one pin
#define GPIO_SetOutput(n) FIO0->DIR |= (1 << n) //affect single pin
#define GPIO_SetInput(n)  FIO0->DIR &= ~(1 << n)

#define GPIO_SetN(x) FIO0->SET = x              //change selected pins
#define GPIO_ClrN(x) FIO0->CLR = x
#define GPIO_Write(x) FIO0->PIN = x             //change all pins
#define GPIO_Read() FIO0->PIN
#define GPIO_SetDir(n)  FIO0->DIR = x           //affect all pins
#define GPIO_SetOutputN(n) FIO0->DIR |= n       //affect multiple pin
#define GPIO_SetInputN(n)  FIO0->DIR &= ~n      //affect multiple pin

#elif defined(__EMU__) /* __LPC_H2106__ */

#elif defined(__ESP03__)
#include <driver/gpio.h>
//#define GPIO_OUTPUT_SET(gpio_no, bit_value) gpio_output_set((bit_value)<<gpio_no, ((~(bit_value))&0x01)<<gpio_no, 1<<gpio_no,0)

#else
#error "Define Board"

#endif /* error */

#endif /* _gpio_h_ */
