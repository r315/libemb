#ifndef _tone_h_
#define _tone_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct tone_init {
    void *per;          // peripheral to be configured
    uint8_t ch;         // Compare channel nr - 1
    uint8_t pin;        // Pin name
    uint8_t pin_idle;   // Pin idle state
    uint8_t volume;     // Initial volume
}tone_init_t;

typedef struct tone{
    uint16_t f;     // Frequency 50Hz to 50kHz
    uint16_t d;     // 1ms to 50000ms
}tone_t;

enum tone_e{
    TONE_IDLE = 0,
    TONE_PLAYNG,
    TONE_ERR
};

/**
 * @brief Initializes and configure
 * low level hardware, implemented at board level
 */
enum tone_e TONE_Init(void);

/**
 * @brief Called from TONE_Init with appropriated
 *       init structure
 * @param init
 * @return
 */
uint32_t TONE_PwmInit(tone_init_t *init_struct);

/**
 * @brief Starts playing a tone or a
 * sequence of tones
 *
 * @param tone : Pointer to tone sequence,
 *               NULL, to get state
 *
 * @return
 */
uint8_t TONE_Play(const tone_t *tone);

/**
 * @brief Starts playing a tone
 *
 * @param f : Tone frequency, 50Hz to 50kHz
 * @param d : Tone duration, 1ms to 50000ms
 */
void TONE_Start(uint16_t f, uint16_t d);
void TONE_Stop(void);

/**
 * @brief Change tone volume.
 * use value > 100 to get current volume
 *
 * @param level : Tone volume 0 to 100
 *
 * @return : Current tone volume
 *
 * */
uint8_t TONE_Volume(uint8_t level);

/**
 * @brief Set a callback for end of tone
 *
 * @param level : Tone volume 0 to 100
 *
 *
 * */
void TONE_SetCallback(void (*cb)(const tone_t **));

/**
 * @brief
 *
 * @return enum tone_e
 */
enum tone_e TONE_Status(void);

#ifdef __cplusplus
};
#endif
#endif
