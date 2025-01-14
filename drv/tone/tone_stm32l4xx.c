
#include <stddef.h>
#include "stm32l4xx.h"
#include "tone.h"

#define BUZ_DEFAULT_VOLUME     	1
#define FREQ_TO_US(_F)          (1000000/_F)

#if defined(__NUCLEO_L412KB__)
#define BUZ_TIM                 TIM16
#define BZ_DMA_CH 				DMA1_Channel3
#define BZ_DMA_ISR 				DMA_ISR_TCIF3
#define BZ_DMA_IFCR 			DMA_IFCR_CGIF3
#define BZ_DMA_IRQ				DMA1_Channel3_IRQn
#define DMA_HANDLER_PROTOTYPE 	void DMA1_Channel3_IRQHandler(void)
#else
#define BUZ_TIM                 TIM1
#define BZ_DMA_CH 				DMA1_Channel5
#define BZ_DMA_ISR 				DMA_ISR_TCIF5
#define BZ_DMA_IFCR 			DMA_IFCR_CGIF5
#define DMA_HANDLER_PROTOTYPE 	void DMA1_Channel5_IRQHandler(void)
#endif

typedef struct {
    const tone_t *ptone;
    uint8_t volume;
    uint8_t status;
    void (*cb)(const tone_t**);
}pwm_tone_t;

static pwm_tone_t pwm_tone = {0};
static uint16_t tone_period;    // Period of PWM frequency in us
static uint16_t tone_duration;  // Tone duration in us


/**
 * @brief TIM1 update event DMA request handler
 *
 * */
DMA_HANDLER_PROTOTYPE{

    if(DMA1->ISR & BZ_DMA_ISR){

    	BZ_DMA_CH->CCR &= ~DMA_CCR_EN;      // Disable DMA channel
    	DMA1->IFCR |= BZ_DMA_IFCR;          // Clear EOT flag

        // Get next tone
        if(pwm_tone.cb){
            pwm_tone.cb(&pwm_tone.ptone);
        }else{
            // double test
            if(pwm_tone.ptone)
            pwm_tone.ptone++;
        }

		if(pwm_tone.ptone != NULL){
			if(pwm_tone.ptone->d != 0){
				// Load tone
				if(pwm_tone.ptone->f != 0){
					tone_period = FREQ_TO_US(pwm_tone.ptone->f) - 1; // Set PWM freq
					#if defined(__NUCLEO_L412KB__)
    				BUZ_TIM->CCER |= TIM_CCER_CC1NE;
					#else
					BUZ_TIM->CCER |= TIM_CCER_CC1E;              // Enable channel
					#endif
				}else{
					// TODO: FIX when frequency = 0
					// for now uses the previous played frequency
					// to calculate the muted duration
					#if defined(__NUCLEO_L412KB__)
    				BUZ_TIM->CCER &= ~TIM_CCER_CC1NE;
					#else
					BUZ_TIM->CCER &= ~TIM_CCER_CC1E;             // Disable channel (mute)
					#endif
				}

				tone_duration = (pwm_tone.ptone->d * 1000UL) / tone_period;

				BUZ_TIM->CCR1 = (tone_period * pwm_tone.volume) / 100; // Set duty

				BZ_DMA_CH->CNDTR = tone_duration;                 // Update tone duration
				BZ_DMA_CH->CCR |= DMA_CCR_EN;    				  // Enable DMA channel
				return;
        	}
        }

		// Tone ended
        TONE_Stop();
    }
}

/**
 * @brief
 * PA8 for TIM1_CH1
 * PB6 for TIM16_CH1N
 */
static inline void initPwmPin(void){
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
    uint32_t tmp = GPIOB->MODER & ~(GPIO_MODER_MODE6);
    GPIOB->MODER = tmp | GPIOB_PIN6_AF;
    tmp = GPIOB->AFR[0] & ~(GPIO_AFRL_AFSEL6);
    GPIOB->AFR[0] = tmp | GPIOB_PIN6_TIM16_CH1N;

#else
	//gpioInit(GPIOA, 8, GPO_AF | GPO_2MHZ);
	GPIOA->CRH = (GPIOA->CRH & ~(0x0F)) | (2 << 2) | (2 << 0);
#endif
}

/**
 * @brief Configure DMA
 */
