#include <stdint.h>
#include "board.h"
#include "pwm.h"
#include "clock_lpc17xx.h"

#define PCONP_PCPWM1 (1<<6)

#define TCR_TCEN    0
#define TCR_PWMEN   2  // errata on UM

//#define PWM_1       1     // P2.0(TXD1)
//#define PWM_2       2     // P2.1(RXD1)
//#define PWM_3       3     // P2.2(PWM1.3)
//#define PWM_4       4     // P2.3(PWM1.4)
//#define PWM_5       5     // P2.4(PWM1.5)
//#define PWM_6       6     // P2.5(PWM1.6)

#define PWMMR0I 0
#define PWMMR0R 1
#define PWMMR0S 2

#define PWMMR1I 3
#define PWMMR1R 4
#define PWMMR1S 5

#define PWMMR2I 6
#define PWMMR2R 7
#define PWMMR2S 8

#define PWMMR3I 9
#define PWMMR3R 10
#define PWMMR3S 11

#define PWMMR4I 12
#define PWMMR4R 13
#define PWMMR4S 14

#define PWM1_PCR_ENA1_POS   9

#define PWM_MAX_CH 6

#warning "PWM driver requires fixing"

/**
 * @brief initalise PWM timer and configure GPIO pins
 *
 *
 * */
uint32_t PWM_Init(pwmchip_t *pwmchip){
    (void)pwmchip;

    LPC_SC->PCONP |= PCONP_PCPWM1;                   // Enable PWM Module

	CLOCK_SetPCLK(PCLK_PWM1, PCLK_1);

	LPC_PWM1->PR = (SystemCoreClock / 1000000UL) - 1;     // Set TC Clock to 1Mhz

    LPC_PWM1->TCR = (1<<TCR_TCEN) | (1<<TCR_PWMEN);

	LPC_PWM1->MCR = (1<<PWMMR0R);                   // reset TC on MR0 match

	//LPC_PWM1->MR0 = tcclk;                          // set all outputs on match

    return 1;
}

/**
 * @brief Changes PWM frequency, duty of enabled channels
 * is adjusted
 *
 * \param freq [in] : New pwm frequency
 * */
void PWM_Freq(uint32_t freq){
    (void)freq;
}

/**
 * @brief Set duty cycle for given channel
 *
 * \param channel [in] : Channel to be configured
 *                       PWM_1 ... PWM_6
 * \param duty [in] :    Duty cycle 0 - 100
 * */
static __IO uint32_t *mrx[] = {&LPC_PWM1->MR1, &LPC_PWM1->MR2, &LPC_PWM1->MR3, &LPC_PWM1->MR4, &LPC_PWM1->MR5, &LPC_PWM1->MR6};
void PWM_Set(uint8_t channel, uint8_t duty){

    if(channel > PWM_MAX_CH)
        return;

    if(duty > 100)
        duty = 100;

    uint32_t match = LPC_PWM1->MR0;
    match *= duty;
    match /= 100;

    *mrx[channel - 1] = match;

	LPC_PWM1->LER = (1<<channel);
}

/**
 * @brief get the duty in % of given pwm channel
 * */
uint8_t PWM_Get(uint8_t channel){
    if(channel > PWM_MAX_CH)
        return 0;

    uint32_t match = *mrx[channel - 1] * 100;
    return match /LPC_PWM1->MR0;
}

/**
 * @brief Enables PWM channel and configure GPIO pin to PWM function
 *
 * \param ch [in] Channel to be configured
 *                 PWM_1 ... PWM_6
 * */
void PWM_Enable(pwmchip_t *pwmchip, uint8_t ch, enum pwmpstate state)
{
    (void)pwmchip;
    if(ch > PWM_MAX_CH)
        return;

    ch -= 1;

    uint8_t shift = ch << 1;                        // two bits per gpio

    if(state == PWM_PIN_ON){
        LPC_PINCON->PINSEL4 &= ~(3 << shift);
        LPC_PINCON->PINSEL4 |= (1 << shift);            // Select PWM function
        LPC_PWM1->PCR |= (1 << (PWM1_PCR_ENA1_POS + ch)); // Enable pwm, single edge
    }else{
        LPC_PINCON->PINSEL4 &= ~(3 << shift);

        LPC_PWM1->PCR &= ~(1 << (PWM1_PCR_ENA1_POS + ch));
    }
}

/**
* @brief Set channel polarity
*
* \param ch    : PWM channel 1 - 4
* \param pol   : Polarity 0 - 1
**/
void PWM_Polarity(pwmchip_t *pwmchip, uint8_t ch, uint8_t pol)
{
    (void)pwmchip;
    (void)ch;
    (void)pol;
}
