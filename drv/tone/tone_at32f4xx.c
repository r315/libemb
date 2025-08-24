#include <stddef.h>
#include "tone.h"
#include "dma.h"
#include "gpio.h"
#include "at32f4xx.h"
#include "gpio_at32f4xx.h"
#include "dma_at32f4xx.h"
#include "tone_at32f4xx.h"




#define TONE_DMA_REQUEST
#define FREQ_TO_US(_F)          ((1000000/_F) - 1)


/**
 * @brief Basic tone generation using Timer and DMA
 *
 * Timer is configured for PWM operation which uses AR register
 * to set frequency period and CCx for volume.
 *
 * Tones are played by setting AR through dma transfers and duration
 * by the number of transfers.
 *
 * */

typedef struct {
    TMR_Type *tmr;              // Timer peripheral instance
    dmatype_t dma;              // DMA data
    const tone_t *next;         // Next tone to be played
    volatile uint32_t *duty;
    uint8_t volume;
    uint8_t status;
    uint16_t duration;          // Duration in us
    uint16_t period;            // PWM frequency period in us
    void (*cb)(const tone_t**); // Next tone load function
}pwm_tone_t;

static pwm_tone_t tone_pwm = {0};

/**
 * @brief callback from dma end of transfer
 * This checks if there are more tones to be played,
 * if so configures new dma else stops dma and timer.
 * */
static void tone_eof_handler(void)
{
    DMA_Cancel(&tone_pwm.dma);

    // Get next tone
    if(tone_pwm.cb){
        tone_pwm.cb(&tone_pwm.next);
    }else{
        // double test
        if(tone_pwm.next)
            tone_pwm.next++;
    }

    // and play it
    if(tone_pwm.next != NULL){
        if(tone_pwm.next->d != 0){
            if(tone_pwm.next->f != 0){
                tone_pwm.period = FREQ_TO_US(tone_pwm.next->f);
            }
            // TODO: FIX when frequency = 0
            // for now uses the previous played frequency
            // to calculate the muted duration

            tone_pwm.duration = (tone_pwm.next->d * 1000UL) / tone_pwm.period;

            *tone_pwm.duty = (tone_pwm.period * tone_pwm.volume) / 100;

            tone_pwm.dma.len = tone_pwm.duration;   // Update tone duration
            DMA_Start(&tone_pwm.dma);
            return;
        }
        // cancel tone if duration is 0, even if next is not null
    }

   // Tone ended
   TONE_Stop();
}


 /**
  * @brief Configure Timer for PWM operation with 1MHz clock.
  * This gives a resolution of 1us which is used to generate
  * square waves from 50Hz to 50kHz.
  *
  * @param init
  * @return     TONE_ERR, TONE_IDLE
  */
