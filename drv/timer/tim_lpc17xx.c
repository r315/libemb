#include <stddef.h>
#include "lpc17xx_hal.h"
#include "timer.h"

typedef struct stimer{
    uint32_t interval;
    uint32_t ticks;
    void (*func)(void);
}tim_t;

static tim_t list_tim[TIM_MR_REG_COUNT];
static void (*tim_cap_cb)(uint32_t);

//-----------------------------------------------
// Generic Timer API
//-----------------------------------------------

/**
 * @brief Calls a function periodically
 * 
 * @param func      : Function to be called
 * @param interval  : Interval in us
 * @return          : Timer index 3-0, other value interval not configured
 */
uint32_t TIMER_SetInterval(void (*func)(void), uint32_t us){

    // Find an Match register that is not assigned to an interval
    for(uint32_t i = 0; i < TIM_MR_REG_COUNT; i++){
        if(list_tim[i].func == NULL){
            TIM_Interval(LPC_TIM0, i, func, us);
            return i;
        }
    }

    return TIM_MR_REG_COUNT;
}

/**
 * @brief Same as TIMER_SetInterval, but calls function only once
 * 
 * @param func      : Function to be called
 * @param interval  : Interval in us
 * @return          : Timer index 3-0, other value timeout not configured
 */
uint32_t TIMER_SetTimeout(void (*func)(void), uint32_t us){
    uint32_t id = TIMER_SetInterval(func, us);

    if(id != TIM_MR_REG_COUNT){
        list_tim[id].interval = 0;
    }

    return id;
}

/**
 * @brief Cancel Interval/Timeout
 * 
 * @param tid   : Timer ID
 */
void TIMER_ClearInterval(uint32_t tid){
    LPC_TIM0->MCR &= ~(TIM_MCR_MR0I << (tid * 3)); // Disable match interrupt
    list_tim[tid].func = NULL;
    list_tim[tid].interval = 0;
}

//-----------------------------------------------
// Low level API
//-----------------------------------------------
void TIM_Start(LPC_TIM_TypeDef *tim){
    tim->TCR = TIM_TCR_EN;
}

void TIM_Stop(LPC_TIM_TypeDef *tim){
    tim->TCR = TIM_TCR_RST;
    tim->TCR = 0;
}

void TIM_Reset(LPC_TIM_TypeDef *tim){
    tim->TCR = TIM_TCR_RST;
}

void TIM_Restart(LPC_TIM_TypeDef *tim){
    tim->TCR = TIM_TCR_RST;
    tim->TCR = TIM_TCR_EN;
}

//-----------------------------------------------
// Capture
//-----------------------------------------------

/**
 * @brief   Configures HW Timer for capturing. Used to 
 *          measure pulses duration 
 * 
 * @param tim   : Structure with HW Timer registers
 */
void TIM_InitCapture(LPC_TIM_TypeDef *tim){

    if (tim == LPC_TIM0){
    }else if (tim == LPC_TIM1){
    }else if (tim == LPC_TIM2){
    }else if (tim == LPC_TIM3){
        PCONP_TIM3_ENABLE;
        TIM_Stop(tim);
        CLOCK_SetPCLK(PCLK_TIMER3, PCLK_1);
        NVIC_EnableIRQ(TIMER3_IRQn);
    }else{
        return;
    }
    
    tim->MCR = 0;                        // Disable match
    tim->PR = (SystemCoreClock / 1000000UL) - 1;  // Set TC Clock to 1Mhz
    
    tim_cap_cb = NULL;
}

/**
 * @brief Configures a capture event on pin P0_23 or P024
 * 
 * @param tim   : Structure with HW Timer registers
 * @param mrx   : Match register, currently only MR0
 * @param edge  : RE, FE or both
 * @param func  : Callback 
 */
void TIM_Capture(LPC_TIM_TypeDef *tim, uint8_t mrx, uint8_t edge, void (*func)(uint32_t)){

    if(func == NULL){
        return ;       
    }

    TIM_Reset(tim);

    mrx &= 1;  

     switch(edge){
        case 0:     // Rising edge
            tim->CCR = (tim->CCR & ~(1 << mrx * 3)) | (1 << mrx * 3); // set CAPxRE bit
            break;

        case 1:     // Falling edge
            tim->CCR = (tim->CCR & ~(2 << mrx * 3)) | (2 << mrx * 3); // set CAPxFE bit
            break;

        default:    // Both edges
        case 2:
            tim->CCR |= (3 << mrx * 3); // set CAPxRE and CAPxFE
            break;
    }

    // Configure GPIO pin
    if(mrx == 0){
        GPIO_Function(P0_23, P0_23_CAP3_0); 
    }
    else{
        GPIO_Function(P0_24, P0_24_CAP3_1);
    }
    
    tim_cap_cb = func;

    tim->CCR |= (TIM_CCR_CAP0I << (mrx * 3));         // Enable Irq    
    
    TIM_Start(tim);
}
// -------------------------------------------------
// Match
// -------------------------------------------------
/**
 * @brief Initialise timer for matching
 * 
 * @param tim 
 */
