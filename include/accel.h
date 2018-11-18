#ifndef _accel_h_
#define _accel_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <spi.h>

typedef struct{
	int16_t x;
	int16_t y;
	int16_t z;
}Accel_Type;


uint8_t ACCEL_Init(void *dev);
uint8_t ACCEL_Read(Accel_Type *acc);

void ACCEL_SetInt(uint16_t threshold);

#ifdef __cplusplus
}
#endif

#endif /* _accel_h_ */
