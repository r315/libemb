#ifndef __timer_h__
#define __timer_h__

#include <stdint.h>
#include "clock.h"

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

#endif
