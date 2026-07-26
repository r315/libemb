#include <stdint.h>
#include <stddef.h>
#include "at32f4xx.h"
#include "gpio_at32f4xx.h"
#include "dma_at32f4xx.h"
#include "gpio.h"
#include "dma.h"
#include "clock.h"
#include "ir.h"


#define IR_CARRIER_FREQ     36000UL
#define IR_CARRIER_T        (1000000UL / IR_CARRIER_FREQ)
#define IR_CARRIER_DUTY     7   /* duty must be between 25% and 50% */

#define TMR_CCM1_C1C_C1IFP1     (1 << 0)
#define TMR_CCM1_C2C_C1IFP2     (2 << 8)
#define TMR_SMC_SMSEL_TRG       (6 << 0)
#define TMR_SMC_STIS_C1IF1      (5 << 4)

typedef struct ir_handle{
    dmatype_t dma;
    TMR_Type *tmr;
    uint16_t duty;              // Internal variable for which the value is copy to CCx by DMA
    const uint16_t *sequence;   // pointer to sequence
    uint16_t length;            // sequence length
    void (*cb)(uint16_t);       // callback for end of transmission, set by application to repeat sequence
}ir_handle_t;

static ir_handle_t hir;

/**
 * @brief
 * @param
 */
static void ir_tx_start(void)
{
    DMA_Start(&hir.dma);
    hir.tmr->CC1 = hir.duty;
    hir.tmr->CTRL1 |= TMR_CTRL1_CNTEN;
}

/**
 * @brief Handler for transmission
 * called at end of each transmitted mark/space
 */
static void ir_eot_handler(void)
{
    if(--hir.length == 0){
        // sequence ended
        hir.tmr->CTRL1 &= ~TMR_CTRL1_CNTEN; // Stop timer
        hir.tmr->EVEG = TMR_EVEG_UEVG;      // Ensure that disables output
        if(hir.cb){
            hir.cb(0);
        }
        return;
    }

    uint16_t duration = *(++hir.sequence);
    hir.duty = (duration & 0x8000) ? IR_CARRIER_T - IR_CARRIER_DUTY : IR_CARRIER_T + 1;
    hir.dma.len = (duration & 0x7FFF) / IR_CARRIER_T;

    ir_tx_start();
}

/**
 * @brief Configures timer to operate on PWM B mode with the frequency of IR carrier.
 * This mode makes output low when CNT < CCx and high when CNT > CCx.
 * Carrier duty is set by CCx and calculated from AR because of PWM B mode.
 *
 * To modulate carrier with pulses, DMA is configured to make N transfers determined
 * by pulse width which are given by N = pulse width / (1 / carrier frequency).
 *
 *
 * @param
 */
static void ir_init_tx(void)
{
    hir.tmr->AR = IR_CARRIER_T;
    hir.tmr->CC1 = IR_CARRIER_T - IR_CARRIER_DUTY;
    hir.tmr->CCM1 = (7 << 4);           // PWM_B low when CCx > CNT
    hir.tmr->CCE = TMR_CCE_C1EN;
    hir.tmr->DIE = TMR_DIE_UEVDE;       // request DMA on update
    hir.tmr->BRKDT = TMR_BRKDT_AOEN;

    hir.dma.dir = DMA_DIR_P2P;
    hir.dma.src = (void*)&hir.duty;
    hir.dma.ssize = DMA_CCR_MSIZE_16;
    hir.dma.dst = (void*)&hir.tmr->CC1;
    hir.dma.dsize = DMA_CCR_PSIZE_16;
    hir.dma.single = 1;                 // Single transfer
    hir.dma.eot = ir_eot_handler;
    DMA_Config(&hir.dma, DMA1_REQ_TIM1_UP);

    GPIO_Config(PA_8, GPO_MS_AF);       // Modulated signal output
}

/**
 * @brief Configure TMR1 for input capture.
 * This is PWM input mode and and operates as fallows:
 *  - First falling edge on channel 1 input only starts timer, subsequent events triggers
 *    capture of counter to CC1 register.
 *  - Rising edge on channel 1 input triggers capture of counter to CC2 register and DMA request.
 *  - DMA reads the two register values from DMABA.
 *    DMABA register always holds the value of the last captured register, as C2IF occurs in last
 *    DMA reads first CC2 value then CC1.
 *  - Timer reaches ARR (65.536ms) and stops generating an update interrupt.
 *  - User callback is called passing the number of DMA transfers
 *
 * @param
 */