uint32_t TONE_PwmInit(tone_pwm_init_t *init){
    uint16_t ccm_value, ctrl1_value, dma_req;

    switch((uint32_t)init->tmr){
        case (uint32_t)TMR1:
            RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, ENABLE);
            dma_req = DMA1_REQ_TIM1_UP;
            TMR1->BRKDT = TMR_BRKDT_MOEN;
            break;
        case (uint32_t)TMR2:
            RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2, ENABLE);
            dma_req = DMA1_REQ_TIM2_UP;
            break;
        case (uint32_t)TMR3:
            RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3, ENABLE);
            dma_req = DMA1_REQ_TIM3_UP;
            break;
        case (uint32_t)TMR4:
            RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR4, ENABLE);
            dma_req = DMA1_REQ_TIM4_UP;
            break;
        case (uint32_t)TMR5:
            RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR5, ENABLE);
            dma_req = DMA2_REQ_TIM5_UP;
            break;
        default:
            return TONE_ERR;
    }

    init->tmr->DIV = (SystemCoreClock/1000000) - 1;   // 1us clock

    if(init->pin_idle){
        // pin idles high
        ctrl1_value = 0;
        // PWM mode 2
        ccm_value = 0x70;
    }else{
        ctrl1_value = TMR_CTRL1_DIR;
        // PWM mode 1
        ccm_value = 0x60;
    }

    // Configure and enable channel
    switch(init->ch){
        case 0:
            init->tmr->CCM1 = (init->tmr->CCM1 & ~0x70) | ccm_value;
            tone_pwm.duty = &init->tmr->CC1;
            break;
        case 1:
            init->tmr->CCM1 = (init->tmr->CCM1 & ~0x7000) | (ccm_value << 8);
            tone_pwm.duty = &init->tmr->CC2;
            break;
        case 2:
            init->tmr->CCM2 = (init->tmr->CCM2 & ~0x70) | ccm_value;
            tone_pwm.duty = &init->tmr->CC3;
            break;
        case 3:
            init->tmr->CCM2 = (init->tmr->CCM2 & ~0x7000) | (ccm_value << 8);
            tone_pwm.duty = &init->tmr->CC4;
            break;
        default:
            return TONE_ERR;
    }

    init->tmr->CTRL1 = ctrl1_value;
    init->tmr->CCE |= TMR_CCE_C1EN << (init->ch << 2);

    // Force idle state
    init->tmr->AR = 0xFFFF;
    init->tmr->CNT = init->tmr->AR;

    tone_pwm.tmr = init->tmr;

    TONE_Volume(tone_pwm.volume);

    tone_pwm.tmr->EVEG |= TMR_EVEG_UEVG;
    // Enable DMA Request on update event
    tone_pwm.tmr->DIE |= TMR_DIE_UEVDE;

    tone_pwm.dma.dir = DMA_DIR_P2P;
    tone_pwm.dma.src = &tone_pwm.period;
    tone_pwm.dma.ssize = DMA_CCR_MSIZE_16;
    tone_pwm.dma.dst = (void*)&tone_pwm.tmr->AR;
    tone_pwm.dma.dsize = DMA_CCR_PSIZE_16;
    tone_pwm.dma.single = 1; // No circular buffer
    tone_pwm.dma.eot = tone_eof_handler;
    DMA_Config(&tone_pwm.dma, dma_req);

    GPIO_Config(init->pin, GPO_MS_AF);

    tone_pwm.next = NULL;
    tone_pwm.cb = NULL;
    tone_pwm.status = TONE_IDLE;

    return tone_pwm.status;
}

/**
 * @brief Starts playing a tone
 */
void TONE_Start(uint16_t f, uint16_t d)
{
    if(f == 0 || d == 0){
        return;
    }

    if(!tone_pwm.tmr){
        tone_pwm.status = TONE_ERR;
        return;
    }

    tone_pwm.period = FREQ_TO_US(f);                        // Get period in us, this value is loaded to AR through DMA
    tone_pwm.duration = (d * 1000UL) / tone_pwm.period;     // Calculate duration in us

	tone_pwm.dma.len = tone_pwm.duration;                   // Number of transfers => duration
    DMA_Start(&tone_pwm.dma);

	tone_pwm.tmr->AR = tone_pwm.period;			            // Set PWM frequency
	tone_pwm.tmr->CNT = tone_pwm.tmr->AR;	            // Force inactive state
	*tone_pwm.duty = (tone_pwm.period * tone_pwm.volume) / 100; // Set duty
    tone_pwm.tmr->CTRL1 |= TMR_CTRL1_CNTEN;
    tone_pwm.status |= TONE_PLAYNG;
}

/**
 * @brief Stops tone
 */
void TONE_Stop(void)
{
    DMA_Cancel(&tone_pwm.dma);
    tone_pwm.tmr->CTRL1 &= ~TMR_CTRL1_CNTEN;    // Stop timer
    tone_pwm.tmr->CNT = tone_pwm.tmr->AR;       // Force idle state
    tone_pwm.next = NULL;
    tone_pwm.cb = NULL;
    tone_pwm.status = TONE_IDLE;
}

/**
 * @brief Plays a sequence of tones
 */
uint8_t TONE_Play(const tone_t *tone)
{
    if(tone){
        TONE_Start(tone->f, tone->d);
        tone_pwm.next = tone;
    }

    return tone_pwm.status;
}

/**
 * @brief Change tone volume by changing
 * duty cycle
 *
 * @param level : Tone volume 0 to tone frequency period
 *
 * @return : Current tone volume
 *
 * */
uint8_t TONE_Volume(uint8_t level)
{
    if(tone_pwm.tmr && level <= 100){
        tone_pwm.volume = level;
        *tone_pwm.duty = (tone_pwm.period * level) / 100;
	}

    return tone_pwm.volume;
}

/**
 * @brief
 *
 * @return enum tone_e
 */
enum tone_e TONE_Status(void)
{
    return tone_pwm.status;
}

/**
 * @brief Set a callback function to be called at
 * the end of tone play
 *
 * @param cb
 */
void TONE_SetCallback(void (*cb)(const tone_t **))
{
    tone_pwm.cb = cb;
}