static inline void initDMA(void){
#if defined(__NUCLEO_L412KB__)
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;         // Enable DMA1
	BZ_DMA_CH->CPAR = (uint32_t)&BUZ_TIM->ARR;  // Destination peripheral
	BZ_DMA_CH->CCR =
		DMA_CCR_MSIZE_0 |                       // 16bit Dst size
		DMA_CCR_PSIZE_0 |                       // 16bit src size
		DMA_CCR_DIR |                           // Read from memory
		DMA_CCR_TCIE;                           // Enable end of transfer interrupt

	DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~(DMA_CSELR_C1S)) | (4 << DMA_CSELR_C3S_Pos);
	NVIC_EnableIRQ(BZ_DMA_IRQ);
#else
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;               // Enable DMA1
	DMA1_Channel5->CPAR = (uint32_t)&BUZ_TIM->ARR;  // Destination peripheral
	DMA1_Channel5->CCR =
			DMA_CCR_MSIZE_0 |                       // 16bit Dst size
			DMA_CCR_PSIZE_0 |                       // 16bit src size
			DMA_CCR_DIR |                           // Read from memory
			DMA_CCR_TCIE;                           // Enable end of transfer interrupt
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);
#endif
}

/**
 * @brief Configure Timer
 */
static inline void initTimer(void){
#if defined(__NUCLEO_L412KB__)
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
#else
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
#endif

#ifdef BUZ_IDLE_HIGH
    BUZ_TIM->CR1 = 0;
    BUZ_TIM->CCMR1 = (7 << 4);                      // PWM mode 2
#else
    BUZ_TIM->CR1 = TIM_CR1_DIR;
    BUZ_TIM->CCMR1 = (6 << 4);                      // PWM mode 1
#endif
    BUZ_TIM->PSC = (SystemCoreClock/1000000) - 1;	// 1us clock
#if defined(__NUCLEO_L412KB__)
    BUZ_TIM->CCER = TIM_CCER_CC1NE;                 // Enable channel inverted
#else
	BUZ_TIM->CCER = TIM_CCER_CC1E;                  // Enable channel
#endif
    BUZ_TIM->BDTR |= TIM_BDTR_MOE;                  // Enable OC and OCN
    // Force idle state
    BUZ_TIM->ARR = 0xFFF;
    BUZ_TIM->CNT = BUZ_TIM->ARR;
    TONE_Volume(BUZ_DEFAULT_VOLUME);                // Low volume level
    BUZ_TIM->EGR |= TIM_EGR_UG;
    // Enable DMA Request
    BUZ_TIM->DIER |= TIM_DIER_UDE;
}
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
void TONE_Init(void){

    // Configure DMA
    initDMA();

    // Configure timer
    initTimer();

    // Configure pin
    initPwmPin();
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

    tone_period = FREQ_TO_US(freq) - 1;              // Get period in us
    tone_duration = (duration * 1000UL) / tone_period;   // Calculate duration in us

	BZ_DMA_CH->CMAR = (uint32_t)(&tone_period);  // Source memory
	BZ_DMA_CH->CNDTR = tone_duration;            // Number of transfers => duration
	BZ_DMA_CH->CCR |= DMA_CCR_EN;                // Enable DMA channel

	BUZ_TIM->ARR = tone_period;			// Set PWM frequency
	BUZ_TIM->CNT = BUZ_TIM->ARR;		// Force inactive state
	#if defined(__NUCLEO_L412KB__)
	BUZ_TIM->CCER |= TIM_CCER_CC1NE; 	// Enable channel
	#else
	BUZ_TIM->CCER |= TIM_CCER_CC1E;     // Enable channel
	#endif
	BUZ_TIM->CCR1 = (tone_period * pwm_tone.volume) / 100; // Set duty
    BUZ_TIM->CR1 |= TIM_CR1_CEN;        // Start Timer
    pwm_tone.status |= TONE_PLAYNG;
}

void TONE_Stop(void)
{
    BZ_DMA_CH->CCR &= ~DMA_CCR_EN; // Disable DMA channel
	BUZ_TIM->CR1 &= ~TIM_CR1_CEN;  // Stop timer
    BUZ_TIM->CNT = BUZ_TIM->ARR;   // Force idle

    pwm_tone.ptone = NULL;
    pwm_tone.cb = NULL;
    pwm_tone.status = TONE_IDLE;
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
        pwm_tone.ptone = tone;
    }

    return pwm_tone.status;
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
		pwm_tone.volume = level;
        BUZ_TIM->CCR1 = (BUZ_TIM->ARR * level)/100;
	}

	return pwm_tone.volume;
}

void TONE_Callback(void (*cb)(const tone_t **))
{
    pwm_tone.cb = cb;
}