static void ir_init_rx(void)
{
    hir.tmr->AR = 0xFFFF;
    hir.tmr->SMC = TMR_SMC_SMSEL_TRG | TMR_SMC_STIS_C1IF1;
    hir.tmr->CCM1 = TMR_CCM1_C1C_C1IFP1 | TMR_CCM1_C2C_C1IFP2;
    hir.tmr->CCE = TMR_CCE_C1EN | TMR_CCE_C2EN | TMR_CCE_C1P;
    hir.tmr->DMAC = (0x0d << 0) | (0x01 << 8);  // Read 2 registers starting from CC1
    hir.tmr->CTRL1 = TMR_CTRL1_OPMODE;
    hir.tmr->EVEG = TMR_EVEG_UEVG;              // Trigger update to set initial state
    hir.tmr->STS = 0;

    hir.dma.dir = DMA_DIR_P2M;
    hir.dma.src = (void*)&hir.tmr->DMABA;       // Access CCx registers using DMA data register
    hir.dma.ssize = DMA_CCR_MSIZE_16;
    hir.dma.dsize = DMA_CCR_PSIZE_16;
    hir.dma.single = 1;
    hir.dma.eot = NULL;
    DMA_Config(&hir.dma, DMA1_REQ_TIM1_CH2);
    GPIO_Config(PA_8, GPI_FLOAT);               // Demodulated signal input

    NVIC_EnableIRQ(TMR1_OV_TMR10_IRQn);

    hir.tmr->DIE = TMR_DIE_UEVIE | TMR_DIE_C2DE; // Enable DMA and INT last to avoid false transfers
}

/**
 * @brief Power up and initialization of timers and dma
 *
 */
uint32_t IR_Init(enum ir_mode mode)
{
    if(mode > IR_MODE_RX){
        return 0;
    }

    hir.tmr = TMR1;

    RCC->APB2EN |= RCC_APB2EN_TMR1EN;
    RCC->APB2RST |= RCC_APB2RST_TMR1RST;
    RCC->APB2RST &= ~RCC_APB2RST_TMR1RST;

    hir.tmr->DIV = (CLOCK_Get(CLOCK_CLK4) / 1000000) - 1; // count 1us units

    if(mode == IR_MODE_TX){
        DMA_CancelRequest(DMA1_REQ_TIM1_CH2);
        ir_init_tx();
    }else{
        DMA_CancelRequest(DMA1_REQ_TIM1_UP);
        ir_init_rx();
    }

    return 1;
}

/**
 * @brief Set a callback for end of transmission
 * or capture.
 *
 * @param cb Callback function or NULL to disable
 */
void IR_SetCallback(void (*cb)(uint16_t))
{
    hir.cb = cb;
}

/**
 * @brief Transmit single pulse
 *
 * @param duration pulse duration in us, must be > than 1000000/carrier * 16
 */
void IR_Pulse(uint16_t duration)
{
    if(duration < IR_CARRIER_T * 16){
        return;
    }

    hir.duty = IR_CARRIER_T - IR_CARRIER_DUTY;
    hir.dma.len = duration / IR_CARRIER_T;  // Number of transfers => duration
    hir.length = 1;

    ir_tx_start();
}

/**
 * @brief Transmits a sequence of pulses.
 * Each value in sequence is the duration of a pulse in us and has to be > than 1000000/carrier * 16
 * Bit 15 of each value specifies if carrier is enabled or not
 *    ex: 0x8200 generates a 512us modulated pulse.
 *        0x0200 generates a 512us pause
 *
 * @param sequence  pulses definition
 * @param length    number of pulses in sequence
 */
void IR_Transmit(const uint16_t *sequence, uint16_t length)
{
    if(!sequence || !length){
        return;
    }

    hir.sequence = sequence;
    hir.length = length;
    hir.duty = IR_CARRIER_T - IR_CARRIER_DUTY;
    hir.dma.len = (sequence[0] & 0x7FFF) / IR_CARRIER_T;   // First duration must enable carrier

    ir_tx_start();
}

/**
 * @brief Configure continuous receive mode
 * to disable it, init in tx mode or
 * set callback to null
 *
 * @param buf
 * @param cb
 */
void IR_Receive(uint16_t *buf, uint16_t len)
{
    if(!buf || !len){
        return;
    }

    hir.dma.len = len;
    DMA_SetDst(&hir.dma, (void*)buf);
    DMA_Start(&hir.dma);
}

void TMR1_OV_TMR10_IRQHandler(void)
{
    DMA_Cancel(&hir.dma);

    // First DMA transfer from DMABA has an invalid CC1 value
    // because first falling edge only starts timer.
    ((uint16_t*)hir.dma.dst)[0] = 0;

    if(hir.cb){
        hir.cb(DMA_GetTransfers(&hir.dma));
    }

    hir.tmr->STS = 0;
    DMA_Start(&hir.dma);
}