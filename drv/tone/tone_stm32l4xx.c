
#include <stddef.h>
#include "stm32l4xx.h"
#include "dma_stm32l4xx.h"
#include "tone.h"
#include "dma.h"

#define BUZ_DEFAULT_VOLUME     	1
#define FREQ_TO_US(_F)          (1000000/_F)

typedef struct {
    TIM_TypeDef *tim;
    dmatype_t dma;
    const tone_t *ptone;
    uint8_t volume;
    uint8_t status;
    uint16_t period;    // Period of PWM frequency in us
    uint16_t duration;  // Tone duration in us
    volatile uint32_t *duty;
    void (*cb)(const tone_t**);
}tone_pwm_t;

static tone_pwm_t tone_pwm = {0};

/**
 * @brief Basic tone generation on pin PA8 using TIM1_CH1 and DMA
 *
 * Buzzer timer is configured as PWM mode1 in count down mode with 1MHz clock.
 * The counter starts from the ARR register value (top) which defines the frequency period in us.
 * It counts down until matches CCR1 (duty) and on that event the correspondent output is set to high.
 * When the counter reaches zero, the correspondent output is set to low and DMA request is raised to
 * transfer a new value to ARR register.
 *
 * On last DMA transfer an interrupt is issued, which will configure the next tone frequency or stop
 * any playing melody.
 *
 * */

/**
 * @brief TIM1 update event DMA request handler
 *
 * */
static void tone_eot_handler(void)
{
    // Get next tone
    if(tone_pwm.cb){
        tone_pwm.cb(&tone_pwm.ptone);
    }else{
        // double test
        if(tone_pwm.ptone)
        tone_pwm.ptone++;
    }

    if(tone_pwm.ptone != NULL){
        if(tone_pwm.ptone->d != 0){
            // Load tone
            if(tone_pwm.ptone->f != 0){
                tone_pwm.period = FREQ_TO_US(tone_pwm.ptone->f) - 1; // Set PWM freq
            }

            // TODO: FIX when frequency = 0
            // for now uses the previous played frequency
            // to calculate the muted duration

            tone_pwm.duration = (tone_pwm.ptone->d * 1000UL) / tone_pwm.period;

            TONE_Volume(tone_pwm.volume);

            tone_pwm.dma.len = tone_pwm.duration;   // Update tone duration
            DMA_Start(&tone_pwm.dma);
            return;
        }
    }

    // Tone ended
    TONE_Stop();
}

/**
 * @brief
 * PA8 for TIM1_CH1
 * PB6 for TIM16_CH1N
 */
static void tone_init_pin(void)
{
#if defined(__NUCLEO_L412KB__)
#define GPIOB_PIN6_AF           (2 << GPIO_MODER_MODE6_Pos)
#define GPIOB_PIN6_TIM16_CH1N   (14 << GPIO_AFRH_AFSEL14_Pos)

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
/*
	GPIO_InitTypeDef GPIO_InitStruct = {
		.Pin = GPIO_PIN_6,
		.Mode = GPIO_MODE_AF_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW,
		.Alternate = GPIO_AF14_TIM16
	};
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
*/
    tone_pwm.tim->CCER |= TIM_CCER_CC1NE;

    uint32_t tmp = GPIOB->MODER & ~(GPIO_MODER_MODE6);
    GPIOB->MODER = tmp | GPIOB_PIN6_AF;
    tmp = GPIOB->AFR[0] & ~(GPIO_AFRL_AFSEL6);
    GPIOB->AFR[0] = tmp | GPIOB_PIN6_TIM16_CH1N;

#else
	//gpioInit(GPIOA, 8, GPO_AF | GPO_2MHZ);
	GPIOA->CRH = (GPIOA->CRH & ~(0x0F)) | (2 << 2) | (2 << 0);
#endif
}

