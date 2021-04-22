
#if defined(STM32L412xx)
#include "main.h"
#else
#include <stm32f1xx.h>
#endif

#include "buzzer.h"

#define BUZ_TIM                 TIM1
#define BUZ_DEFAULT_VOLUME      1
#define FREQ_TO_US(_F)          (1000000/_F)

#define BUZ_PLAYING             (1 << 0)
#define BUZ_PLAYING_rtttl        (1 << 1)

#if defined(STM32L412xx)
#define BZ_DMA_CH DMA1_Channel6
#define BZ_DMA_ISR DMA_ISR_TCIF6
#define BZ_DMA_IFCR DMA_IFCR_CGIF6
#define DMA_HANDLER_PROTOTYPE void DMA1_Channel6_IRQHandler(void)


#else
#define BZ_DMA_CH DMA1_Channel5
#define BZ_DMA_ISR DMA_ISR_TCIF5
#define BZ_DMA_IFCR DMA_IFCR_CGIF5
#define DMA_HANDLER DMA1_Channel5_IRQHandler
#endif

typedef struct {
    tone_t *ptone;      // Pointer to tones
    uint8_t level;
    uint16_t period;	// Period of PWM frequency in us
    uint16_t duration;  // Tone duration in us
    volatile uint32_t flags;
}buzzer_t;

typedef struct{
	const char *codes;         // notes to play
	uint8_t o;	               // default octave
	uint8_t d;                 // default note duration
	uint16_t b;                // music rhythm
	uint16_t fullnoteduration; // duration of whole note in ms
	tone_t tone;
}rtttl_t;

static buzzer_t hbuz;
static rtttl_t rtttl;

const uint16_t rtttl_notes [] = {
		// 4th Octave
		220,  // A
		247,  // B
		262,  // C
		294,  // D
		330,  // E
		349,  // F
		392,  // G
		233,  // A#
		0,
		277,  // C#
		311,  // D#
		0,
		370,  // F#
		415,  // G#
};

void rtttlNextNote(void);
/**
 * @brief TIM1 update event DMA request handler
 *
 * */
DMA_HANDLER_PROTOTYPE{

    if(DMA1->ISR & BZ_DMA_ISR){

    	BZ_DMA_CH->CCR &= ~DMA_CCR_EN;      // Disable DMA channel
    	DMA1->IFCR |= BZ_DMA_IFCR;          // Clear EOT flag

		if(hbuz.ptone != NULL){
			if(hbuz.flags & BUZ_PLAYING_rtttl){
				rtttlNextNote();       // Get next note
			}else{
				hbuz.ptone++;     // Move to next tone
			}

			if(hbuz.ptone->d != 0){
				// Load tone
				if(hbuz.ptone->f != 0){
					hbuz.period = FREQ_TO_US(hbuz.ptone->f) - 1; // Set PWM freq
					BUZ_TIM->CCER |= TIM_CCER_CC1E;              // Enable channel
				}else{
					// TODO: FIX when frequency = 0
					// for now uses the previous played frequency
					// to calculate the muted duration
					BUZ_TIM->CCER &= ~TIM_CCER_CC1E;             // Disable channel (mute)
				}

				hbuz.duration = (hbuz.ptone->d * 1000UL) / hbuz.period;

				BUZ_TIM->CCR1 = (hbuz.period * hbuz.level) / 100; // Set duty

				BZ_DMA_CH->CNDTR = hbuz.duration;                 // Update tone duration
				BZ_DMA_CH->CCR |= DMA_CCR_EN;    				  // Enable DMA channel
				return;
        	}
        }

		// Tone ended
		BUZ_TIM->CR1 &= ~TIM_CR1_CEN; // Stop timer
		BUZ_TIM->CNT = BUZ_TIM->ARR;  // Force idle

		hbuz.flags &= ~(BUZ_PLAYING | BUZ_PLAYING_rtttl);  // Clear playing flag
		hbuz.ptone = NULL;
    }
}

