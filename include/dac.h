#ifndef _dac_h_
#define _dac_h_

#include <stdint.h>

/**
 * @brief Initialize DAC
 * */
void DAC_Init(void);

/**
 * @brief Set update rate
 * @ param fq  clock frequency for DMA timer
 * */
void DAC_SetRate(uint32_t fq);

/**
 * @brief
 * @param[in] buf   Pointer to buffer containing wave data
 * @param[in] len   Number of data points in buffer
 * @param[in] rate  Frequency at data is retrieved from buffer
 * */
void DAC_StartSave(uint16_t *buf, uint32_t len, uint16_t rate);  

/**
 * @brief
 * */
void DAC_Stop(void);


void DAC_WriteBufer(uint16_t *buf, uint32_t len, uint16_t rate);



#endif /* _dac_h_ */