uint32_t TONE_PwmInit(tone_init_t *init)
{
    uint16_t ccm_value, ctrl1_value, dma_req;
    // Enable timer and set dma request
    switch((uint32_t)init->per){
        case (uint32_t)TIM1:
            RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
            dma_req = DMA1_REQ_TIM1_UP;
            // Enable OC and OCN
            TIM1->BDTR |= TIM_BDTR_MOE;
            break;
        //case (uint32_t)TIM2:
        //case (uint32_t)TIM6:
        //case (uint32_t)TIM15:
        case (uint32_t)TIM16:
            RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
            dma_req = DMA1_REQ_TIM16_UP;
            TIM16->BDTR |= TIM_BDTR_MOE;
            break;

        default:
            return TONE_ERR;
    }
    // Assign peripheral to internal structure
    tone_pwm.tim = init->per;
    // Stop timer
    tone_pwm.tim->CR1 = 0;
    // 1us clock
    tone_pwm.tim->PSC = (SystemCoreClock/1000000) - 1;
    // Set pin idle state
    // TODO: this only applies to some timers, apply only on the ones that support it
    // otherwise return error
    if(init->pin_idle){
        // pin idles high
        ctrl1_value = 0;
        // PWM mode 2
        ccm_value = 0x70;
    }else{
        ctrl1_value = TIM_CR1_DIR;
        // PWM mode 1
        ccm_value = 0x60;
    }
    // Configure and enable timer channel
    switch(init->ch){
        case 0:
            tone_pwm.tim->CCMR1 = (tone_pwm.tim->CCMR1 & ~0x70) | ccm_value;
            tone_pwm.duty = &tone_pwm.tim->CCR1;
            break;
        case 1:
        case 2:
        case 3:
        default:
            return TONE_ERR;
    }

    // set timer configuration
    tone_pwm.tim->CR1 = ctrl1_value;
    // Force idle state
    tone_pwm.tim->ARR = 0xFFF;
    tone_pwm.tim->CNT = tone_pwm.tim->ARR;

    TONE_Volume(BUZ_DEFAULT_VOLUME);

    // reset timer
    tone_pwm.tim->EGR |= TIM_EGR_UG;
    // Enable DMA Request
    tone_pwm.tim->DIER |= TIM_DIER_UDE;

    tone_init_pin();

    // Configure dma
    tone_pwm.dma.dir = DMA_DIR_P2P;
    tone_pwm.dma.src = &tone_pwm.period;
    tone_pwm.dma.ssize = DMA_CCR_MSIZE_16;
    tone_pwm.dma.dst = (void*)&tone_pwm.tim->ARR;
    tone_pwm.dma.dsize = DMA_CCR_PSIZE_16;
    tone_pwm.dma.single = 1; // No circular buffer
    tone_pwm.dma.eot = tone_eot_handler;
    DMA_Config(&tone_pwm.dma, dma_req);
    // Clear tone pointer
    tone_pwm.ptone = NULL;
    tone_pwm.cb = NULL;
    tone_pwm.status = TONE_IDLE;

    return tone_pwm.status;
}

/**
 * @brief Plays a single tone for a given time
 *
 * @param freq     : Tone fundamental frequency
 * @param duration : duration of tone in ms
 * */
void TONE_Start(uint16_t freq, uint16_t duration)
{
    if(freq == 0 || duration == 0){
        return;
    }

    if(!tone_pwm.tim){
        tone_pwm.status = TONE_ERR;
        return;
    }

    tone_pwm.period = FREQ_TO_US(freq) - 1;
    tone_pwm.duration = (duration * 1000UL) / tone_pwm.period;

    tone_pwm.dma.len = tone_pwm.duration;                   // Number of transfers => duration
    DMA_Start(&tone_pwm.dma);
    // Set frequency
	tone_pwm.tim->ARR = tone_pwm.period;
	tone_pwm.tim->CNT = tone_pwm.tim->ARR;
    // Set duty
	TONE_Volume(tone_pwm.volume);

    tone_pwm.tim->CR1 |= TIM_CR1_CEN;
    tone_pwm.status |= TONE_PLAYNG;
}

void TONE_Stop(void)
{
    DMA_Cancel(&tone_pwm.dma);

	tone_pwm.tim->CR1 &= ~TIM_CR1_CEN;  // Stop timer
    tone_pwm.tim->CNT = tone_pwm.tim->ARR;   // Force idle

    tone_pwm.ptone = NULL;
    tone_pwm.cb = NULL;
    tone_pwm.status = TONE_IDLE;
}

/**
 * @brief Plays a melody composed of multiple tones.
 * The last tone on melody must have duration of zero
 *
 * FIX: Calling this multiple times with same parameter
 * will play different melodies.
 * Make parameter tones constant or avoid storing
 * convertion
 *
 * @param tones : pointer to tones array.
 * */
uint8_t TONE_Play(const tone_t *tone){

    if(tone){
        TONE_Start(tone->f, tone->d);
        tone_pwm.ptone = tone;
    }

    return tone_pwm.status;
}

/**
 * @brief Change tone volume by changing
 * duty cycle
 *
 * @param level : Tone volume 0 - 100,
 *                > 100 query level
 *
 * @return : Current tone volume
 *
 * */
uint8_t TONE_Volume(uint8_t level){

	if(level <= 100){
		tone_pwm.volume = level;
        *tone_pwm.duty = (tone_pwm.period * level) / 100;
	}

	return tone_pwm.volume;
}

void TONE_SetCallback(void (*cb)(const tone_t **))
{
    tone_pwm.cb = cb;
}