/**
 * @brief Private helper to initiate tone generation
 * 
 * \param tone : pointer to tone to be played
 * */
static void buzStartTone(tone_t *tone){
	if(tone->f != 0){
		hbuz.period = FREQ_TO_US(tone->f) - 1;              // Get period in us
		hbuz.duration = (tone->d * 1000UL) / hbuz.period;   // Calculate duration in us
	}else{
		// TODO: FIX when frequency = 0
		hbuz.period = 0;
		hbuz.duration = tone->d;
	}

	BZ_DMA_CH->CMAR = (uint32_t)(&hbuz.period);  // Source memory
	BZ_DMA_CH->CNDTR = hbuz.duration;            // Number of transfers => duration
	BZ_DMA_CH->CCR |= DMA_CCR_EN;                // Enable DMA channel

	BUZ_TIM->ARR = hbuz.period;			// Set PWM frequency
	BUZ_TIM->CNT = BUZ_TIM->ARR;		// Force inactive state
	BUZ_TIM->CCER |= TIM_CCER_CC1E;     // Enable channel
	BUZ_TIM->CCR1 = (hbuz.period * hbuz.level) / 100; // Set duty
    BUZ_TIM->CR1 |= TIM_CR1_CEN;        // Start Timer
    hbuz.flags |= BUZ_PLAYING;
}

/**
 * @brief For st micros, configures PA8 as AF-PP, 2MHz
 */
static inline void initPwmPin(void){
#if defined(STM32L412xx)
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER = (GPIOA->MODER & ~(3 << (8*2))) | (2 << (8*2));
	GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~(3 << (8*2))) | (1 << (8*2));
	GPIOA->AFR[1] = (GPIOA->AFR[1] & ~(3 << (0*2))) | (GPIO_AF1_TIM1 << (0*2));
#else
	//gpioInit(GPIOA, 8, GPO_AF | GPO_2MHZ);
	GPIOA->CRH = (GPIOA->CRH & ~(0x0F)) | (2 << 2) | (2 << 0);
#endif
}

/**
 * @brief Configure DMA
 */
static inline void initDMA(void){
#if defined(STM32L412xx)
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;         // Enable DMA1
	BZ_DMA_CH->CPAR = (uint32_t)&BUZ_TIM->ARR;  // Destination peripheral
	BZ_DMA_CH->CCR =
		DMA_CCR_MSIZE_0 |                       // 16bit Dst size
		DMA_CCR_PSIZE_0 |                       // 16bit src size
		DMA_CCR_DIR |                           // Read from memory
		DMA_CCR_TCIE;                           // Enable end of transfer interrupt
	// Select TIM1_UP request for channel 6
	DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~(DMA_CSELR_C6S_Msk)) | (7 << DMA_CSELR_C6S_Pos);
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
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
 * @brief Configure Timer1
 */
