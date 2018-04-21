#ifndef _pwm_h_
#define _pwm_h_

#define PCPWM1 (1<<6)
#define PCLK_PWM1 12

#define TCR_TCEN  0
#define TCR_PWMEN 2  // errata on UM

#define PWM_1 1     // P2.0(TXD1)
#define PWM_2 2     // P2.1(RXD1)
#define PWM_3 3     // P2.2(PWM1.3)
#define PWM_4 4     // P2.3(PWM1.4)

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

#define PWMENA1 9

#define PWM_MAX_CH 4

/**
 * @brief initialyze PWM with given frequency
 * 		ACTIVE Outputs: P2.1, P2.2, P2.3
 * after initialization pwm channels must be enable
 * */
void PWM_Init(uint32_t tcclk);

/**
 * @brief set the duty in % for the given pwm channel
 * */
void PWM_Set(uint8_t channel, uint8_t duty);

/**
 * @brief Enable one PWM channel range(1:4)
 **/
 void PWM_Enable(uint8_t ch);
 
 /**
 * @brief Disable one PWM channel range(1:4)
 **/
 void PWM_Disable(uint8_t ch);
 
#endif
