#ifndef _scapture_
#define _scapture_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Initializes capture block
 * 
 *  */
void SCAP_Init(void);

/**
 * @brief Starts a capture
 * 
 * \param dst : Pointer for destination buffer of captured values
 * \param size : Buffer size
 * \param duration : Duration of capture
 *  */
void SCAP_Start(uint16_t *dst, uint32_t size, uint32_t duration);

/**
 * @brief Stops a capture
 * 
 *  */
void SCAP_Stop(void);

/**
 * @brief Waits for a capture to end
 * \retval : number of remaining captured values
 *  */
uint32_t SCAP_Wait(void);

/**
 * @brief Checks is a capture has ended
 * 
 * \retval : 0 not ended otherwise finished
 *  */
uint8_t SCAP_HasEnded(void);

#ifdef __cplusplus
}
#endif

#endif