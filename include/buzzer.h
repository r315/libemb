#ifndef _buzzer_h_
#define _buzzer_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct tone{
    uint16_t f;
    uint16_t d;
}tone_t;

//wholenote duration = (60 seconds * 1000ms * 4 beats full note) / (bpm of song)
#define RTTL_FULL_NOTE_DURATION 	240000UL
#define RTTL_BPM_TO_MS(_bpm)		(60000/_bpm)
#define RTTL_BASE_OCTAVE 			4  //4th octave

#define MAX_CCP_VALUE 1024          //
#define RTTL_PAUSE_VALUE -1

/**
 * @brief Basic tone generation on pin PA8 using TIM1_CH1
 * and DMA 
 * 
 * Buzzer timer is configured as PWM mode1 in downcount mode.
 * The counter starts from ARR register (top) that defines the frequency perioud in us,
 * and counts down, when matches CCR1 the output is set to high.
 * When the counter reaches zero, set the output to low and request a DMA transfer to ARR register.
 * On the last DMA transfer an interrupt is issued, that will configure the next tone periout to be 
 * loaded to ARR or stop the melody. 
 * 
 * */
void buzInit(void);

/**
 * @brief Plays a single tone for a given time
 * 
 * @param freq     : Tone fundamental frequency
 * @param duration : duration of tone in ms
 * */
void buzPlayTone(uint16_t freq, uint16_t duration);

/**
 * @brief Plays a melody composed of multiple tones.
 * The last tone on melody must have duration of zero
 * 
 * @param tones : pointer to tones array.
 * */
void buzPlay(tone_t *tones);

/**
 * @brief Change tone volume by changing 
 * duty cycle
 * 
 * @param level : Tone volume 0 to tone frequency period
 * 
 * @return : Current tone volume
 *  
 * */
uint8_t buzSetLevel(uint8_t level);

/**
 * @brief Waits for the end of tone(s)
 * Blocking call duh..
 * */
void buzWaitEnd(void);

/**
 * @brief
 *
 */
void buzPlayRttl(const char *song);

#ifdef __cplusplus
};
#endif
#endif
