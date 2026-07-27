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
#define TMR_CCM1_C1OCTRL_PWM_B  (7 << 4);           // PWM_B low when CCx > CNT

#ifdef ENABLE_IR_DUPLEX
#define IR_TX_TMR           TMR3
#define IR_RX_TMR           TMR1
#define IR_TX_PIN           PA_6
#define IR_RX_PIN           PA_8
#define IR_TX_DMA_REQUEST   DMA1_REQ_TIM3_CH1
#else
#define IR_TX_TMR           TMR1    /* TMR1_CH1 dma is same as spi1 */
#define IR_RX_TMR           TMR1
#define IR_TX_PIN           PA_8
#define IR_RX_PIN           PA_8
#define IR_TX_DMA_REQUEST   DMA1_REQ_TIM1_UP
#endif

#define TMR_CCM1_C1C_C1IFP1     (1 << 0)
#define TMR_CCM1_C2C_C1IFP2     (2 << 8)
#define TMR_SMC_SMSEL_TRG       (6 << 0)
#define TMR_SMC_STIS_C1IF1      (5 << 4)

typedef struct {
    TMR_Type *tmr;
    dmatype_t dma;
    void (*cb)(void);           // callback for end of transmission, set by application to repeat sequence
    uint16_t duty;              // Internal variable for which the value is copy to CCx by DMA
    const uint16_t *sequence;   // pointer to sequence
    uint16_t length;            // sequence length
}ir_tx_inst_t;

typedef struct {
    TMR_Type *tmr;
    dmatype_t dma;
    void (*cb)(uint16_t);       // callback for end of reception, to notify application
}ir_rx_inst_t;

typedef struct ir_handle{
#ifdef ENABLE_IR_DUPLEX
    ir_tx_inst_t   tx;
    ir_rx_inst_t   rx;
#else
    union{
        ir_tx_inst_t   tx;
        ir_rx_inst_t   rx;
    };
#endif
}ir_handle_t;

static ir_handle_t hir;

/**
 * @brief
 * @param
 */
static void ir_tx_start(void)
{
    DMA_Start(&hir.tx.dma);
    hir.tx.tmr->CC1 = hir.tx.duty;
    hir.tx.tmr->CTRL1 |= TMR_CTRL1_CNTEN;
}

/**
 * @brief Handler for transmission
 * called at end of each transmitted mark/space
 */
static void ir_eot_handler(void)
{
    if(--hir.tx.length == 0){
        // sequence ended
        hir.tx.tmr->CTRL1 &= ~TMR_CTRL1_CNTEN; // Stop timer
        hir.tx.tmr->EVEG = TMR_EVEG_UEVG;      // Ensure that disables output
        if(hir.tx.cb){
            hir.tx.cb();
        }
        return;
    }

    uint16_t duration = *(++hir.tx.sequence);
    hir.tx.duty = (duration & 0x8000) ? IR_CARRIER_T - IR_CARRIER_DUTY : IR_CARRIER_T + 1;
    hir.tx.dma.len = (duration & 0x7FFF) / IR_CARRIER_T;

    ir_tx_start();
}

/**
 * @brief Configures timer to operate on PWM B mode with the frequency of IR carrier.
 * This mode makes output low when CNT < CCx and high when CNT > CCx.
 * Carrier duty is set by CCx and calculated from AR because of PWM B mode.
 *
 * To modulate carrier with pulses, DMA is configured to make N transfers determined
 * by pulse width which are given by N = pulse width / (1 / carrier frequency).
 * DMA is triggered by CCx matching and at end of transfer callback stopt timer
 *
 *
 * @param
 */