void TIM_InitMatch(LPC_TIM_TypeDef *tim){
    IRQn_Type irq;

    if (tim == LPC_TIM0){
        PCONP_TIM0_ENABLE;
        CLOCK_SetPCLK(PCLK_TIMER0, PCLK_1);
        LPC_TIM0->TCR = TIM_TCR_RST;
        irq = TIMER0_IRQn;
    }else if (tim == LPC_TIM1){
        PCONP_TIM1_ENABLE;
        CLOCK_SetPCLK(PCLK_TIMER1, PCLK_1);
        LPC_TIM1->TCR = TIM_TCR_RST;
        irq = TIMER1_IRQn;
    }else if (tim == LPC_TIM2){
        PCONP_TIM2_ENABLE;
        CLOCK_SetPCLK(PCLK_TIMER2, PCLK_1);
        LPC_TIM2->TCR = TIM_TCR_RST;
        irq = TIMER2_IRQn;
    }else if (tim == LPC_TIM3){
        PCONP_TIM3_ENABLE;
        CLOCK_SetPCLK(PCLK_TIMER3, PCLK_1);
        LPC_TIM3->TCR = TIM_TCR_RST;
        irq = TIMER3_IRQn;
    }else{
        return;
    }       
    
    tim->CCR = 0;                                   // Disable capture
    tim->MCR = 0;                                   // Disable match
    tim->TCR = TIM_TCR_RST;
    tim->PR = (SystemCoreClock / 1000000UL) - 1;    // Set TC Clock to 1Mhz

    for(uint8_t i = 0; i < TIM_MR_REG_COUNT; i++){  // Init intervals list
        list_tim[i].interval = 0;
        list_tim[i].func = NULL;
    }

    tim->TCR = TIM_TCR_EN;                          // start timer
    NVIC_EnableIRQ(irq);
}

/**
 * @brief Configure timer in compare mode, interrupt occurs when timer matches MRx registers
 * 
 * @param tim       : Timer structure LPC_TIMx
 * @param match     : Match register to be used 3-0
 * @param func      : Function to be called
 * @param interval  : Number of micro seconds to pass before interrupt
 */
void TIM_Interval(LPC_TIM_TypeDef *tim, uint8_t match, void (*func)(void), uint32_t interval){

    if(func == NULL){
        return ;       
    }    

    match &= 3;         // Only four match registers

    list_tim[match].func = func;
    list_tim[match].interval = interval;
   
    interval += tim->TC; // Compute next interval from current timer count

   *((uint32_t*)(&tim->MR0 + (match << 2))) = interval - 1; // Set match value
    

    tim->MCR |= (TIM_MCR_MR0I << (match * 3));      // Enable interrupt on match
}    

//--------------------------------------------------
//  Interrupt handlers
//--------------------------------------------------
static void tim_irq_handler(LPC_TIM_TypeDef *tim, uint32_t ir_reg){
    if(ir_reg & ( 0X0F << 0)){  // Check match interrupt flags
        // Match occurred, go through each match interrupt flag
        for(uint8_t i = 0; i < TIM_MR_REG_COUNT; i++){
            if(ir_reg & (1 << i)){
                list_tim[i].func();         // Invoke callback, if null exception will occur
                if(list_tim[i].interval){   // Check if interval is valid
                    *((uint32_t*)(&tim->MR0 + (i << 2))) = 
                    tim->TC + list_tim[i].interval;   // Compute next match value 
                }else{
                    tim->MCR &= ~(TIM_MCR_MR0I << (i * 3)); // Otherwise cancel interval
                    list_tim[i].func = NULL;
                }
            }
        }
    }    
}

void TIMER0_IRQHandler(void){
    tim_irq_handler(LPC_TIM0, LPC_TIM0->IR);
    LPC_TIM0->IR = LPC_TIM0->IR;
}

void TIMER3_IRQHandler(void){

    uint32_t ir_reg = LPC_TIM3->IR;
    
    LPC_TIM3->IR = ir_reg;

    if(ir_reg & (3 << 4)){  // Check capture interrupt flags
        // Capture occurred
        tim_cap_cb(LPC_TIM3->CR0);
    }

    tim_irq_handler(LPC_TIM3, ir_reg);
}