static inline void initTimer(void){
#if defined(STM32L412xx)
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
#else

#endif

#ifdef BUZ_IDLE_HIGH
    BUZ_TIM->CR1 = 0;
    BUZ_TIM->CCMR1 = (7 << 4);                      // PWM mode 2
#else
    BUZ_TIM->CR1 = TIM_CR1_DIR;
    BUZ_TIM->CCMR1 = (6 << 4);                      // PWM mode 1
#endif
    BUZ_TIM->PSC = (SystemCoreClock/1000000) - 1;	// 1us clock
    BUZ_TIM->CCER = TIM_CCER_CC1E;                  // Enable channel
    BUZ_TIM->BDTR |= TIM_BDTR_MOE;                  // Enable OC and OCN
    // Force idle state
    BUZ_TIM->ARR = 0xFFF;
    BUZ_TIM->CNT = BUZ_TIM->ARR;
    buzSetLevel(BUZ_DEFAULT_VOLUME);                // Low volume level
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
void buzInit(void){

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
void buzPlayTone(uint16_t freq, uint16_t duration){
	tone_t tone;

	tone.f = freq;
    tone.d = duration;

    hbuz.ptone = NULL;       //Play single tone
    buzStartTone(&tone);
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
void buzPlay(tone_t *tones){

    hbuz.ptone = tones;

    // Start first tone
    buzStartTone(tones);   
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
uint8_t buzSetLevel(uint8_t level){

	if(level <= 100){
		hbuz.level = level;
        BUZ_TIM->CCR1 = (BUZ_TIM->ARR * level)/100;
	}

	return hbuz.level;
}

/**
 * @brief Waits for the end of tone(s)
 * Blocking call duh..
 * */
void buzWaitEnd(void){
    while(hbuz.flags & BUZ_PLAYING);
}

/**
 * @brief Private helper to convert string to integer
 */
uint16_t getInt(const char *str){
	uint16_t num = 0;
	while((*str > 0x2f) && (*str < 0x3a)){
		num = (num * 10) + (*str++ - '0');
	}
	return num;
}

/**
 * @brief
 *
 * */
void buzPlayRtttl(const char *melody){

	// Name section, skip it
	do{
		if(!(*melody)){
			return;     //not rtttl header
		}
	}while( (*melody++) != ':');

	// Default values section
	rtttl.d = 4;
	rtttl.o = 6;
	rtttl.b = 63;

	do{
		if(!(*melody))
			return;

		if( *melody == 'd'){
			melody += 2; // skip "d="
			rtttl. d = getInt(melody);
		}

		if(*melody == 'o'){
			melody += 2; // skip "o="
			rtttl.o = *melody - '0';
		}

		if(*melody == 'b'){
			melody += 2; // skip "b="
			rtttl.b = getInt(melody);
		}

	}while( (*melody++) != ':');

	rtttl.codes = melody;
	rtttl.fullnoteduration = RTTTL_BPM_TO_MS(rtttl.b) * 4;

	rtttlNextNote();     // Get first note

	hbuz.flags |= BUZ_PLAYING_rtttl;	// Set rtttl playing flag
	hbuz.ptone = &rtttl.tone;        // Set pointer to note

	buzStartTone(&rtttl.tone);
}


void rtttlNextNote(void){
//static uint16_t count = 0;
	uint16_t tmp;

	// Check if song has ended
	if(!(*rtttl.codes)){
		rtttl.tone.d = 0;
		return;
	}

	// Get duration (1,2,4,8,16,32)
	uint16_t duration = 0;
	while((*rtttl.codes > 0x2f) && (*rtttl.codes < 0x3a)){
		duration = (duration * 10) + (*rtttl.codes++ - '0');
	}

	if(!duration){
		duration = rtttl.d;
	}

	rtttl.tone.d = rtttl.fullnoteduration / duration;  // set duration in ms

	// Get pitch [a-f](#)
	if((*rtttl.codes) == 'p'){
		rtttl.codes++;
		rtttl.tone.f = 0;
	}else{
		tmp = *rtttl.codes - 'a';

		rtttl.codes++;

		if( (*rtttl.codes) == '#'){
			rtttl.codes++;
			tmp += 7;  // # notes are placed after normal notes in frequency table
		}
		rtttl.tone.f = rtttl_notes[tmp];  // set base note frequency
	}

	// Check for dotted note, can be before octave
	if( (*rtttl.codes) == '.'){
		rtttl.codes++;
		rtttl.tone.d += (rtttl.tone.d >> 1);
	}

	// Get octave (4,5,6,7)
	tmp = 0;
	if((*rtttl.codes > 0x2f) && (*rtttl.codes < 0x3a)){
		tmp = *(rtttl.codes++) - '0';
	}

	if(!tmp){
		tmp = rtttl.o;
	}

	while(tmp > RTTTL_BASE_OCTAVE){
		rtttl.tone.f <<= 1;  // Adjust note frequency
		tmp--;
	}

	//skip comma
	if(*rtttl.codes == ','){
		rtttl.codes++;
	}

	//count ++;
}