static void ir_init_tx(void)
{
    hir.tx.tmr->AR = IR_CARRIER_T;
    hir.tx.tmr->CC1 = IR_CARRIER_T - IR_CARRIER_DUTY;
    hir.tx.tmr->CCM1 = TMR_CCM1_C1OCTRL_PWM_B;
    hir.tx.tmr->CCE = TMR_CCE_C1EN;
    hir.tx.tmr->DIE = TMR_DIE_C1DE;       // request DMA on match
    hir.tx.tmr->BRKDT = TMR_BRKDT_AOEN;

    hir.tx.dma.dir = DMA_DIR_P2P;
    hir.tx.dma.src = (void*)&hir.tx.duty;
    hir.tx.dma.ssize = DMA_CCR_MSIZE_16;
    hir.tx.dma.dst = (void*)&hir.tx.tmr->CC1;
    hir.tx.dma.dsize = DMA_CCR_PSIZE_16;
    hir.tx.dma.single = 1;                 // Single transfer
    hir.tx.dma.eot = ir_eot_handler;
    DMA_Config(&hir.tx.dma, IR_TX_DMA_REQUEST);

    GPIO_Config(IR_TX_PIN, GPO_MS_AF);     // Modulated signal output
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
    hir.rx.tmr->AR = 0xFFFF;
    hir.rx.tmr->SMC = TMR_SMC_SMSEL_TRG | TMR_SMC_STIS_C1IF1;
    hir.rx.tmr->CCM1 = TMR_CCM1_C1C_C1IFP1 | TMR_CCM1_C2C_C1IFP2;
    hir.rx.tmr->CCE = TMR_CCE_C1EN | TMR_CCE_C2EN | TMR_CCE_C1P;
    hir.rx.tmr->DMAC = (0x0d << 0) | (0x01 << 8);  // Read 2 registers starting from CC1
    hir.rx.tmr->CTRL1 = TMR_CTRL1_OPMODE;
    hir.rx.tmr->EVEG = TMR_EVEG_UEVG;              // Trigger update to set initial state
    hir.rx.tmr->STS = 0;

    hir.rx.dma.dir = DMA_DIR_P2M;
    hir.rx.dma.src = (void*)&hir.rx.tmr->DMABA;       // Access CCx registers using DMA data register
    hir.rx.dma.ssize = DMA_CCR_MSIZE_16;
    hir.rx.dma.dsize = DMA_CCR_PSIZE_16;
    hir.rx.dma.single = 1;
    hir.rx.dma.eot = NULL;
    DMA_Config(&hir.rx.dma, DMA1_REQ_TIM1_CH2);      // Channel 2 is the one that triggers DMA
    GPIO_Config(IR_RX_PIN, GPI_FLOAT);               // Demodulated signal input

    NVIC_EnableIRQ(TMR1_OV_TMR10_IRQn);
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

    RCC->APB2EN |= RCC_APB2EN_TMR1EN;
    RCC->APB2RST |= RCC_APB2RST_TMR1RST;
    RCC->APB2RST &= ~RCC_APB2RST_TMR1RST;

    #ifdef ENABLE_IR_DUPLEX
    RCC->APB1EN |= RCC_APB1EN_TMR3EN;
    RCC->APB1RST |= RCC_APB1RST_TMR3RST;
    RCC->APB1RST &= ~RCC_APB1RST_TMR3RST;
    hir.tx.tmr = IR_TX_TMR;
    hir.rx.tmr = IR_RX_TMR;
    hir.tx.tmr->DIV = (CLOCK_Get(CLOCK_CLK4) / 1000000) - 1;
    hir.rx.tmr->DIV = (CLOCK_Get(CLOCK_CLK4) / 1000000) - 1;
    ir_init_tx();
    ir_init_rx();
    #else
    hir.tx.tmr = IR_TX_TMR;
    hir.tx.tmr->DIV = (CLOCK_Get(CLOCK_CLK4) / 1000000) - 1; // count 1us units
    if(mode == IR_MODE_TX){
        DMA_CancelRequest(DMA1_REQ_TIM1_CH2);
        NVIC_DisableIRQ(TMR1_OV_TMR10_IRQn);
        ir_init_tx();
    }else{
        DMA_CancelRequest(DMA1_REQ_TIM1_UP);
        ir_init_rx();
    }
    #endif


    return 1;
}

/**
 * @brief Set a callback for end of transmission
 * or capture.
 *
 * @param cb Callback function or NULL to disable
 */
void IR_SetTxCallback(void (*cb)(void))
{
    hir.tx.cb = cb;
}

/**
 * @brief Set a callback for reception timeout (timer overflow)
 *
 * @param cb Callback function or NULL to disable
 */
void IR_SetRxCallback(void (*cb)(uint16_t))
{
    hir.rx.cb = cb;
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

    hir.tx.duty = IR_CARRIER_T - IR_CARRIER_DUTY;
    hir.tx.dma.len = duration / IR_CARRIER_T;  // Number of transfers => duration
    hir.tx.length = 1;

    ir_tx_start();
}

/**
 * @brief Transmits a sequence of pulses.
 * Each value in sequence is the duration of a pulse in us and has to be > than 1000000/carrier * 16
 * Bit 15 of each value specifies if carrier is enabled or not
 *    ex: 0x8200 generates a 512us modulated pulse.
 *        0x0200 generates a 512us pause
 *
 * Note: zero values on sequence are not allowed
 *
 * @param sequence  pulses definition
 * @param length    number of pulses in sequence
 */
void IR_Transmit(const uint16_t *sequence, uint16_t length)
{
    if(!sequence || !length){
        return;
    }

    hir.tx.sequence = sequence;
    hir.tx.length = length;
    hir.tx.duty = IR_CARRIER_T - IR_CARRIER_DUTY;
    hir.tx.dma.len = (sequence[0] & 0x7FFF) / IR_CARRIER_T;   // First duration must enable carrier

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

    hir.rx.dma.len = len;
    DMA_SetDst(&hir.rx.dma, (void*)buf);
    DMA_Start(&hir.rx.dma);
    hir.rx.tmr->DIE = TMR_DIE_UEVIE | TMR_DIE_C2DE;
}

void TMR1_OV_TMR10_IRQHandler(void)
{
    DMA_Cancel(&hir.rx.dma);

    // First DMA transfer from DMABA has an invalid CC1 value
    // because first falling edge only starts timer.
    ((uint16_t*)hir.rx.dma.dst)[0] = 0;

    if(hir.rx.cb){
        hir.rx.cb(DMA_GetTransfers(&hir.rx.dma));
    }

    hir.rx.tmr->STS = 0;
    DMA_Start(&hir.rx.dma);
}