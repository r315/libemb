#ifndef _rit_lpc17xx_h_
#define _rit_lpc17xx_h_

#define RIT_PEN               (1<<16)

//RICTRL Bits
#define RIT_RICTRL_RITINT     (1 << 0)
#define RIT_RICTRL_RITENCLR   (1 << 1)
#define RIT_RICTRL_RITENBR    (1 << 2)
#define RIT_RICTRL_RITEN      (1 << 3)

/**
 * @brief 
 * 
 */
void RIT_Init(void);
void RIT_DeInit(void);

#endif /* _rittimer_h_*/
