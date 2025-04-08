#include <stddef.h>
#include "tone.h"
#include "dma.h"
#include "gpio.h"
#include "at32f4xx.h"
#include "gpio_at32f4xx.h"
#include "dma_at32f4xx.h"

#define FREQ_TO_US(_F)          ((1000000/_F) - 1)

#define TONE_TIM                TMR5
#define TONE_TIM_CH             4
#define TONE_DMA_REQUEST        DMA2_REQ_TIM5_UP
#define TONE_PIN                PA_3
#define TONE_PIN_IDLE           1

#define TONE_DEFAULT_VOLUME     1


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
    TMR_Type *tmr;
    const tone_t *ptone;        // Next tone to be played
    volatile uint32_t *duty;    // CCM reg
    uint8_t volume;
    uint8_t status;
    void (*cb)(const tone_t**);
}pwm_tone_t;

static pwm_tone_t pwm_tone = {0};
static dmatype_t pwm_tone_dma;
static uint16_t tone_period;    // Period of PWM frequency in us
static uint16_t tone_duration;  // Tone duration in us

/**
 * @brief callback from dma end of transfer
 * This checks if there are more tones to be played,
 * if so configures new dma else stops dma and timer.
 * */
static void tone_callback(void)
{
    DMA_Cancel(&pwm_tone_dma);

    // Get next tone
    if(pwm_tone.cb){
        pwm_tone.cb(&pwm_tone.ptone);
    }else{
        // double test
        if(pwm_tone.ptone)
            pwm_tone.ptone++;
    }

    // and play it
    if(pwm_tone.ptone != NULL){
        if(pwm_tone.ptone->d != 0){
            if(pwm_tone.ptone->f != 0){
                tone_period = FREQ_TO_US(pwm_tone.ptone->f);
            }
            // TODO: FIX when frequency = 0
            // for now uses the previous played frequency
            // to calculate the muted duration

            tone_duration = (pwm_tone.ptone->d * 1000UL) / tone_period;

            *pwm_tone.duty = (tone_period * pwm_tone.volume) / 100;

            pwm_tone_dma.len = tone_duration;   // Update tone duration
            DMA_Start(&pwm_tone_dma);
            return;
        }
        // cancel tone if duration is 0, even if ptone is not null
    }

   // Tone ended
   TONE_Stop();
}


/**
 * @brief Configure Timer for PWM operation with 1MHz clock.
 * This gives a resolution of 1us which is used to generate
 * square waves from 50Hz to 50kHz.
 *
 */
static void init_timer(TMR_Type *tmr, uint8_t ch, uint8_t idle){
    uint16_t ccm_value;

    switch((uint32_t)tmr){
        case (uint32_t)TMR2:
            RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2, ENABLE);
            break;
        case (uint32_t)TMR5:
            RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR5, ENABLE);
            break;
    }

    tmr->DIV = (SystemCoreClock/1000000) - 1;   // 1us clock

    if(idle){
        // pin idles high
        tmr->CTRL1 = 0;
        // PWM mode 2
        ccm_value = 0x70;
    }else{
        tmr->CTRL1 = TMR_CTRL1_DIR;
        // PWM mode 1
        ccm_value = 0x60;
    }

    // Configure and enable channel
    switch(ch){
        case 1:
            tmr->CCM1 = (tmr->CCM1 & ~0x70) | ccm_value;
            tmr->CCE |= TMR_CCE_C1EN;
            pwm_tone.duty = &tmr->CC1;
            break;
        case 2:
            tmr->CCM1 = (tmr->CCM1 & ~0x7000) | (ccm_value << 8);
            tmr->CCE |= TMR_CCE_C2EN;
            pwm_tone.duty = &tmr->CC2;
            break;
        case 3:
            tmr->CCM2 = (tmr->CCM2 & ~0x70) | ccm_value;
            tmr->CCE |= TMR_CCE_C3EN;
            pwm_tone.duty = &tmr->CC3;
            break;
        case 4:
            tmr->CCM2 = (tmr->CCM2 & ~0x7000) | (ccm_value << 8);
            tmr->CCE |= TMR_CCE_C4EN;
            pwm_tone.duty = &tmr->CC4;
            break;
    }

    // Force idle state
    tmr->AR = 0xFFFF;
    tmr->CNT = tmr->AR;

    pwm_tone.tmr = tmr;

    TONE_Volume(TONE_DEFAULT_VOLUME);

    tmr->EVEG |= TMR_EVEG_UEVG;
    // Enable DMA Request on update event
    tmr->DIE |= TMR_DIE_UEVDE;
}

/**
 * @brief Initializes and configure
 * low level hardware
 */
void TONE_Init(void)
{
    init_timer(TONE_TIM, TONE_TIM_CH, TONE_PIN_IDLE);

    pwm_tone_dma.dir = DMA_DIR_P2P;
    pwm_tone_dma.src = &tone_period;
    pwm_tone_dma.ssize = DMA_CCR_MSIZE_16;
    pwm_tone_dma.dst = (void*)&pwm_tone.tmr->AR;
    pwm_tone_dma.dsize = DMA_CCR_PSIZE_16;
    pwm_tone_dma.single = 1; // No circular buffer
    pwm_tone_dma.eot = tone_callback;
    DMA_Config(&pwm_tone_dma, TONE_DMA_REQUEST);

    GPIO_Config(TONE_PIN, GPO_MS_AF);

    pwm_tone.ptone = NULL;
    pwm_tone.cb = NULL;
    pwm_tone.status = TONE_IDLE;
}

/**
 * @brief Starts playing a tone
 */
void TONE_Start(uint16_t f, uint16_t d)
{
    if(f == 0 || d == 0){
        return;
    }

    if(!pwm_tone.tmr){
        pwm_tone.status = TONE_ERR;
        return;
    }

    tone_period = FREQ_TO_US(f);                        // Get period in us, this value is loaded to AR through DMA
    tone_duration = (d * 1000UL) / tone_period;         // Calculate duration in us

	pwm_tone_dma.len = tone_duration;                   // Number of transfers => duration
    DMA_Start(&pwm_tone_dma);

	pwm_tone.tmr->AR = tone_period;			            // Set PWM frequency
	pwm_tone.tmr->CNT = pwm_tone.tmr->AR;	            // Force inactive state
	*pwm_tone.duty = (tone_period * pwm_tone.volume) / 100; // Set duty
    pwm_tone.tmr->CTRL1 |= TMR_CTRL1_CNTEN;
    pwm_tone.status |= TONE_PLAYNG;
}

/**
 * @brief Stops tone
 */
void TONE_Stop(void)
{
    DMA_Cancel(&pwm_tone_dma);
    pwm_tone.tmr->CTRL1 &= ~TMR_CTRL1_CNTEN;    // Stop timer
    pwm_tone.tmr->CNT = pwm_tone.tmr->AR;       // Force idle state
    pwm_tone.ptone = NULL;
    pwm_tone.cb = NULL;
    pwm_tone.status = TONE_IDLE;
}

/**
 * @brief Plays a sequence of tones
 */
uint8_t TONE_Play(const tone_t *tone)
{
    if(tone){
        TONE_Start(tone->f, tone->d);
        pwm_tone.ptone = tone;
    }

    return pwm_tone.status;
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
    if(pwm_tone.tmr && level <= 100){
        pwm_tone.volume = level;
        *pwm_tone.duty = (tone_period * level) / 100;
	}

    return pwm_tone.volume;
}

void TONE_SetCallback(void (*cb)(const tone_t **))
{
    pwm_tone.cb = cb;
}
