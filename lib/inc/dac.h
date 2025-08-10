#ifndef _dac_h_
#define _dac_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    void *per;
    int16_t *buf;
    uint32_t len;
    uint32_t rate;
    uint32_t loop;
}dactype_t;

/**
 * @brief Power up DAC and configure default
 *      output pin
 * */
void DAC_Init(dactype_t *dac);

/**
 * @brief Power down DAC and configure output pin to reset state
 * 
 */
void DAC_DeInit(dactype_t *dac);

/**
 * @brief Start DAC
 * 
 */
void DAC_Start(dactype_t *dac);

/**
 * @brief Stops generating wave, resets DMA and buffer pointers
 * */
void DAC_Stop(dactype_t *dac);

/**
 * @brief Pause wave generation
 * 
 */
void DAC_Pause(dactype_t *dac);

/**
 * @brief 
 * 
 * @param dac 
 */
void DAC_Config(dactype_t *dac);

/**
 * @brief 
 * 
 * @param value 
 */
void DAC_Write(dactype_t *dac, uint16_t value);

/**
 * @brief 
 * 
 * @param dac 
 */
void DAC_UpdateRate(dactype_t *dac, uint32_t rate);

#ifdef __cplusplus
}
#endif

#endif /* _dac_h_ */