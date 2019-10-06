#ifndef __timer_h__
#define __timer_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if defined(__BB__)
#include "clock_lpc17xx.h"

#define PCTIM0 1
#define PCTIM1 2
#define PCTIM2 22
#define PCTIM3 23

#define TIMER_PowerUp(x) LPC_SC->PCONP |= (1<<x)

/* TCTR Bits */
#define TIMER_ENABLE (1<<0)
#define TIMER_RESET  (1<<1)

/* External match function control*/
#define EMC_0   4
#define EMC_1   6
#define EMC_2   8
#define EMC_3   10

#define EMR_OFF 0  // Do Nothing
#define EMR_CLR 1  // Clear the corresponding External Match bit/output to 0 (MATn.m pin is LOW if pinned out).
#define EMR_SET 2  // Set the corresponding External Match bit/output to 1 (MATn.m pin is HIGH if pinned out).
#define EMR_TGL 3  // Toggle the corresponding External Match bit/output.

// TIMx MCR bits
#define MR0_IRQ     (1<<0)      // Enable interrupt on match
#define MR0_RST     (1<<1)      // Reset Timer0 on match
#define MR0_STP     (1<<2)      // Stop Timer0 on match
#define MR1_IRQ     (1<<3)      // Enable interrupt on match
#define MR1_RST     (1<<4)      // Reset Timer1 on match
#define MR1_STP     (1<<5)      // Stop Timer1 on match
#define MR2_IRQ     (1<<6)      // Enable interrupt on match
#define MR2_RST     (1<<7)      // Reset Timer2 on match
#define MR2_STP     (1<<8)      // Stop Timer2 on match
#define MR3_IRQ     (1<<9)      // Enable interrupt on match
#define MR3_RST     (1<<10)     // Reset Timer3 on match
#define MR3_STP     (1<<11)     // Stop Timer3 on match

#define MCR_MRxI    (1<<0)
#define MCR_MRxR    (1<<1)
#define MCR_MRxS    (1<<2)


typedef void (*CallBack)(void *);

/**
 * Capture related macros
 *
 * P1.26   CAP0.0
 * P1.27   CAP0.1
 * 
 * P1.18   CAP1.0
 * P1.19   CAP1.1
 * 
 * P0.4    CAP2.0
 * P0.5    CAP2.1
 * 
 * P0.23   CAP3.0
 * P0.24   CAP3.1
 * 
 */

#define PCONP_PCTIM3 (1<<23)

enum{
    CAP_FE = 1,  // Falling edge
    CAP_RE,      // Rising edge
    CAP_RFE      // Both
};

/**
 * Configure an capture event on a pin. The timer will configured to have a 1us clock
 * Each timer supports two channels with his ouwn pin. 
 * 
 * @param tim  Timer structure to be used for capture
 * @param ch   channel to be used [0,1]
 * @param edge Capturing edge
 * @param cb   callback function to execut on event, a pointer for the captured value is passed as parameter
 * */
void TIMER_CAP_Init(LPC_TIM_TypeDef *tim, char ch, char edge, CallBack cb);

/**
 * Configure a timer to generate an event when reach the given ticks. The timer will configured to have a 1us clock
 * 
 * @param time   Timer structure to be used on match
 * @param ch     channel to be used [0,3]
 * @param us     number of us between callback calls
 * @param cb     callback function to execut on event.
 * @param ptr    callback parameters pointer
 * */
void TIMER_Periodic(LPC_TIM_TypeDef *tim, char ch, uint32_t us, CallBack cb, void *ptr);

/**
 * Stops generating events
 * 
 * @param tim    Timer structure to be used on match
 * */
void TIM_Stop(LPC_TIM_TypeDef *tim);


/**
 * Stops generating events
 * 
 * @param tim    Timer structure to be used on match
 * */
void TIM_Restart(LPC_TIM_TypeDef *tim);

#endif

typedef struct{
    uint32_t initial;
    uint32_t expires;
    uint32_t interval;
}Timer;

/**
 * @brief Gives the current tick counter value
 **/
#define TIMER_GetTicks() CLOCK_GetTicks()

/**
 *@brief Start a timer
 **/
void TIMER_Start(Timer *tm, uint32_t interval);

/**
 *@brief check if a timer is expired
 **/
uint8_t TIMER_IsExpired(Timer *tm);

/**
 *@brief resets a timer with the same interval
 *    this makes the timer consistent over time 
 **/
void TIMER_Reset(Timer *tm);

/**
 *@brief resets a timer
 **/
void TIMER_Restart(Timer *tm);

/**
 *@brief set clock divider for timerx
 **/
void TIMER_SetClockDivider(uint8_t tim, uint8_t div);

#ifdef __cplusplus
}
#endif

#endif
