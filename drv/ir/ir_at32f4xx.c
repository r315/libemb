#include <stdint.h>
#include "at32f4xx.h"
#include "gpio_at32f4xx.h"
#include "dma_at32f4xx.h"
#include "gpio.h"
#include "dma.h"
#include "clock.h"


#define IR_CARRIER_FREQ     36000UL
#define IR_CARRIER_T        (1000000UL / IR_CARRIER_FREQ)
#define IR_CARRIER_DUTY     7   /* duty must be between 25% and 50% */

typedef struct ir_handle{
    dmatype_t dma;
    TMR_Type *tmr;
    uint16_t duty;              // Internal variable for which the value is copy to CCx by DMA
    const uint16_t *sequence;   // pointer to sequence
    uint16_t length;            // sequence length
    void (*cb)(void);           // callback for end of transmission, set by application to repeat sequence
}ir_handle_t;

static ir_handle_t hir;

/**
 * @brief
 * @param
 */
static void ir_start(void)
{
    DMA_Start(&hir.dma);
    hir.tmr->CC1 = hir.duty;
    hir.tmr->CTRL1 |= TMR_CTRL1_CNTEN;
}

/**
 * @brief Handler for transmission
 * called at end of each transmitted mark/space
 */
static void ir_eof_handler(void)
{
    if(--hir.length == 0){
        // sequence ended
        hir.tmr->CTRL1 &= ~TMR_CTRL1_CNTEN; // Stop timer
        hir.tmr->EVEG = TMR_EVEG_UEVG;      // Ensure that disables output
        if(hir.cb){
            hir.cb();
        }
        return;
    }

    hir.sequence++;
    uint16_t duration = *hir.sequence;
    hir.duty = (duration & 0x8000) ? IR_CARRIER_T - IR_CARRIER_DUTY : IR_CARRIER_T + 1;
    hir.dma.len = (duration & 0x7FFF) / IR_CARRIER_T;
    ir_start();
}

/**
 * @brief Power up and initialization of timers and dma
 *
 */
void IR_Init(void)
{
    uint32_t pclk;
    hir.tmr = TMR1;

    RCC->APB2EN |= RCC_APB2EN_TMR1EN;
    pclk = CLOCK_Get(CLOCK_CLK4);   // clock for timers

    hir.tmr->CTRL1 = 0;
    hir.tmr->DIV = (pclk / 1000000) - 1; // count 1us units
    hir.tmr->AR = IR_CARRIER_T;
    hir.tmr->CC1 = IR_CARRIER_T - IR_CARRIER_DUTY;
    hir.tmr->CCM1 = (7 << 4);           // PWM_B low when CCx > CNT
    hir.tmr->CCE = TMR_CCE_C1EN;
    hir.tmr->DIE = TMR_DIE_UEVDE;
    hir.tmr->BRKDT = TMR_BRKDT_AOEN;

    hir.dma.dir = DMA_DIR_P2P;
    hir.dma.src = (void*)&hir.duty;
    hir.dma.ssize = DMA_CCR_MSIZE_16;
    hir.dma.dst = (void*)&hir.tmr->CC1;
    hir.dma.dsize = DMA_CCR_PSIZE_16;
    hir.dma.single = 1;                 // Single transfer
    hir.dma.eot = ir_eof_handler;
    DMA_Config(&hir.dma, DMA1_REQ_TIM1_UP);

    GPIO_Config(PA_8, GPO_MS_AF);
}

/**
 * @brief Set a callback for end of transmission
 *
 * @param cb
 */
void IR_SetEotHandler(void (*cb)(void))
{
    hir.cb = cb;
}

/**
 * @brief Transmit single pulse
 *
 * @param duration pulse duration in us, must be > than 270
 */
void IR_Pulse(uint16_t duration)
{
    if(duration < IR_CARRIER_T * 10){
        return;
    }

    hir.duty = IR_CARRIER_T - IR_CARRIER_DUTY;
    hir.dma.len = duration / IR_CARRIER_T;  // Number of transfers => duration
    hir.length = 1;
    ir_start();
}


void IR_Transmit(const uint16_t *sequence, uint16_t length)
{
    if(!sequence || !length){
        return;
    }

    hir.sequence = sequence;
    hir.length = length;
    hir.duty = IR_CARRIER_T - IR_CARRIER_DUTY;
    hir.dma.len = (sequence[0] & 0x7FFF) / IR_CARRIER_T;   // First duration must enable carrier
    ir_start();
}