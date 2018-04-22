 /** 
 @file rittimer.h
 @brief  Function prototypes for Repetitive timer driver
 @author Hugo Reis
 @date 25-01-2017
 **/
#ifndef _rittimer_h_
#define _rittimer_h_

#define RIT_PEN (1<<16)

//RICTRL Bits
#define RITINT 1
#define RITENCLR (1<<1)
#define RITENBR  (1<<2)
#define RITEN    (1<<3)


/**
@brief configure rit timer for calling a handler in intervlas of
      the specified time
**/
void RIT_InitHandler(unsigned int ticks, void *handler);

/**
 @brief set RIT clock
 **/
void RIT_SetClockDivider(uint8_t ckdiv);

/**
 @brief Stop timer and disable interrupt
 **/
void RIT_Disable(void);

/**
 @brief Stop timer
 **/
void RIT_Stop(void);

/**
 @brief Reset and start timer
 **/
void RIT_Start(void);

#endif /* _rittimer_h_*/
